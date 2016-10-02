/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */
#include "connection.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <algorithm>
#include <sys/socket.h>

/*
 * static methods
 */
void Connection::read_cb(bufferevent *bev, void *ctx) {
    Connection* conn = (Connection*) ctx;
    DEBUG("read");
    
    if (conn->bev == bev) {
        while (conn->stateLoop());
    } else {
        DEBUG("error read_cb");
    }
}

void Connection::write_cb(bufferevent *bev, void *ctx) {
    Connection* conn = (Connection*) ctx;
    evbuffer *output = bufferevent_get_output(bev);
    
    if (conn->readyToSend() && evbuffer_get_length(output) == 0) {
        DEBUG("response sent");
        Connection::release(&conn);
    }
}

void Connection::event_cb(bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_ERROR) {
        int err = EVUTIL_SOCKET_ERROR();

        if (err) {
            DEBUG("Socket error: %s\n", evutil_socket_error_to_string(err));
        }
    }

    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        Connection* conn = (Connection*) ctx;
        if (conn->bev == bev) {
            DEBUG("release conn");
            Connection::release(&conn);
        } else {
            DEBUG("error event_cb");
        }
    }
}

Connection* Connection::create(event_base* base, evutil_socket_t fd) {
    Connection* conn = new Connection(base, fd);
    return conn;
}

void Connection::release(Connection** conn) {
    delete *conn;
    *conn = NULL;
}
/*
 * end static methods
 */

Connection::Connection(event_base* base, evutil_socket_t fd) : state_(PARSE_HTTP_REQUEST_LINE), readyToSend_(false) {
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE );
    if (bev == NULL) {
        ERROR("bufferevent_socket_new error!");
        return;
    }
}

Connection::~Connection() {
    if (bev) {
        bufferevent_free(bev);
        bev = NULL;
    }
    DEBUG("~");
}

bool Connection::readyToSend() {
    return readyToSend_;
}

void Connection::readyToSend(bool flag) {
    readyToSend_ = flag;
}

void Connection::initEvent(void* arg) {
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, arg);
    bufferevent_enable(bev, EV_READ);
}

bool Connection::parseRequestLine() {
    assert(state_ == PARSE_HTTP_REQUEST_LINE);
    evbuffer *input = bufferevent_get_input(bev);

    char* line;
    size_t len;
    line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF);
    if (line) {
        char *token;
        const char *search = " ";
        token = strtok(line, search);
        if (token) {
            requestMethod_ = string(token);
            token = strtok(NULL, search);
            requestPath_ = string(token);
            token = strtok(NULL, search);
            requestVersion_ = string(token);
        }
        
        DEBUG("method: %s", requestMethod_.c_str());
        DEBUG("path: %s", requestPath_.c_str());
        DEBUG("version: %s", requestVersion_.c_str());

        if (requestMethod_.empty() || requestPath_.empty() || requestVersion_.empty()) {
            DEBUG("%s", line);
            state_ = ERROR_STATUS;
            free(line);
            return false;
        }

        transform(requestMethod_.begin(), requestMethod_.end(), requestMethod_.begin(), ::toupper);
        free(line);
        state_ = PARSE_HTTP_HEADER;
        return true;
    }

    DEBUG("%s", line);
    state_ = ERROR_STATUS;
    free(line);
    return false;
}

bool Connection::parseHeader() {
    assert(state_ == PARSE_HTTP_HEADER);
    evbuffer *input = bufferevent_get_input(bev);

    string key, value;
    char *token;
    const char *search = ":";
    char* line;
    size_t len;
    
    line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF);
    if (line == NULL) {
        return false;
    }
    
    token = strtok(line, search);
    if (token) {
        key = string(token);
        token = strtok(NULL, search);
        value = string(token);
    }

    if (key.empty()) {
        state_ = PARSE_HTTP_BODY;
        free(line);
        return true;
    }

    requestHeaders_.insert(make_pair(key, value));
    DEBUG("%s : %s", key.c_str(), value.c_str());

    free(line);
    return true;
}

bool Connection::parseBody() {
    assert(state_ == PARSE_HTTP_BODY);
    evbuffer *input = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(input);

    if (requestMethod_.compare("GET") || true) {
        state_ = PARSE_HTTP_FINISHED;
        return true;
    }

    if (len == 0) {
        DEBUG("msg_body is empty");
    } else {
        msgBody_ = (char*) evbuffer_pullup(input, len);
        DEBUG("msg_body:%s", msgBody_.c_str());
        evbuffer_drain(input, len);
        state_ = PARSE_HTTP_FINISHED;
    }

    state_ = PARSE_HTTP_FINISHED;
    return true;
}

bool Connection::stateLoop() {
    bool flag = false;
    switch (state_) {
        case PARSE_HTTP_REQUEST_LINE:
            flag = parseRequestLine();
            break;

        case PARSE_HTTP_HEADER:
            flag = parseHeader();
            break;

        case PARSE_HTTP_BODY:
            flag = parseBody();
            break;

        case PARSE_HTTP_FINISHED:
            flag = processRequest();
            break;

        case ERROR_STATUS:
            flag = false;
            break;
    }
    return flag;
}

bool Connection::processRequest() {

    assert(state_ == PARSE_HTTP_FINISHED);
    DEBUG("processRequest");

    if (requestMethod_ != "GET" && requestMethod_ != "POST" || false) { // test
        sendError(404, "Not Found");
    } else {
        sendOk("200 OK");
    }

    return false;
}

inline void Connection::sendOk(string msg) {
    evbuffer* output = bufferevent_get_output(bev);
    string str = "HTTP/1.1 200 OK\r\n";
    str += "Content-type: text/html\r\n";
    str += "\r\n";

    str += "<html> <body> <h1 align = center> " + msg + " </h1> </body>";

    evbuffer_add(output, str.c_str(), str.length());
    readyToSend(true);
}

inline void Connection::sendError(int code, string msg) {
    evbuffer* output = bufferevent_get_output(bev);
    string str = "HTTP/1.1 " + to_string(code) + " " + msg + "\r\n";
    str += "Content-type: text/html\r\n";
    str += "\r\n";

    str += "<html> <body> <h1 align = center>" + to_string(code) + " " + msg + "</h1> </body>";

    evbuffer_add(output, str.c_str(), str.length());
    readyToSend(true);
}

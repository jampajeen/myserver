/* 
 * Author: Thitipong Jampajeen <jampajeen@gmail.com>
 *
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"

using namespace std;

class Connection {
    static void event_cb(bufferevent *bev, short events, void *ctx);
    static void read_cb(bufferevent *bev, void *ctx);
    static void write_cb(bufferevent *bev, void *ctx);

    enum State {
        PARSE_HTTP_REQUEST_LINE,
        PARSE_HTTP_HEADER,
        PARSE_HTTP_BODY,
        PARSE_HTTP_FINISHED,
        PARSE_WS_FRAME_HEADER,
        PARSE_WS_FRAME_PAYLOAD,
        PARSE_WS_FINISHED,
        ERROR_STATUS
    };

private:
    bufferevent* bev;

    string requestMethod_;
    string requestPath_;
    string requestVersion_;
    map<string, string> requestHeaders_;
    string msgBody_;

    State state_;
    bool readyToSend_;

    bool parseRequestLine();
    bool parseHeader();
    bool parseBody();
    bool processRequest();

    void sendOk(string msg);
    void sendError(int code, string msg);

public:
    Connection(event_base*, evutil_socket_t);
    ~Connection();

    static Connection* create(event_base*, evutil_socket_t);
    static void release(Connection**);

    bool stateLoop();
    void initEvent(void* arg);

    void readyToSend(bool flag);
    bool readyToSend();
};

#endif /* CONNECTION_H */

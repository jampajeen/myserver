#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <signal.h>
#include <sys/wait.h>
#include "connection.h"
#include "common.h"

using namespace std;

int PORT = 5555;
evconnlistener* listener;

void accept_conn_cb(evconnlistener *listener, evutil_socket_t fd, sockaddr *address, int socklen, void *ctx) {
    event_base *base = evconnlistener_get_base(listener);
    
    Connection* connection = Connection::create(base, fd);
    connection->initEvent(connection);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(((sockaddr_in*) address)->sin_addr), ip, sizeof (ip));

    DEBUG("accept from:%s", ip);
}

void accept_error_cb(evconnlistener *listener, void *ctx) {
    event_base *base = evconnlistener_get_base(listener);

    int err = EVUTIL_SOCKET_ERROR();
    if (err) {
        DEBUG("error %d (%s) on the listener.\n", err, evutil_socket_error_to_string(err));
    }
    event_base_loopexit(base, NULL);
}

void kill_server(void) {
    event_base *base = evconnlistener_get_base(listener);
    if (event_base_loopexit(base, NULL)) {
        ERROR("Error shutting down server");
    }
}

static void sighandler(int signal) {
    int stat;

    DEBUG("Received signal %d: %s.  Shutting down.\n", signal, strsignal(signal));
    kill_server();

    // cleanup child process
    while (waitpid(-1, &stat, WNOHANG) > 0);
}

int main(int argc, char* argv[]) {

    /* Set signal handlers */
    sigset_t sigset;
    sigemptyset(&sigset);
    struct sigaction siginfo;
    siginfo.sa_handler = sighandler;
    siginfo .sa_mask = sigset;
    siginfo .sa_flags = SA_RESTART;
    sigaction(SIGINT, &siginfo, NULL);
    sigaction(SIGTERM, &siginfo, NULL);
    sigaction(SIGCHLD, &siginfo, NULL);

    event_base* base = event_base_new();
    if (base == NULL) {
        DEBUG("event_base_new error!");
        return 0;
    }
    
    int port = PORT;
    if (argc > 2) {
        port = atoi(argv[1]);
    }
    
    sockaddr_in sin;
    memset(&sin, 0, sizeof (sockaddr_in));
    sin . sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    listener = evconnlistener_new_bind(base, accept_conn_cb, 0, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*) & sin, sizeof (sin));

    if (listener == NULL) {
        ERROR("evconnlistener_new_bind error!");
        return 0;
    }

    evconnlistener_set_error_cb(listener, accept_error_cb);

    DEBUG("listening");
    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_base_free(base);
    return 0;
}
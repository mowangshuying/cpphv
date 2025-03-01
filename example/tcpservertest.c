

#include "hloop.h"
#include "hsocket.h"
#include "hlog.h"

// hloop_create_tcp_server -> on_accept -> hio_read -> on_recv -> hio_write

static void on_close(hio_t* io) {
    hlogd("on_close fd=%d error=%d", hio_fd(io), hio_error(io));
}

static void on_recv(hio_t* io, void* buf, int readbytes) {
    hlogd("on_recv fd=%d readbytes=%d", hio_fd(io), readbytes);
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    hlogd("[%s] <=> [%s]",
            SOCKADDR_STR(hio_localaddr(io), localaddrstr),
            SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));
    hlogd("< %.*s", readbytes, (char*)buf);
    // echo
    hlogd("> %.*s", readbytes, (char*)buf);
    hio_write(io, buf, readbytes);
}

static void on_accept(hio_t* io) {
    hlogd("on_accept connfd=%d", hio_fd(io));
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    hlogd("accept connfd=%d [%s] <= [%s]", hio_fd(io),
            SOCKADDR_STR(hio_localaddr(io), localaddrstr),
            SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));

    hio_setcb_close(io, on_close);
    hio_setcb_read(io, on_recv);
    hio_read_start(io);
}

int main(int argc, char** argv) {
        // hlog settings.
    hlog_set_file("");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    hloop_t* loop = hloop_new(0);
    hio_t* listenio = hloop_create_tcp_server(loop, "0.0.0.0", 8080, on_accept);
    if (listenio == NULL) {
        return -1;
    }

    hlogd("listenfd=%d", hio_fd(listenio));
    hloop_run(loop);
    hloop_free(&loop);
    return 0;
}

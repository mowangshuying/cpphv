#include "hloop.h"
#include "hsocket.h"
#include "hlog.h"


static void on_message(hio_t* io, void* buf, int len) {
    hlogd("onmessage: %.*s", len, (char*)buf);
}
static void on_connect(hio_t* io)
{
    hlogd("on_connect fd=%d", hio_fd(io));
    hio_setcb_read(io, on_message);
    hio_read_start(io);
    hio_write(io, "hello", 5);
}

static void on_close(hio_t* io)
{
    hlogd("on_close fd=%d error=%d", hio_fd(io), hio_error(io));
}


int main(int argc, char** argv)
{    
    hlog_set_file("");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    hloop_t* loop = hloop_new(0);
    hloop_create_tcp_client(loop, "127.0.0.1", 8080, on_connect, on_close);
    hloop_run(loop);
    hloop_free(&loop);
    return 0;
}
#include <hlog.h>
#include <TcpServer.h>
#include <string>
#include <iostream>

using namespace hv;
int main(int argc, char** argv)
{
    // hlog settings.
    hlog_set_file("");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    TcpServer server;
    int listenfd = server.createsocket(8080);
    if (listenfd < 0)
        return -1;

    server.onConnection = [](const SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            hlogd("%s connected! connfd=%d id=%d tid=%ld", peeraddr.c_str(), channel->fd(), channel->id(), currentThreadEventLoop->tid());
        } else {
            hlogd("%s disconnected! connfd=%d id=%d tid=%ld", peeraddr.c_str(), channel->fd(), channel->id(), currentThreadEventLoop->tid());
        }
    };

    server.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
        hlogd("< %.*s\n", (int)buf->size(), (char*)buf->data());
        channel->write(buf);
    };

    server.setThreadNum(4);
    server.setLoadBalance(LB_LeastConnections);
    server.start();

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            server.stop();
            break;
        }
    }

    return 0;
}
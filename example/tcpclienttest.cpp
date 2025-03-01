#include <iostream>
#include <TcpClient.h>
#include <htime.h>
#include <hlog.h>

using namespace hv;
int main(int argc, char** argv)
{
    // hlog settings.
    hlog_set_file("");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    TcpClient client;
    int connfd = client.createsocket(8080, "127.0.0.1");
    if (connfd < 0) {
        return -1;
    }

    hlogd("client connect to port=%d, connfd=%d", 8080, connfd);
    // connect cb
    client.onConnection = [&client](const SocketChannelPtr& channel){
        if (channel->isConnected())
        {
            hlogd("client connected, connfd=%d", channel->fd());
        }
    };

    client.onMessage = [&client](const SocketChannelPtr& channel, Buffer* buf){
        hlogd("< %.*s\n", (int)buf->size(), (char*)buf->data());
    };

    client.start();

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            client.stop();
            break;
        }
    }

    return 0;
}
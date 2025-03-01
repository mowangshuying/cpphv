#include <WebSocketServer.h>
#include <EventLoop.h>
#include <htime.h>
#include <hlog.h>

using namespace hv;

int main(int argc, char** argv)
{
    // hlog settings.
    hlog_set_file("testlog");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    HttpService httpService;
    httpService.GET("/ping", [](const HttpContextPtr& ctx) {
        return ctx->send("pong");
    });

    WebSocketService wsService;
    wsService.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
        hlogd("onopen: %s", channel->peeraddr().c_str());
    };

    wsService.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
        hlogd("onmessage: %s", msg.c_str());
    };

    wsService.onclose = [](const WebSocketChannelPtr& channel) {
        hlogd("onclose: %s", channel->peeraddr().c_str());
    };

    WebSocketServer wsServer;
    wsServer.port = 8080;
    wsServer.registerHttpService(&httpService);
    wsServer.registerWebSocketService(&wsService);
    wsServer.start();

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            wsServer.stop();
            break;
        }
    }

    return 0;
}
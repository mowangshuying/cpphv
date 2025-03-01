#include <WebSocketClient.h>
#include <hlog.h>
using namespace hv;
int main(int argc, char** argv)
{
        // hlog settings.
    hlog_set_file("testlog");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    WebSocketClient wsClient;
    wsClient.onopen = [&wsClient]() {
        hlogd("onopen");
        wsClient.send("hello websocket!");
    };

    wsClient.onclose = [&wsClient]() {
        hlogd("onclose");
    };

    wsClient.onmessage = [&wsClient](const std::string& msg) {
        hlogd("onmessage: %s", msg.c_str());
    };

    // reconnect: 1,2,4,8,10,10,10...
    reconn_setting_t reconn;
    reconn_setting_init(&reconn);
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;
    wsClient.setReconnect(&reconn);

    wsClient.open("ws://127.0.0.1:8080");

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            wsClient.stop();
            break;
        }
    }

    return 0;
}
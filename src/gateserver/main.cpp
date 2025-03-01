#include <GateServer.hpp>
int main(int argc, char** argv)
{
    hlog_set_file("gateServer");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    cpphv::GateServer gateServer;
    gateServer.init();
    gateServer.run();

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            gateServer.stop();
            break;
        }
    }
}
#include <LinkServer.hpp>
int main(int argc, char** argv)
{
    hlog_set_file("linkServer");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    cpphv::LinkServer linkServer;
    linkServer.init();
    linkServer.run();

    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "exit") {
            linkServer.stop();
            break;
        }
    }

    return 0;
}
#include <hlog.h>

int main(int argc, char** argv)
{
    // hlog settings.
    hlog_set_file("testlog");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    // use hlog;
    hlogd("hlog d!");
    hlogi("hlog i!");
    return 0;
}
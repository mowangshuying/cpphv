#include <hlog.h>
#include <hloop.h>
#include <hbase.h>

void on_timer(htimer_t* timer)
{
    static int times = 0;
    hlogd("ontimer times=%d", ++times);
    // htimer_add(hevent_loop(timer), on_timer, 1000, 1);
}
void on_timer_quit_loop(htimer_t* timer)
{
    hlogd("quit loop");
    hloop_stop(hevent_loop(timer));
}


int main(int argc, char** argv)
{
        // hlog settings.
    hlog_set_file("testlog");
    hlog_set_format(DEFAULT_LOG_FORMAT);
    hlog_set_handler(stdout_logger);
    hlog_set_level(LOG_LEVEL_DEBUG);

    // create hloop
    hloop_t* loop = hloop_new(0);
    // loop now
    hlogd("hloop now time=%llus", hloop_now(loop));

    // just run 1 time. 
    //htimer_add(loop, on_timer, 1000, 1);

    // just run 10 times.
    //htimer_add(loop, on_timer, 1000, 10);

    // just run 100 times.
    htimer_add(loop, on_timer, 1000, 100);

    // quit loop after 30 seconds.
    htimer_add(loop, on_timer_quit_loop, 30 * 1000, 1);

    // loop run
    hloop_run(loop);
    // free hloop
    hloop_free(&loop);
    return 0;
}
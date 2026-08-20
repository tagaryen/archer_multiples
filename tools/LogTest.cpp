#include "../libcommon/Logger.h"



int main() {
    int i = 1;
    console_out("this is %s  %d", "out", i++);
    console_err("this is %s  %d", "err", i++);
    console_warn("this is %s  %d", "warn", i++);

    LOG_trace("log %s  %d", "trace", i++);
    LOG_debug("log %s  %d", "debug", i++);
    LOG_info("log %s  %d", "info", i++);
    LOG_warn("log %s  %d", "warn", i++);
    LOG_error("log %s  %d", "error", i++);
    LOG_fatal("log %s  %d", "fatal", i++);

    std::chrono::seconds s(1);
    std::this_thread::sleep_for(s);

    return 0;
}
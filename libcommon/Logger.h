#pragma once

#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <iostream>
#include <ctime>


namespace fs 
{
namespace common 
{
enum { LOG_LEVEL_NONE, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARN, LOG_LEVEL_ERROR, LOG_LEVEL_FATAL };

class Logger 
{
public:
    static Logger& instance() {
        static Logger instance(LOG_LEVEL_INFO, "logs"); 
        return instance;
    }
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void setLevel(int level) {m_level = level;}

    void setLogPath(std::string const& path);

    void log(int level, const char *srcFile, const int line, const char *fmt, ...);

    void log_console(int level, const char *fmt, ...);
private:

    Logger(int level, std::string const& path):m_level(level) {
        setLogPath(path);
    }
    ~Logger() {
        m_running = false;
    };

    void worker();

    int m_level;
    std::string m_path;

    bool m_running = false;
    mutable std::mutex m_mutex;                // mutable 以允许 const 方法加锁
    std::condition_variable m_cond;
    std::deque<std::string> m_lines;
};
}
}

#define console_out(...) fs::common::Logger::instance().log_console(fs::common::LOG_LEVEL_INFO, __VA_ARGS__)
#define console_warn(...) fs::common::Logger::instance().log_console(fs::common::LOG_LEVEL_WARN, __VA_ARGS__)
#define console_err(...) fs::common::Logger::instance().log_console(fs::common::LOG_LEVEL_ERROR, __VA_ARGS__)



#define LOG_trace(...) fs::common::Logger::instance().log(fs::common::LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_debug(...) fs::common::Logger::instance().log(fs::common::LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_info(...)  fs::common::Logger::instance().log(fs::common::LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_warn(...)  fs::common::Logger::instance().log(fs::common::LOG_LEVEL_WARN, __FILE__, __LINE__,  __VA_ARGS__)
#define LOG_error(...) fs::common::Logger::instance().log(fs::common::LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_fatal(...) fs::common::Logger::instance().log(fs::common::LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)







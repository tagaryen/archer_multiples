#include "Logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace fs::common;

static void DO_mkdir(const char *path) {
    if(access(path, 0)) {
#ifdef _WIN32
        mkdir(path);
#else
        mkdir(path, S_IRWXU);
#endif
    }
}

static void DO_mkdirs(const char *path) {
    size_t len = strlen(path);
    char *dir = (char *) malloc(len + 1);
    memcpy(dir, path, len);
    dir[len] = '\0';
    if(dir[len - 1] == 92 || dir[len - 1] == 47) {
        dir[len - 1] = '\0';
        --len;
    }
    for(int i = 1; i < len; i++) {
        if(dir[i] == 47 || dir[i] == 92) {
            dir[i] = 0;
            DO_mkdir(dir);
            dir[i] = '/';
        }
    }
    DO_mkdir(dir);
}

static int IS_absolute_path(const char *path) {
#ifdef __WIN32
    return (isalpha(path[0]) && path[1] == ':') || (path[0] == 92 && path[1] == 92);
#else
    return (path[0] == '/');
#endif
}

static std::string GET_now_time() {
    std::time_t now = std::time(nullptr);
    // now += 8 * 3600;//时区问题
    std::tm *localTime = std::localtime(&now);
    char timestr[20];
    std::strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", localTime);
    std::string nowStr(timestr, 19);
    return std::move(nowStr);
}

void Logger::setLogPath(std::string const& path) {
    if(m_running) {
        return ;
    }
    if(path.empty()) {
        char rootPath[1024];
        if(getcwd(rootPath, 1023)) {}
        m_path = std::string(rootPath, strlen(rootPath));
        m_path += "/logs/";
    } else {
        size_t pathLen = path.length();
        m_path = path;
        if(IS_absolute_path(path.c_str())) {
            m_path = path;
        } else {
            char rootPath[1024];
            if(getcwd(rootPath, 1023)) {}
            m_path = std::string(rootPath, strlen(rootPath));
            //check path first char is '/' or '\'
            if(path[0] != 47 && path[0] != 92) {
                m_path += "/";
                m_path += path;
            } else {
                m_path += path;
            }
        }
        //add last '/' or '\'
        if(m_path[m_path.length() - 1] != 47 && m_path[m_path.length() - 1] != 92) {
            m_path += "/";
        }
    }
}

void Logger::worker() {
    
    DO_mkdirs(m_path.c_str());
    std::string time = GET_now_time();
    
    std::string currentFile = m_path + time.substr(0, 10) + ".log";
    std::ofstream fout(currentFile, std::ios::out | std::ios::app);
    if(!fout.is_open()) {
        perror("can not open log file");
    }

    std::chrono::seconds timeout(1);
    std::string userLogMsg;
    
    m_running = true;
    while(m_running) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_cond.wait_for(lock, timeout,  [this] { return !m_lines.empty();})) {
            userLogMsg = std::move(m_lines.front());
            m_lines.pop_front();
            
            // eg. INFO [2025-09-10 11:12:45][...] ...
            std::string logFile = m_path + userLogMsg.substr(userLogMsg.find_first_of('[') + 1, 10) + ".log";
            if(currentFile != logFile) {
                fout.close();
                fout.open(logFile, std::ios::out | std::ios::app);
                currentFile = std::move(logFile);
            }
            fout<<userLogMsg.c_str();
            fout<<"\n";
            fout.flush();
        }
    }
    fout.close();
}

void Logger::log(int level, const char *srcFile, const int line, const char *fmt, ...) {
    if(level < m_level) {
        return ;
    }

    std::string logMsg;
    logMsg.reserve(512);
    switch (level)
    {
    case LOG_LEVEL_TRACE:
        logMsg += "TRACE";
        break;
    case LOG_LEVEL_DEBUG:
        logMsg += "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        logMsg += "INFO";
        break;
    case LOG_LEVEL_WARN:
        logMsg += "WARN";
        break;
    case LOG_LEVEL_ERROR:
        logMsg += "ERROR";
        break;
    case LOG_LEVEL_FATAL:
        logMsg += "FATAL";
        break;
    default:
        logMsg += "TRACE";
        break;
    }
    logMsg += " [";
    logMsg += GET_now_time();
    logMsg += ']';
    if(srcFile) {
        logMsg += '[';

        size_t len = strlen(srcFile);
        long off = len - 1;
        while(off >= 0 && srcFile[off] != 47 && srcFile[off] != 92) {
            --off;
        }
        logMsg.append(srcFile + off + 1, len - off - 1);
        logMsg += ':';
        logMsg += std::to_string(line);
        logMsg += ']';
    }
    logMsg += ' ';

    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char* userLine = (char*)malloc(size + 1);
    va_start(args, fmt);
    vsnprintf(userLine, size + 1, fmt, args);
    va_end(args);

    logMsg.append(userLine, size + 1);
    free(userLine);

    if(!m_running) {
        std::thread run(&Logger::worker, this);
        run.detach();
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_lines.emplace_back(logMsg);
}


void Logger::log_console(int level, const char *fmt, ...) {
    if(level < m_level) {
        return ;
    }

    if(level >= LOG_LEVEL_WARN) {
        fprintf(stdout, "%s", "\033[31m");
    }

    std::string logMsg;
    logMsg.reserve(50);
    switch (level)
    {
    case LOG_LEVEL_TRACE:
        logMsg += "TRACE";
        break;
    case LOG_LEVEL_DEBUG:
        logMsg += "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        logMsg += "INFO";
        break;
    case LOG_LEVEL_WARN:
        logMsg += "WARN";
        break;
    case LOG_LEVEL_ERROR:
        logMsg += "ERROR";
        break;
    case LOG_LEVEL_FATAL:
        logMsg += "FATAL";
        break;
    default:
        logMsg += "TRACE";
        break;
    }
    logMsg += " [";
    logMsg += GET_now_time();
    logMsg += "] ";

    // char* userLine = (char*)malloc(size);
    // va_start(args, fmt);
    // vsnprintf(userLine, size, fmt, args);
    // va_end(args);

    // logMsg.append(userLine);
    // free(userLine);
    
    va_list args;
    fprintf(stdout, "%s", logMsg.c_str());
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");
    if(level >= LOG_LEVEL_WARN) {
        fprintf(stdout, "%s", "\033[0m");
    }
    fflush(stdout);
}



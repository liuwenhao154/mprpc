#pragma once
#include "lockqueue.h"

// Mprpc Logger system header file
enum LogLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class Logger
{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string message);
    ~Logger();
private:
    int m_logLevel;
    LockQueue<std::string> m_logQueue;
    std::thread m_thread;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

#define LOG_INFO(msg, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(LOG_INFO); \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), msg, ##__VA_ARGS__); \
        logger.Log(buffer); \
    }while (0);

#define LOG_ERROR(msg, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(LOG_ERROR); \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), msg, ##__VA_ARGS__); \
        logger.Log(buffer); \
    }while (0);

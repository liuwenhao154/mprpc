#include "logger.h"
#include <iostream>
#include <thread>

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : m_exit(false){
    m_thread = std::thread([this]() {
        while (true) {
            time_t now = time(nullptr);
            tm* localTime = localtime(&now);
            char file_name[128];
            sprintf(file_name, "%04d-%02d-%02d-log.txt",
                    localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday);

            FILE* log_file = fopen(file_name, "a+");
            if (log_file == nullptr) {
                std::cerr << "Failed to open log file: " << file_name << std::endl;
                continue;
            }

            std::string message = m_logQueue.Pop();
            if (m_exit && message.empty()) break; // 退出条件
            if (message.empty()) continue; // 忽略空消息

            char time_buffer[64];
            sprintf(time_buffer, "%d:%d:%d => [%s]", 
                    localTime->tm_hour,
                    localTime->tm_min,
                    localTime->tm_sec, 
                    (m_logLevel == LOG_INFO) ? "INFO" : "ERROR");
                    
            message.insert(0, time_buffer);

            fputs(message.c_str(), log_file);
            fclose(log_file);
        }
    });
}

Logger::~Logger(){
    // Wait for the logging thread to finish
    m_logQueue.Exit(); // 唤醒所有等待线程并设置退出标志
    m_exit = true;
    if (m_thread.joinable()) m_thread.join();
}

void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::Log(std::string message)
{
    // Here you can add logic to filter messages based on m_logLevel
    m_logQueue.Push(message);
}
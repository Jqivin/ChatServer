#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : running(true)
{
    fileDate = currentDate();

    
}

Logger::~Logger()
{
    stop();
}

std::string Logger::currentDate() const
{
    time_t t = time(nullptr);
    struct tm tmInfo;
#ifdef _WIN32
    localtime_s(&tmInfo, &t);
#else
    localtime_r(&t, &tmInfo);
#endif

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tmInfo);
    return std::string(buf);
}

std::string Logger::toString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::log(const std::string& level,
    const std::string& file,
    int line,
    const std::string& func,
    const std::string& msg)
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    
    // 获取毫秒部分
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // 格式化时间
    char timebuf[32];
    struct tm timeinfoBuffer;
    
#ifdef _WIN32
    localtime_s(&timeinfoBuffer, &t);
#else
    localtime_r(&t, &timeinfoBuffer);
#endif
    
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &timeinfoBuffer);
    
    // 组合日期时间和毫秒
    std::string timeStr = std::string(timebuf) + "." + 
                         std::to_string(milliseconds.count());
    
    // 提取文件名
    int iLast = file.find_last_of("/");
    std::string filename;
    if (iLast != std::string::npos) {
        filename = file.substr(iLast + 1);
    } else {
        filename = file;
    }
    
    // 构建日志行
    std::string logLine =
        "[" + timeStr + "] [" + level + "] [" + filename + "] [" + 
        func + ":" + std::to_string(line) + "] " + msg + "\n";
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        logQueue.push(logLine);
    }
    cv.notify_one();
}

void Logger::writeThreadFunc()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !logQueue.empty() || !running; });

        while (!logQueue.empty())
        {
            if (currentDate() != fileDate)
            {
                fileDate = currentDate();
                logFile.close();
                logFile.open("client_" + fileDate + ".log", std::ios::app);
            }

            std::string msg = logQueue.front();
            logQueue.pop();

            logFile << msg;
            logFile.flush(); 
        }
    }
}


void Logger::stop()
{
    if (!running) return;
    running = false;
    cv.notify_one();
    if (writeThread.joinable())
        writeThread.join();
    if (logFile.is_open())
        logFile.close();
}

void Logger::StartLogger(const std::string& strPath)
{
    std::string filename = strPath + "/JQChat_" + fileDate + ".log";

    logFile.open(filename, std::ios::out | std::ios::app);

    if(!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }
    LOG_INFO("Logger started.");
    writeThread = std::thread(&Logger::writeThreadFunc, this);
}

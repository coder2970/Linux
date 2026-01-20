#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem> // c++17
#include "mutex.hpp"
#include <memory>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <unistd.h>

// 查看日志等级
enum class LogLevel
{
    DEBUG,  // 测试
    INFO,   // 常规输出
    WARING, // 出错但不影响运行
    ERROR,  // 出错运行退出
    FATAL   // 重大问题
};

std::string LogToSring(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return "Debug";
    case LogLevel::INFO:
        return "Info";
    case LogLevel::WARING:
        return "Waring";
    case LogLevel::ERROR:
        return "Error";
    case LogLevel::FATAL:
        return "Fatal";
    default:
        return "Unknown";
    }
}

// 日志刷新问题 -- 假设已经有了一条完整的日志
// string --> 显示器/文件

// 基类方法
class LogStragegy
{
public:
    virtual ~LogStragegy() = default;
    virtual void SyncLog(const std::string &logmessage) = 0; // 刷新策略
};

// 1.显示器刷新
class ConsoleLogStragegy : public LogStragegy
{
public:
    ~ConsoleLogStragegy()
    {
    }
    void SyncLog(const std::string &logmessage) override
    {
        // 加锁 显示器是公共资源
        {
            LockGuard lockguard(&_lock);
            std::cout << logmessage << std::endl;
        }
    }

private:
    Mutex _lock;
};

const std::string default_dir = "Log";
const static std::string default_filename = "test.log";
// 2.文件刷新
class FileLogStragegy : public LogStragegy
{
public:
    FileLogStragegy(const std::string &dir_path_name = default_dir, const std::string &filename = default_filename)
        : _dir_path_name(dir_path_name), _filename(filename)
    {
        {
            LockGuard lockguard(&_lock);
            if (std::filesystem::exists(_dir_path_name))
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_dir_path_name);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    ~FileLogStragegy()
    {
    }

    void SyncLog(const std::string &logmessage) override
    {
        LockGuard lockguard(&_lock);
        std::string target = _dir_path_name;
        target += "/";
        target += _filename;

        std::ofstream out(target.c_str(), std::ios::app);
        if (!out.is_open())
        {
            return;
        }
        out << logmessage << "\n";
        out.close();
    }

private:
    std::string _dir_path_name; // 日志写在的路径 Log
    std::string _filename;      // Log/hello.log
    Mutex _lock;                // 创建文件也需要加锁
};

std::string GetCurrentTime()
{
    // 1. 获取时间戳
    time_t current_time = time(nullptr);

    // 2.把时间戳转化为具体时间
    struct tm current_tm;
    localtime_r(&current_time, &current_tm);

    // 3.把时间转化为字符串
    char time_buffer[128];
    snprintf(time_buffer, sizeof(time_buffer), "%4d-%02d-%02d %02d:%02d:%02d",
             current_tm.tm_year + 1900,
             current_tm.tm_mon + 1,
             current_tm.tm_mday,
             current_tm.tm_hour,
             current_tm.tm_min,
             current_tm.tm_sec);
    return time_buffer;
}

// 1.刷新策略 2.打印完整日志
class Logger
{
public:
    Logger()
    {
    }

    // 激活策略
    void EnableFileLogStragegy()
    {
        _strategy = std::make_unique<FileLogStragegy>();
    }
    void EnableConsoleLogStragegy()
    {
        _strategy = std::make_unique<ConsoleLogStragegy>();
    }

    // 生成一条完整的日志
    // 采用内部类的方式
    class LogMessage
    {
    public:
        LogMessage(LogLevel level, const std::string &filename, int line, Logger &logger)
            : _current_time(GetCurrentTime()),
              _level(level),       /*日志等级用户传入*/
              _pid(getpid()),      /**/
              _filename(filename), /*文件名也需要外部传入,这里无法判断*/
              _line(line),         /*行号也需要外部传入*/
              _logger(logger)

        {
            std::stringstream ss;
            // 按照格式输出 : [时间] [日志等级] [进程pid] [对应日志文件名] [行号] - "消息内容"
            // 左半部分:
            ss << "[" << _current_time << "] "
               << "[" << LogToSring(_level) << "] "
               << "[" << _pid << "] "
               << "[" << _filename << "] "
               << "[" << _line << "] "
               << "- ";
            _loginfo = ss.str();
        }

        // 右半部分
        // 重载运算符<<
        template <class T>
        LogMessage &operator<<(const T &info)
        {
            std::stringstream ss;
            ss << info;
            _loginfo += ss.str(); // 至此获得右半部分的第一个参数
            return *this;
        }
        // 获得第一个参数后如何继续获得? --> 把返回值设置为LogMessage&的作用?(为什么把当前对象直接返回)
        // ----> 见 Logger 类中 的重载

        ~LogMessage()
        {
            if (_logger._strategy)
            {
                _logger._strategy->SyncLog(_loginfo);
            }
        }

    private:
        std::string _current_time; // 日志时间
        LogLevel _level;           // 日志等级
        pid_t _pid;                // 进程pid
        std::string _filename;     // 日志文件名
        int _line;                 // 行号

        std::string _loginfo; // 一条合并完成的完整的日志信息
        Logger &_logger;      // 提供刷新策略
        /*
        为什么在logmessage中要带一个外部类的引用?
        在logmessage在调用自己的析构函数时,在析构一个日志信息(logmessage对象)时,要把一条完整的日志信息
        刷新到外设中(可以是文件,也可以是显示器)
        目的就是让临时变量在析构时 统一做刷新
        */
    };

    LogMessage operator()(LogLevel level, const std::string &filename, int line)
    {
        return LogMessage(level, filename, line, *this);
        // 返回临时对象!
    }

    ~Logger()
    {
    }

private:
    std::unique_ptr<LogStragegy> _strategy; // 策略指针
};

Logger logger; // 定义成全局,在头文件中直接include即可

// 如何调用
#define LOG(level) logger(level, __FILE__, __LINE__)
// 模式选择
#define EnableConsoleLogStragegy() logger.EnableConsoleLogStragegy()
#define EnableFileLogStragegy() logger.EnableFileLogStragegy()
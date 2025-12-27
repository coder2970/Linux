#include "Logger.hpp"
int main()
{
    // RAII风格的日志构建与输出刷新的过程
    EnableFileLogStragegy();
    LOG(LogLevel::ERROR) << "hello world" << "1234" << 3.14;
    LOG(LogLevel::WARING) << "hello world" << "1234" << 3.14;
    LOG(LogLevel::FATAL) << "hello world" << "1234" << 3.14;
    LOG(LogLevel::INFO) << "hello world" << "1234" << 3.14;
    return 0;
}
// int main()
// {
//     // std::unique_ptr<LogStragegy> loggerptr = std::make_unique<ConsoleLogStragegy>();
//     // loggerptr->SyncLog(GetCurrentTime());
//     // sleep(1);
//     // loggerptr->SyncLog(GetCurrentTime());
//     // sleep(1);
//     // loggerptr->SyncLog(GetCurrentTime());
//     // sleep(1);
//     // loggerptr->SyncLog(GetCurrentTime());
//     // sleep(1);
//     // loggerptr->SyncLog(GetCurrentTime());
//     // sleep(1);

//     return 0;
// }
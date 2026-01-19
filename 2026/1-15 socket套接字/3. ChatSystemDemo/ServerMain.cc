#include "ChatServer.hpp"
#include "Route.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <memory>

void Usage(std::string message)
{
    std::cerr << "usage:" << message << " local_port" << std::endl;
}

// void Chat(int sockfd, std::string message, InetAddr addr)
// {
//     LOG(LogLevel::DEBUG) << "sockfd: " << sockfd;
//     LOG(LogLevel::DEBUG) << "message: " << message;
//     LOG(LogLevel::DEBUG) << "client info: " << addr.ToString();
//     sendto(sockfd, message.c_str(), message.size(), 0, addr.Addr(), addr.Length());
// }
using task_t = std::function<void()>;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    EnableConsoleLogStragegy(); // 启用控制台日志刷新策略 - 向显示器打印日志

    uint16_t port = std::stoi(argv[1]);
    // 1.路由消息转发功能
    std::unique_ptr<Route> r = std::make_unique<Route>();

    // 2.线程池对象 完成任务
    ThreadPool<task_t> *tp = ThreadPool<task_t>::GetInstance();

    // 3.服务器对象
    // 创建对象
    std::unique_ptr<ChatServer> usvr = std::make_unique<ChatServer>(port, [&r, &tp](int sockfd, std::string message, InetAddr addr)
                                                                    { 
            task_t task = std::bind(&Route::RouteMessageToAll, r.get(), sockfd, message, addr); 
            tp->Enqueue(task); });

    usvr->Init(); // 初始化

    usvr->Start(); // 启动
    return 0;
}
#include "ChatServer.hpp"
#include <iostream>
#include <memory>

void Usage(std::string message)
{
    std::cerr << "usage:" << message << " local_port" << std::endl;
}

void Chat(int sockfd, std::string message, InetAddr addr)
{
    LOG(LogLevel::DEBUG) << "sockfd: " << sockfd;
    LOG(LogLevel::DEBUG) << "message: " << message;
    LOG(LogLevel::DEBUG) << "client info: " << addr.ToString();
    sendto(sockfd, message.c_str(), message.size(), 0, addr.Addr(), addr.Length());
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    EnableConsoleLogStragegy(); // 启用控制台日志刷新策略 - 向显示器打印日志

    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<ChatServer> usvr = std::make_unique<ChatServer>(port, Chat);
    // // 创建对象
    // std::unique_ptr<ChatServer> usvr = std::make_unique<ChatServer>(port,
    //                                                                 [](const std::string &word, const std::string &who_ip, uint16_t who_port) -> std::string
    //                                                                 {  });

    usvr->Init(); // 初始化

    usvr->Start(); // 启动
    return 0;
}
#include "UdpServer.hpp"
#include <iostream>
#include <memory>

// udp_server server_ip server_port

void Usage(std::string message)
{
    std::cerr << "usage:" << message << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);

    EnableConsoleLogStragegy(); // 启用控制台日志刷新策略 - 向显示器打印日志
    // 创建对象
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(ip, port);

    usvr->Init(); // 初始化

    usvr->Start(); // 启动
    return 0;
}
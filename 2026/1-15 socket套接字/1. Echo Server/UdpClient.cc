// Udp客户端 暂不做封装
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "mutex.hpp"

// ./udp_client server_ip server_port
void Usage(std::string message)
{
    std::cerr << "usage:" << message << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(2);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Client create socket error" << std::endl;
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // 客户端不需要显示绑定IP和端口号,会在sendto发送消息时,由系统自动完成隐式绑定(随机绑定端口号)
    while (true)
    {
        std::cout << "Please Enter:";
        std::string line;
        std::getline(std::cin, line);
        // 发送数据
        sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr *)&server, sizeof(server));

        // 读取数据
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        buffer[0] = 0;
        int m = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&tmp, &len);
        if (m > 0)
        {
            buffer[m] = 0;
            // LOG(LogLevel::DEBUG) << "client say: " << buffer;
            std::cout << buffer << std::endl;
        }
    }
    return 0;
}
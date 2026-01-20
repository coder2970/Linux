#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "mutex.hpp"

int sockfd = -1;
std::string server_ip;
uint16_t server_port;
void Usage(std::string message)
{
    std::cerr << "usage:" << message << " server_ip server_port" << std::endl;
}
void InitClient(const std::string &server_ip, uint16_t server_port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Client create socket error" << std::endl;
        exit(1);
    }
}
void recver()
{
    while (true)
    {
        // 读取数据
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        buffer[0] = 0;
        int m = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&tmp, &len);
        if (m > 0)
        {
            buffer[m] = 0;
            std::cerr << buffer << std::endl;
        }
    }
}
void sender()
{
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    while (true)
    {
        std::cout << "Please Enter:";
        std::string line;
        std::getline(std::cin, line);
        // 发送数据
        sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr *)&server, sizeof(server));
    }
}

// ./ chat_client ip port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(2);
    }

    server_ip = argv[1];
    server_port = std::stoi(argv[2]);
    InitClient(server_ip, server_port);

    std::thread trecv(recver);
    std::thread tsend(sender);

    trecv.join();
    tsend.join();

    return 0;
}
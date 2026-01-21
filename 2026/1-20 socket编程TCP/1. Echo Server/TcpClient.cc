#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.hpp"
#include "InetAddr.hpp"

void Usage(const std::string message)
{
    std::cerr << "Usage : " << message << " server_ip server_port" << std::endl;
}
// ./tcp_client server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "create client sockfd error" << std::endl;
        exit(SOCKET_CRATE_ERR);
    }

    // tcp 客户端是否需要显示绑定
    // 客户端自己socket地址,让本地os自主随机选择,尤其是端口号
    // 客户端要向服务器端发起连接
    InetAddr server(serverport, serverip);
    if (connect(sockfd, server.Addr(), server.Length()) != 0)
    {
        std::cerr << "connect server error" << std::endl;
        exit(SOCKET_CONNECT_ERR);
    }
    std::cout << "connect" << server.ToString() << " success" << std::endl;

    while (true)
    {
        std::cout << "Please Enter: ";
        std::string line;
        std::getline(std::cin, line);

        ssize_t n = write(sockfd, line.c_str(), line.size());
        if (n >= 0)
        {
            char buffer[1024];
            ssize_t m = read(sockfd, buffer, sizeof(buffer) - 1);
            if (m > 0)
            {
                buffer[m] = 0;
                std::cout << buffer << std::endl;
            }
        }
    }

    return 0;
}
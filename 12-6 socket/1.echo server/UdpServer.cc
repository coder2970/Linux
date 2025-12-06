#include "UdpServer.hpp"
#include <iostream>
#include <memory>

void Usage(std::string proc)
{
    std::cerr << "usage" << proc << " serverip serverport" << std::endl;
}
// ./UdpServer serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(2);
    }

    uint16_t port = std::stoi(argv[2]);
    std::string ip = argv[1];
    // 智能指针
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(ip, port);
    usvr->Init();
    usvr->Start();
    return 0;
}
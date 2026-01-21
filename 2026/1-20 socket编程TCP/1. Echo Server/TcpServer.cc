#include "TcpServer.hpp"
#include <memory>

int main()
{
    EnableConsoleLogStragegy();

    std::unique_ptr<TcpEchoServer> tsvr = std::make_unique<TcpEchoServer>();

    tsvr->Init();
    tsvr->Start();

    return 0;
}
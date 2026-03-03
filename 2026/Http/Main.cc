#include <iostream>
#include <string>
#include <memory>
#include "Http.hpp"
#include "TcpServer.hpp"

void Usage(const std::string &message)
{
    std::cout << "usage: " << message << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    // 显示器格式日志打印
    EnableConsoleLogStragegy();
    uint16_t httpport = std::stoi(argv[1]);
    // 协议
    std::unique_ptr<Http> http = std::make_unique<Http>();
    std::unique_ptr<TcpServer> tsock = std::make_unique<TcpServer>(httpport, [&http](std::string &request_str){
        return http->HandlerRequest(request_str); 
    });
    tsock->Run();
    /*
    tcp收到网络数据 
    -> 调用回调函数,将数据传给http对象 
    -> http对象处理完后返回响应内容 
    -> 回调函数将值返回给tcpserver,并发送给客户端 
    */
    return 0;
}
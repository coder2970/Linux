#pragma once
#include "Socket.hpp"
#include "InetAddr.hpp"
#include <memory>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

using callback_t = std::function<std::string(std::string &)>; // &:输入输出型参数
// TcpServer: 负责IO通信
class TcpServer
{
public:
    TcpServer(int port, callback_t cb) : _port(port), _listen_socket(std::make_unique<TcpSocket>()), _cb(cb)
    {
        // 创建套接字
        _listen_socket->BuildListenSocketMethod(_port);
    }
    void HandlerRequest(std::shared_ptr<Socket> sockfd, InetAddr client) // 套接字 和 client信息
    {
        // 通过sockfd读取数据
        // 短服务
        std::string inbuffer; // 读数据时的输入缓冲区, 可以看作字节流式的队列
        ssize_t n = sockfd->Recv(&inbuffer);
        if (n > 0)
        {
            std::string send_str = _cb(inbuffer);
            sockfd->Send(send_str);
        }
        else if (n == 0)
        {
            LOG(LogLevel::DEBUG) << client.ToString() << " 退出, me too";
        }
        else
        {
            LOG(LogLevel::DEBUG) << client.ToString() << " read error, quit";
        }
        sockfd->Close();
    }
    void Run()
    {
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            // 1.fd, 2.client_addr -- 统一封装
            InetAddr client_addr;
            auto sockfd = _listen_socket->Accept(&client_addr);
            if (sockfd == nullptr)
            {
                continue;
            }
            LOG(LogLevel::DEBUG) << "获取新连接: " << client_addr.ToString() << " sockfd: " << sockfd->Sockfd();

            // 创建进行执行任务
            if (fork() == 0)
            {
                _listen_socket->Close();
                HandlerRequest(sockfd, client_addr);
                exit(0);
            }
            sockfd->Close();
        }
    }

private:
    int _port;
    std::unique_ptr<Socket> _listen_socket;
    callback_t _cb;
};
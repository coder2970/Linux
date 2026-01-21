#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.hpp"
#include "Logger.hpp"
#include "InetAddr.hpp"

static const int g_default_listensockfd = -1;
static const int g_default_backlog = 8;
static const int g_default_port = 8080;

class TcpEchoServer
{
private:
    void HandlerIO(int sockfd, InetAddr &client)
    {
        char buffer[1024];
        while (true)
        {
            buffer[0] = 0;
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                std::string echo_string = "server echo: ";
                echo_string += buffer;
                LOG(LogLevel::INFO) << client.ToString() << " say: " << buffer;
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                // 网络中代表客户端关闭
                LOG(LogLevel::INFO) << "client " << client.ToString() << " quit, me too, close fd: " << sockfd;
                break;
            }
            else
            {
                // 出错
                LOG(LogLevel::WARING) << "read client " << client.ToString() << " error, sockfd: " << sockfd;
                break;
            }
        }
        close(sockfd);
    }

public:
    TcpEchoServer(uint16_t port = g_default_port) : _listensockfd(g_default_listensockfd), _port(port)
    {
    }
    void Init()
    {
        // 1.创建sockfd
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "tcp socket create error";
            exit(SOCKET_CRATE_ERR);
        }
        LOG(LogLevel::INFO) << "tcp socket creta success, sockfd: " << _listensockfd;

        // 2.绑定
        InetAddr local(_port);

        if (bind(_listensockfd, local.Addr(), local.Length()) != 0)
        {
            LOG(LogLevel::FATAL) << "tcp socket bind error";
            exit(SOCKET_BIND_ERR);
        }
        LOG(LogLevel::INFO) << "tcp socket bind success, bind sockfd: " << _listensockfd;

        // 3.listen
        // 一个tcp服务器在listen启动之后,就算是运行了
        if (listen(_listensockfd, g_default_backlog) != 0)
        {
            LOG(LogLevel::FATAL) << "tcp socket listen error";
            exit(SOCKET_LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "tcp socket listen success, listen sockfd: " << _listensockfd;
    }

    void Start()
    {
        while (true)
        {
            struct sockaddr_in peer; // 远端
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARING) << "tcp server accept error";
                continue;
            }
            InetAddr clientaddr(peer);
            // 此时获得新连接 sockfd
            LOG(LogLevel::INFO) << "获取新连接成功, sockfd: " << sockfd << " client addr: " << clientaddr.ToString();

            // 处理任务
            // 单进程版本 不持支多个客户端同时发消息, 不能同步消息
            HandlerIO(sockfd, clientaddr);
        }
    }
    ~TcpEchoServer()
    {
    }

private:
    int _listensockfd; // 监听套接字
    uint16_t _port;
};
#endif
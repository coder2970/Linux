#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.hpp"
#include "Logger.hpp"

static const int g_default_listensockfd = -1;
static const int g_default_backlog = 8;
static const int g_default_port = 8080;

class TcpEchoServer
{
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
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port); // 主机转网络序列
        // 最佳实践: 不绑定具体ip
        // 如果绑定了ip, 当前服务器就只能收到具体ip(发给这台服务器的具体ip所对应的报文)
        // 如绑定了127.0.0.1 就只能收到从127.0.0.1这个ip发来的消息
        local.sin_addr.s_addr = htonl(INADDR_ANY); // 主机转网络

        if (bind(_listensockfd, (struct sockaddr *)&local, sizeof(local)) != 0)
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

            // 此时获得新连接 sockfd
            LOG(LogLevel::INFO) << "获取新连接成功" << sockfd;
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
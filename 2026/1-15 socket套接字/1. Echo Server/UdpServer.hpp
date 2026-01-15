// UDP服务端 -- 封装
#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"

static const int g_default_sockfd = -1;
class UdpServer
{
public:
    UdpServer(const std::string &ip, uint16_t port)
        : _sockfd(g_default_sockfd), _ip(ip), _port(port), _isrunning(false)
    {
    }

    // 初始化服务
    void Init()
    {
        // 1.创建socket fd
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            // 套接字创建失败
            LOG(LogLevel::FATAL) << "create socket error";
            exit(1);
        }
        LOG(LogLevel::INFO) << "create socket success:" << _sockfd; // 应该为3

        // 2.绑定 bind
        // 2.1 填充IP和Port
        /*
        typedef uint16_t in_port_t;
        struct sockaddr_in
        {
            sa_family_t    sin_family;   // 地址族 (Address Family)
            in_port_t      sin_port;     // 端口号 (Port Number)
            struct in_addr sin_addr;     // IP 地址 (IP Address)
            unsigned char  sin_zero[8];  // 填充字节 (Padding)
        };
        typedef uint32_t in_addr_t;
        struct in_addr
        {
            in_addr_t s_addr; // 32位无符号整数 (uint32_t)
        };
        */
        struct sockaddr_in local;
        bzero(&local, sizeof(local)); // 把指定的内存块清零

        local.sin_family = AF_INET;
        local.sin_port = htons(_port);                  // 主机序列转网络序列
        local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 1.字符串转整数ip 2.主机序列转网络序列

        // 2.2 和socket进行绑定
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error";
            exit(2);
        }
        LOG(LogLevel::INFO) << "bind socket success";
    }

    // 启动服务
    void Start()
    {
        // 所有的服务器都是死循环
        _isrunning = true;
        while (_isrunning)
        {
            // 1.读取数据
                }
        _isrunning = false;
    }

    // 暂停服务
    void Stop()
    {
        _isrunning = false;
    }
    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    std::string _ip;
    bool _isrunning;
};
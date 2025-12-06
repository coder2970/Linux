#pragma once
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <cstdlib>

static const int g_defult_sockfd = -1;
class UdpServer
{
public:
    UdpServer(const std::string &ip, uint16_t port)
        : _sockfd(g_defult_sockfd),
          _port(port),
          _ip(ip),
          _isrunning(false)
    {
    }
    // 初始化服务 - 创建套接字->填充信息->绑定
    void Init()
    {
        // 我要在 IP 层（IPv4）上，建立一个基于 UDP 协议的通信端点
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            exit(1);
        }
        std::cout << "socket success" << std::endl;

        // 2.bind
        // 2.1 填充ip 和 port
        struct sockaddr_in local;
        bzero(&local, sizeof(local));                   // 把整个结构体清零
        local.sin_family = AF_INET;                     // 地址类型
        local.sin_port = htons(_port);                  // 对端主机端口号 htons:主机序列转网络序列 解决端口不一致问题
        local.sin_addr.s_addr = inet_addr(_ip.c_str()); // ip地址 1.字符串转整数ip 2.主机序列转网络序列，整数ip就是网络序列的

        // 2.2 和socketfd 进行绑定
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            exit(2);
        }
        std::cout << "socket bind success" << std::endl;
    }
    // 启动服务
    void Start()
    {
        // 所有的服务器都是死循环
        _isrunning = true;
        while (_isrunning)
        {
            // 1.读取数据 read这类系统调用是面向字节流的这里不能用
        }
        _isrunning = false; // 万一服务器自己跑出来了，设置为false
    }

    // 让服务器暂停
    void Stop()
    {
        _isrunning = false;
    }
    ~UdpServer()
    {
    }

private:
    int _sockfd; // 文件描述符，接受socket的返回值

    uint16_t _port;  // 端口号
    std::string _ip; // 暂时 "192.168.1.1" 字符串风格的点分十进制

    bool _isrunning; // 是否在运行
};
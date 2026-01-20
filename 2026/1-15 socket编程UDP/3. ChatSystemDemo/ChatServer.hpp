#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <functional>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

// 定义回调函数
using callback_t = std::function<void(int sockfd, std::string message, InetAddr addr)>;

static const int g_default_sockfd = -1;
class ChatServer
{
public:
    ChatServer(uint16_t port, callback_t cb /*告诉服务器要做的工作*/)
        : _sockfd(g_default_sockfd), _port(port), _isrunning(false), _cb(cb)
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

        InetAddr local(_port);

        // 2.2 和socket进行绑定
        int n = bind(_sockfd, local.Addr(), local.Length());
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
            char buffer[1024];
            buffer[0] = 0; // 清空缓冲区

            struct sockaddr_in peer;      // 对端主机的套接字信息
            socklen_t len = sizeof(peer); // 输入输出型参数, 初始化时要设为原来结构体大小,且每次都要重新设置
            // 1.读取数据
            ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&peer, &len);
            if (n > 0)
            {
                // buffer:聊天消息
                buffer[n] = 0;
                // client信息
                InetAddr clientaddr(peer);
                LOG(LogLevel::DEBUG) << "get a client info# " << clientaddr.Ip() << "-" << clientaddr.Port() << ": " << buffer;
                // 服务器只负责IO(读写数据), 翻译结果由_cb来完成
                std::string message = buffer;
                // 回调
                _cb(_sockfd, message, clientaddr);
            }
        }
        _isrunning = false;
    }

    // 暂停服务
    void Stop()
    {
        _isrunning = false;
    }
    ~ChatServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    bool _isrunning;
    callback_t _cb;
};
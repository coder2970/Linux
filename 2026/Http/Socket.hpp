#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

// 封装套接字

static const int g_sockfd = -1;
static const int g_backlog = 16;

enum
{
    OK,
    CREATE_ERROR,
    BIND_ERROR,
    LISTEN_ERROR
};

// 设计模式:模板方法类
// 基类 - 模板类
class Socket
{
public:
    virtual ~Socket() {}
    virtual void CreateSocket() = 0;                                   // 创建套接字
    virtual void BindSocket(int port) = 0;                             // 绑定
    virtual void ListenSocket(int backlog) = 0;                        // 监听
    virtual std::shared_ptr<Socket> Accept(InetAddr *client_addr) = 0; // 获取连接
    virtual int Sockfd() = 0;
    virtual void Close() = 0;
    virtual ssize_t Recv(std::string *out) = 0;      // 返回读到的数据
    virtual ssize_t Send(const std::string &in) = 0; // 把数据拷贝到tcp的发送缓冲区
    virtual bool Connect(InetAddr& peer) = 0;
    void BuildListenSocketMethod(int _port)
    {
        CreateSocket();
        BindSocket(_port);
        ListenSocket(g_backlog);
    }
    void BuildClientSocketMethod()
    {
        CreateSocket();

    }
};

// 子类 - 方法类
class TcpSocket : public Socket
{
public:
    TcpSocket() : _sockfd(g_sockfd)
    {
    }
    TcpSocket(int sockfd) : _sockfd(sockfd)
    {
    }
    void CreateSocket() override
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "sockfd create error";
            exit(CREATE_ERROR);
        }
        LOG(LogLevel::DEBUG) << "sockfd create success";
    }
    void BindSocket(int port) override
    {
        InetAddr local(port);
        if (bind(_sockfd, local.Addr(), local.Length()) != 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERROR);
        }
        LOG(LogLevel::DEBUG) << "bind success";
    }
    void ListenSocket(int backlog) override
    {
        if (listen(_sockfd, backlog) != 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERROR);
        }
        LOG(LogLevel::DEBUG) << "listen success";
    }
    std::shared_ptr<Socket> Accept(InetAddr *client_addr) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int fd = accept(_sockfd, (struct sockaddr *)&peer, &len);
        if (fd < 0)
        {
            LOG(LogLevel::WARING) << "listen error";
            return nullptr;
        }
        LOG(LogLevel::DEBUG) << "accept success";
        client_addr->Init(peer);
        return std::make_shared<TcpSocket>(fd);
    }
    int Sockfd() override
    {
        return _sockfd;
    }
    void Close() override
    {
        if (_sockfd >= 0)
            close(_sockfd);
    }
    ssize_t Recv(std::string *out) override
    {
        // 只读一次
        char buffer[1024];
        ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            *out += buffer; // out中的内容不断增多(故意)
        }
        return n;
    }
    ssize_t Send(const std::string &in) override
    {
        return send(_sockfd, in.c_str(), in.size(), 0);
    }
    bool Connect(InetAddr& peer) override
    {
        int n = connect(_sockfd, peer.Addr(), peer.Length());
        if(n >= 0)
            return true;
        else
            return false;
    }
    ~TcpSocket()
    {
    }

private:
    int _sockfd;
};
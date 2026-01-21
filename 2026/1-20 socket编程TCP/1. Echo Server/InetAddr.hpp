#pragma once

// 描述client socket信息
// 方便后续管理客户端
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define Conv(addr) ((struct sockaddr *)&addr)
class InetAddr
{
private:
    void HostToNet()
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    }
    void NetToHost()
    {
        _port = ntohs(_addr.sin_port);
        _ip = inet_ntoa(_addr.sin_addr);
    }

public:
    InetAddr(const struct sockaddr_in &addr) : _addr(addr)
    {
        NetToHost();
    }

    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0") : _ip(ip), _port(port)
    {
        HostToNet();
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }

    struct sockaddr *Addr()
    {
        return Conv(_addr);
    }

    socklen_t Length()
    {
        return sizeof(_addr);
    }
    std::string ToString()
    {
        return _ip + "-" + std::to_string(_port);
    }

    bool operator==(const InetAddr &addr)
    {
        return (_ip == addr._ip && _port == addr._port);
    }
    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr; // 网络风格地址
    // 主机风格地址
    std::string _ip;
    uint16_t _port;
};
// 路由功能 维护在线用户
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "InetAddr.hpp"

class Route
{
private:
    bool IsExists(const InetAddr &addr)
    {
        for (auto &user : _online_user)
        {
            if (user == addr)
            {
                return true;
            }
        }
        return false;
    }

    void AddUser(const InetAddr &addr)
    {
        if (!IsExists(addr))
            _online_user.push_back(addr);
    }

    void SendMessageToAll(int sockfd, std::string &message, InetAddr &addr)
    {
        for (auto &user : _online_user)
        {
            LOG(LogLevel::DEBUG) << "route [" << message << "] to : " << user.ToString();
            std::string info = addr.ToString();
            info += "# ";
            info += message;
            sendto(sockfd, info.c_str(), info.size(), 0, user.Addr(), user.Length());
        }
    }
    void DeleteUser(const std::string &message, const InetAddr &addr)
    {
        if (message == "QUIT")
        {
            auto iter = _online_user.begin();
            while (iter != _online_user.end())
            {
                if (*iter == addr)
                {
                    _online_user.erase(iter);
                    break;
                }
                iter++;
            }
        }
    }

public:
    Route()
    {
    }

    void RouteMessageToAll(int sockfd, std::string &message, InetAddr &addr) // 将消息转发给所有人
    {
        // 1.判断用户(inetaddr)是否在在线列表
        // 不存在,把用户添加进来
        AddUser(addr);

        // 此时一定会有在线用户列表
        // 把消息转给所有人
        SendMessageToAll(sockfd, message, addr);

        DeleteUser(message, addr); // 用户退出
    }
    void RouteMessageToOne() // 将消息转发给某一个人
    {
    }

    ~Route()
    {
    }

private:
    std::vector<InetAddr> _online_user; // 在线用户
};
#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

static const std::string sep = "\r\n";
// 应用层协议 client 向 server 发送 request, server经过计算, 得到response并返回
// 报文内容: 有效载荷长度(int) + \r\n + jsonstr + \r\n
class Request
{
public:
    Request() :_x(0), _y(0), _oper(0)
    {
    }
    // 序列化对象
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;
        Json::StyledWriter writer;
        *out = writer.write(root);
        if (out->empty())
            return false;
        return true;
    }
    // 反序列化对象
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ret = reader.parse(in, root);
        if (!ret)
            return false;
        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _oper = root["oper"].asInt();
        return true;
    }
    int X()
    {
        return _x;
    }
    int Y()
    {
        return _y;
    }
    char Oper()
    {
        return _oper;
    }
    ~Request()
    {
    }

// private:
    // 运算规则 _x _oper _y
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response() : _result(0), _code(0)
    {
    }
    // 序列化对象
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        Json::StyledWriter writer;
        *out = writer.write(root);
        if (out->empty())
            return false;
        return true;
    }
    // 反序列化对象
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ret = reader.parse(in, root);
        if (!ret)
            return false;
        _result = root["result"].asInt();
        _code = root["code"].asInt();
        return true;
    }
    void SetResult(int result)
    {
        _result = result;
    }
    void SetCode(int code)
    {
        _code = code;
    }
    void Print()
    {
        std::cout << "计算结果: " << _result << "可信度: " << _code << std::endl;
    }
    ~Response()
    {
    }

private:
    int _result; // 计算结果
    int _code;   // 可信度:0 -> 可信 , 1 2 3... -> 对应不同的错误
};

class Protocol
{
public:
    // jsonstr -> len\r\njsonstr\r\n
    static std::string Package(const std::string &jsonstr) // 以类的方式直接访问
    {
        if (jsonstr.empty())
            return std::string();
        std::string jsonstr_lenth = std::to_string(jsonstr.size());
        return jsonstr_lenth + sep + jsonstr + sep;
    }

    // 解析报文 - str(输入输出): 从网络中读入的字符串, package(输出): 一条完整的json报文
    static int Unpack(std::string &str, std::string *package)
    {
        if (!package)
            return 0;
        auto pos = str.find(sep); // 从左向右查找第一个sep
        if (pos == std::string::npos)
            return 0;
        // 1.此时, str是包含 len\r\n 的, 即至少收到了一个报文的长度信息
        std::string str_len = str.substr(0, pos);
       
        if(!DigitSafeCheck(str_len)) // ???????????????????????????????
            return -1;
        int digit_len = std::stoi(str_len);

        // 报文总长度
        int target_len = str_len.size() + digit_len + 2 * sep.size();
        if(str.size() < target_len) // 报文仍不完整
            return 0;
        // 2.此时,得到了完整的报文
        *package = str.substr(pos + sep.size(), digit_len);
        str.erase(0, target_len);

        return package->size();
    }
    static bool DigitSafeCheck(const std::string& str)
    {
        for(const auto& e : str)
        {
            if(!(e >= '0' && e <= '9'))
                return false; 
        }
        return true;
    }
};

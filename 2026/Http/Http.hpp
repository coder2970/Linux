#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Logger.hpp"

static const std::string line_sep = "\r\n";
static const std::string inner_sep1 = " ";
static const std::string inner_sep2 = ": ";
static const std::string webroot = "./wwwroot";
static const std::string defaulthome = "index.html";

// 定制Http协议 -- 不考虑完整性问题

// 读端
class HttpRequest
{
private:
    std::string ReadOneLine(std::string &request_str, bool *status)
    {
        *status = false;
        auto pos = request_str.find(line_sep);
        if (pos == std::string::npos)
            return std::string();
        auto line = request_str.substr(0, pos);
        request_str.erase(0, pos + line_sep.size());
        *status = true;
        return line; // line是临时对象,所以传值返回
    }
    void BuildKV(std::string &line, std::string *key, std::string *value)
    {
        auto pos = line.find(inner_sep2);
        if (pos == std::string::npos)
        {
            *key = *value = std::string();
            return;
        }
        *key = line.substr(0, pos);
        *value = line.substr(pos + inner_sep2.size());
    }

    void ParseReqLine(std::string &request_line)
    {
        std::stringstream ss(request_line);
        ss >> _method >> _uri >> _httpversion;
    }

public:
    HttpRequest() {}
    // 序列和反序列化
    void Serialize()
    {
        // 省略 -- 服务端通常不需要把数据再发给别人,只需要看收到数据的内容
    }
    bool Deserialize(std::string &request_str)
    {
        // 解析请求行
        bool status = true;
        std::string request_line = ReadOneLine(request_str, &status);
        if (!status)
            return false;
        LOG(LogLevel::DEBUG) << request_line;
        ParseReqLine(request_line);

        // 解析报头
        while (true)
        {
            status = true;
            request_line = ReadOneLine(request_str, &status);
            if (status && !request_line.empty())
            {
                std::string key, value;
                BuildKV(request_line, &key, &value);
                if (key.empty() || value.empty())
                    continue;
                _request_headers.insert({key, value});
            }
            else if (status)
            {
                // 空行
                _blank_line = line_sep;
                break;
            }
            else
            {
                return false;
            }
        }

        // 解析正文
        _request_body = request_str;

        _path = webroot;
        _path += _uri;

        if (_uri == "/")
        {
            _path += defaulthome;
        }

        LOG(LogLevel::DEBUG) << "path: " << _path;
        return true;
    }

    std::string Path()
    {
        return _path;
    }
    ~HttpRequest() {}

private:
    // 请求行
    std::string _method;
    std::string _uri;
    std::string _httpversion;
    // 报头
    std::unordered_map<std::string, std::string> _request_headers;
    // 空行
    std::string _blank_line;
    // 请求正文
    std::string _request_body;

    std::string _path; // 访问资源的路径
};

// 写端
class HttpResponse
{
public:
    HttpResponse() : _httpversion("HTTP/1.1"), _blank_line("\r\n") {}

    std::string Serialize()
    {
        std::string response_str = _httpversion + inner_sep1 + std::to_string(_code) + inner_sep1 + _desc + line_sep;
        // 拼接报头
        for (auto &e : _response_headers)
        {
            std::string line = e.first + inner_sep2 + e.second + line_sep;
            response_str += line;
        }
        response_str += _blank_line;
        response_str += _response_body;
        return response_str;
    }
    void ReadContent(const std::string &path)
    {
        // 以二进制方式读取
        // 1. 以二进制模式打开，并直接定位到文件末尾 (ate)
        std::ifstream in(path, std::ios::binary | std::ios::ate);

        // 2. 如果文件打开失败，直接返回空字符串
        if (!in.is_open())
        {
            throw std::runtime_error("文件打开失败" + path);
        }

        // 3. 获取文件大小 (tellg)
        std::streamsize size = in.tellg();

        // 4. 处理空文件或异常大小的情况
        if (size <= 0)
        {
            throw std::runtime_error("文件为空或异常" + path);
        }

        // 5. 预分配 string 空间，避免内存重新分配
        _response_body.resize(size);

        // 6. 将文件指针移回开头
        in.seekg(0, std::ios::beg);

        // 7. 直接读取全部内容到 string 的内存中
        // &content[0] 是 C++11 标准保证的安全写法
        in.read(&_response_body[0], size);
        in.close();
    }
    void SetCode(int code, const std::string &desc)
    {
        _code = code;
        _desc = desc;
    }
    void Deserialize()
    {
    }
    ~HttpResponse() {}

private:
    // 状态行
    std::string _httpversion;
    int _code;
    std::string _desc;
    // 响应报头
    std::unordered_map<std::string, std::string> _response_headers;
    // 空行
    std::string _blank_line;
    // 响应正文
    std::string _response_body;
};

class Http
{
public:
    Http() {}
    ~Http() {}
    std::string HandlerRequest(std::string &request_str)
    {
        std::string response_str;
        HttpRequest req;
        if (req.Deserialize(request_str))
        {
            std::string target = req.Path();
            HttpResponse response;
            response.ReadContent(target);
            response.SetCode(200, "OK");
            response_str = response.Serialize();
        }

        return response_str;
    }
};
// 字典类 执行翻译任务
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Logger.hpp"
#include "mutex.hpp"

static const std::string sep = ": "; // 定义分割符

class Dictionary
{
private:
    // 加载配置文件 -- 字典
    void LoadConf()
    {
        // 打开配置文件
        std::ifstream in(_path);
        if (!in.is_open())
        {
            LOG(LogLevel::DEBUG) << "open file error";
            return;
        }
        std::string line;
        while (std::getline(in, line))
        {
            LOG(LogLevel::DEBUG) << "dict message: " << line;
            // apple:苹果
            // 切割
            auto pos = line.find(sep);
            // 如果找不到 跳过该行
            if (pos == std::string::npos)
            {
                LOG(LogLevel::WARING) << "format error: " << line;
                continue;
            }
            std::string world = line.substr(0, pos);           // 从0开始,截取pos长度 [0, pos)
            std::string value = line.substr(pos + sep.size()); // 从分隔符后第一个字符开始,截取到结尾
            if (world.empty() || value.empty())
            {
                LOG(LogLevel::WARING) << "format error: world or value is empty! " << line;
                continue;
            }
            _dict.insert({world, value});
        }

        // 关闭文件
        in.close();
    }

public:
    Dictionary(const std::string &path) : _path(path)
    {
        LoadConf();
    }

    std::string Translate(const std::string &word, const std::string &who_ip, uint16_t who_port)
    {
        auto iter = _dict.find(word);
        if (iter == _dict.end())
        {
            return "unkown word!";
        }
        return iter->first + "->" + iter->second;
    }
    ~Dictionary()
    {
    }

private:
    std::string _path; // 字典位置 配置文件
    std::unordered_map<std::string, std::string> _dict;
};
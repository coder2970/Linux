// 字典类 执行翻译任务
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

class Dictionary
{
public:
    Dictionary(const std::string &path) : _path(path)
    {
    }
    std::string Translate(const std::string &word, const std::string &who_ip, uint16_t who_port)
    {
    }
    ~Dictionary()
    {
    }

private:
    std::string _path; // 字典位置
    std::unordered_map<std::string, std::string> _dict;
};
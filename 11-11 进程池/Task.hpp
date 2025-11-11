#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

// 4种任务
// tasl[4];
using task_t = std::function<void()>;

void Download()
{
    std::cout << "i m Download task!" << std::endl;
}

void Mysql()
{
    std::cout << "i m Mysql task!" << std::endl;
}

void Sync()
{
    std::cout << "i m Sync task!" << std::endl;
}

void Log()
{
    std::cout << "i m Log task!" << std::endl;
}

std::vector<task_t> tasks;

class Init
{
public:
    Init()
    {
        tasks.push_back(Download);
        tasks.push_back(Mysql);
        tasks.push_back(Sync);
        tasks.push_back(Log);
    }
};

Init ginit;
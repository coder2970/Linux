#pragma once
#include <iostream>
#include <functional>
#include <unistd.h>
#include <sstream>

class Task
{
public:
    Task()
    {
    }
    Task(int x, int y) : a(x), b(y)
    {
    }
    void Excute()
    {
        res = a + b;
    }

    void operator()()
    {
        Excute();
    }
    void Print()
    {
        std::cout << a << "+" << b << "=" << res << std::endl;
    }

    std::string ResToStr()
    {
        std::stringstream ss;
        ss << a << "+" << b << "=" << res;
        return ss.str();
    }

private:
    int res;
    int a, b;
};
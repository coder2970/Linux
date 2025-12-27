#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"
const int gdefult_process_nums = 5; // 创建五个子进程

using callback_t = std::function<void(int fd)>;

class Channel
{
public:
    Channel() {}
    Channel(int fd, std::string &name, pid_t id)
        : _wfd(fd), _name(name), _sub_target(id)
    {
    }
    ~Channel() {}

    int Fd() { return _wfd; }
    std::string Name() { return _name; }
    pid_t Target() { return _sub_target; }

    void Close() { close(_wfd); }
    void Wait()
    {
        pid_t rid = waitpid(_sub_target, nullptr, 0);
        (void)rid;
    }

private:
    int _wfd;          // 写端文件描述符值
    std::string _name; // 每个通道起名字
    pid_t _sub_target; // 目标子进程
};

class ProcessPool
{
private:
    void CtrlSubProcessHelper(int &index)
    {
        // 1.选择一个通道
        int who = index;
        index++;
        index %= _channels.size();

        // 2.选择一个任务，随机选
        int x = rand() % tasks.size();

        // 3.任务推送给子进程
        std::cout << "选择信道 " << _channels[who].Name() << ",subtarget : " << _channels[who].Target() << std::endl;
        write(_channels[who].Fd(), &x, sizeof(x));
        sleep(1);
    }

public:
    ProcessPool(int num = gdefult_process_nums)
        : _processnum(num)
    {
        srand(time(nullptr) ^ getpid() ^ 0x777);
    }
    ~ProcessPool()
    {
    }
    bool InitProcessPool(callback_t cb)
    {
        for (int i = 0; i < _processnum; i++)
        {
            // 1.创建管道
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
                return false;

            // 2.创建子进程
            pid_t id = fork();
            if (id < 0)
                return false;
            if (id == 0)
            {
                // 子进程要关闭自己的写端，还要关闭从父进程继承下来的所有写端
                // 那么这些继承的写端wfd都在哪里？
                // _channels是会被子进程继承下去的，
                // 1.子进程不需要考虑父进程之后的操作会影响自己
                // 2.fork之后，当前进程会看到历史父进程所创建的所有的wfd，至于后续父进程emplace_back，和当前子进程无关

                // 因此，只要每次fork之后都把子进程继承父进程的wfd关闭，每个子进程在回收时，对应的文件描述符的引用计数就不会不为1
                std::cout << "进程： " << getpid() << "关闭";
                for (auto &c : _channels)
                {
                    c.Close(); // 关闭子进程继承下来的父进程的文件描述符
                    std::cout << c.Fd() << " ";
                }
                std::cout << std::endl;

                // 子 读端
                // 3.关闭不需要的rw端，形成信道
                close(pipefd[1]);
                // 子进程执行任务
                cb(pipefd[0]);

                exit(0);
            }
            // 父 写端
            close(pipefd[0]);
            std::string name = "channel -" + std::to_string(i);
            _channels.emplace_back(pipefd[1], name, id);
        }

        return true;
    }

    void PollingCtrlProcess()
    {
        int index = 0;
        while (true)
        {
            CtrlSubProcessHelper(index);
        }
    }
    void PollingCtrlProcess(int count)
    {
        if (count < 0)
            return;
        int index = 0;
        while (count)
        {
            CtrlSubProcessHelper(index);
            count--;
        }
    }

    void WaitSubProcess()
    {
        // for (int end = _channels.size() - 1; end >= 0; end--)
        // {
        //     _channels[end].Close();
        //     _channels[end].Wait();
        // }
        for (auto &c : _channels)
        {
            c.Close();
            c.Wait();
        }
    }

private:
    std::vector<Channel> _channels;
    int _processnum;
};
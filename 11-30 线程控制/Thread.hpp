#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <functional>
#include <sys/syscall.h>

#define get_lwp_id() syscall(SYS_gettid)

using func_t = std::function<void()>;
const std::string threadname = "defult_thread";

class Thread
{
public:
    static void *thread_routine(void *args) // static函数也无法访问类内成员变量,所以传this进来
    {
        Thread *self = static_cast<Thread *>(args);
        self->_isrunning = true;
        self->_lwpid = get_lwp_id();

        self->_func();

        pthread_exit((void *)0);
    }

    // 线程启动时要做的任务,传递进去
    Thread(func_t func, const std::string name = threadname)
        : _name(name), _func(func), _isrunning(false)
    {
    }

    void Start()
    {
        int n = pthread_create(&_tid, nullptr, thread_routine, this);
        if (n == 0)
        {
            std::cout << "create success" << std::endl;
        }
    }

    // 检测线程结束并回收
    void Join()
    {
        // 没启动直接返回
        if (!_isrunning)
            return;

        int n = pthread_join(_tid, nullptr);
        if (n == 0)
        {
            std::cout << "join success" << std::endl;
        }
    }
    ~Thread()
    {
    }

private:
    bool _isrunning;   // 线程是否启动
    pthread_t _tid;    // 线程id 库级别
    pid_t _lwpid;      // 系统级别
    std::string _name; // 用户级别
    func_t _func;
};

#endif
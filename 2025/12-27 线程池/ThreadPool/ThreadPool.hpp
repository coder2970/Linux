#pragma once
#include "Thread.hpp"
#include "Cond.hpp"
#include "mutex.hpp"
#include <unistd.h>
#include <iostream>
#include <queue>
#include <vector>

const static int default_threadnum = 5;

template <class T>
class ThreadPool
{
private:
    bool QueueIsEmpty()
    {
        return _q.empty();
    }
    void routine(const std::string &name)
    {
        while (true)
        {
            /*测试:
            sleep(1);
            LOG(LogLevel::INFO) << name << " hello";
            */
            // 把任务从线程获取到线程私有 临界区 -> 私有栈结构
            T t;
            {
                LockGuard lockguard(&_lock);
                while (QueueIsEmpty() && _is_running) // 休眠条件:1. 队列为空取不到任务 2.运行状态为running
                {
                    _wait_thread_num++;
                    _cond.Wait(_lock);
                    _wait_thread_num--;
                }
                if (!_is_running && QueueIsEmpty()) // 运行状态为false 且 队列为空,满足推出条件
                {
                    LOG(LogLevel::INFO) << "线程池退出 && 任务队列为空, " << name << " 退出";
                    break;
                }

                // 运行至此可以确保队列中有任务
                t = _q.front();
                _q.pop();
            }
            // 任务处理
            t(); // 规定未来的任务必须这样处理 处理任务需要在临界区中处理吗
            LOG(LogLevel::DEBUG) << name << " handler task : " << t.ResToStr();
        }
    }

public:
    ThreadPool(int threadnum = default_threadnum)
        : _is_running(false), _threadnum(threadnum), _wait_thread_num(0)
    {
        for (int i = 0; i < _threadnum; i++)
        {
            std::string name = "thread-" + std::to_string(i + 1);
            _threads.emplace_back([this](const std::string &name)
                                  { this->routine(name); }, name);
            // Thread t([this]()
            //          { this->hello(); }, name);
            // _threads.push_back(std::move(t));
        } // 此处只是创建五个单线程单进程,还没有start,所以不存在同步互斥等问题
        LOG(LogLevel::INFO) << " thread pool create success";
    }

    // 启动线程池
    void Start()
    {
        // 防止重复启动
        if (_is_running)
            return;
        _is_running = true;
        for (auto &t : _threads)
        {
            t.Start();
        }
        LOG(LogLevel::INFO) << " thread pool running success";
    }

    // 核心逻辑:线程走正常的唤醒逻辑退出
    // 线程池要退出
    // 1.线程被唤醒 && 任务队列没有任务   -> 让线程退出
    // 2.线程被唤醒 && 任务队列有任务 ->线程任务执行完队列任务后在退出
    // 3.线程没有被休眠 && 正在处理任务 -> 让线程把任务执行完后退出
    // 不管怎么变都是把2或3转化为1
    // 也就是说,如果线程池中有任务,线程是不会休眠的
    void Stop()
    {
        if (!_is_running)
            return;
        _is_running = false;

        if (_wait_thread_num) // 如果有线程还在等待,就把所有的线程都唤醒
            _cond.NotifyAll();

        // for (auto &t : _threads)
        // {
        //     t.Stop();
        // }
        // LOG(LogLevel::INFO) << " thread pool stop success";
    }

    void Wait()
    {
        for (auto &t : _threads)
        {
            t.Join();
        }
        LOG(LogLevel::INFO) << " thread pool wait success";
    }
    // 向线程池入队列
    void Enqueue(const T &t)
    {
        if (!_is_running)
            return;
        {
            LockGuard lockguard(&_lock);
            // 添加任务
            _q.push(t);

            // 有任务在等待, 直接唤醒让其执行任务
            if (_wait_thread_num > 0)
                _cond.NotifyOne();
        }
    }

    ~ThreadPool()
    {
    }

private:
    // 需要任务队列
    std::queue<T> _q; // 多线程都要访问这个队列 临界资源 -> 当成一种整体使用的临界资源
    // 多个线程
    std::vector<Thread> _threads; // 1.创建线程对象 2.让线程对象启动
    int _threadnum;               // 线程的数量
    int _wait_thread_num;         // 线程池中有多少线程正在等待
    // 保护机制
    Mutex _lock;
    Cond _cond; // 只需要一个条件变量即可

    // 其他属性
    bool _is_running; // 线程池是否在运动
};

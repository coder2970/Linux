#pragma once

#include <iostream>
#include <vector>
#include "Sem.hpp"
#include "mutex.hpp"

static int g_defult_capacity = 5;
// 环形队列
template <class T>
class RingQueue
{
public:
    RingQueue(int capacity = g_defult_capacity)
        : _capacity(capacity),
          _ring_queue(capacity),
          _space_sem(capacity),
          _data_sem(0),
          _p_step(0),
          _c_step(0)
    {
    }

    void Pop(T *out)
    {
        _data_sem.P();
        // _c_lock.Lock();
        {
            LockGuard lockguard(&_c_lock);
            // 走到此处一定不为空
            *out = _ring_queue[_c_step++];
            _c_step %= _capacity;
        }
        _space_sem.V();
        // _c_lock.Unlock();
    }

    void Enqueue(const T &in)
    {

        // 生产数据
        _space_sem.P();
        // _p_lock.Lock(); // 注意申请锁和申请信号量的位置 先申请信号量 在申请锁
        {
            LockGuard lockguard(&_p_lock);
            _ring_queue[_p_step++] = in;
            // 维持环形特点
            _p_step %= _capacity;
        }
        _data_sem.V();
        // _p_lock.Unlock();
    }

    ~RingQueue()
    {
    }

private:
    std::vector<T> _ring_queue; // 临界资源
    int _capacity;

    // 对环形队列进行保护
    Sem _space_sem;
    Sem _data_sem;

    int _p_step; // 在哪生产
    int _c_step; // 在哪消费

    // 多多模型中定义两把锁
    Mutex _p_lock;
    Mutex _c_lock;
};
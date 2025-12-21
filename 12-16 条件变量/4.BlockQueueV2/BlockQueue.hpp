#pragma once

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include "../3.Cond/Cond.hpp"
#include "../3.Cond/mutex.hpp"

const static u_int32_t gcapacity = 5;
template <class T>
class BlockQueue
{
private:
    bool IsFull()
    {
        return _bq.size() >= _capacity;
    }

    bool IsEmpty()
    {
        return _bq.empty();
    }

public:
    BlockQueue(u_int32_t capacity = gcapacity) : _capacity(capacity) /*定义阻塞队列的大小*/
    {
        // // 对锁和条件变量进行初始化
        // pthread_mutex_init(&_lock, nullptr);
        // pthread_cond_init(&_c_cond, nullptr);
        // pthread_cond_init(&_p_cond, nullptr);
    }
    // 单生产单消费
    void Enqueue(const T &in)
    {
        LockGuard lockguard(&_lock);
        {
            // 加锁,保证生产过程不被打扰,防止消费者进入
            // pthread_mutex_lock(&_lock);
            // 生产:
            // 要进行生产,就一定能生产吗? -- 必须满足生产条件
            // 阻塞队列不为满,才可以生产
            while (IsFull()) // 使用while而不是if 增加健壮性
            {
                // 满了 就不要生产,而是去等待
                // 1.等待时,线程时在临界区里等的,是持有锁的
                // 所以必须把锁释放,于是wait中存在一个参数,让我们把锁传进去
                // 2.为什么把判断逻辑放在临界区内?
                // 因为生产的第一步就是判断队列是否为满(生产条件是否满足)
                // 而判断队列是否为满,本身就是在访问临界资源
                // pthread_cond_wait(&_p_cond, &_lock);
                /*特征1:自动释放锁 特征2:被唤醒时自动重新竞争锁资源
                example: wait中前50行代码释放锁,卡在第50行等待,唤醒后,后50行代码完成重新竞争并持有锁的逻辑*/
                _p_cond.Wait(_lock);
                // 当线程再次被唤醒时,也应该从这里唤醒 -> 在临界区内被唤醒了

                // 3.等待时,谁来唤醒生产者
            }
            // 队列不满,可以生产
            _bq.push(in);

            // 3.生产完了,唤醒消费者,让其来消费
            _c_cond.NotifyOne();
            // pthread_cond_signal(&_c_cond);
            // pthread_mutex_unlock(&_lock);
        }
    }

    void Pop(T *out)
    {
        // pthread_mutex_lock(&_lock);
        LockGuard lockguard(&_lock);
        {
            while (IsEmpty())
            {
                // pthread_cond_wait(&_c_cond, &_lock);
                _c_cond.Wait(_lock);
                // 等待时,谁来唤醒消费者
            }
            *out = _bq.front();
            _bq.pop();

            _p_cond.NotifyOne();
        }
        // pthread_cond_signal(&_p_cond);
        // pthread_mutex_unlock(&_lock);
    }

    ~BlockQueue()
    {
        // // 释放锁资源和条件变量资源
        // pthread_mutex_destroy(&_lock);
        // pthread_cond_destroy(&_c_cond);
        // pthread_cond_destroy(&_p_cond);
    }

private:
    // 临界资源
    std::queue<T> _bq;
    u_int32_t _capacity;

    // pthread_mutex_t _lock;
    // // 需要两个条件变量 可以定向的唤醒生产者或消费者
    // // 只有一个条件变量,让生产者和消费者同时去等待 也可以,但是无法做到定向
    // // 两个条件变量,让生产者和消费者在各自独立的条件变量下等待
    // pthread_cond_t _c_cond; // 消费者用的条件变量
    // pthread_cond_t _p_cond; // 生产者用的条件变量

    Mutex _lock;
    Cond _c_cond;
    Cond _p_cond;
};
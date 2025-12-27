#pragma once

#include <iostream>
#include <mutex>
#include <pthread.h>

// 对互斥锁做封装
class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_lock, nullptr);
    }
    // 加锁
    void Lock()
    {
        pthread_mutex_lock(&_lock);
    }

    // 解锁
    void Unlock()
    {
        pthread_mutex_unlock(&_lock);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&_lock);
    }

private:
    pthread_mutex_t _lock; // 锁
};

class LockGuard
{
public:
    LockGuard(Mutex *mutex) : _mutex(mutex)
    {
        _mutex->Lock();
    }
    ~LockGuard()
    {
        _mutex->Unlock();
    }

private:
    Mutex *_mutex;
};
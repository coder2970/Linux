#pragma once

#include <iostream>
#include <pthread.h>
#include "mutex.hpp"

class Cond
{
public:
    Cond()
    {
        pthread_cond_init(&_cond, nullptr);
    }

    void Wait(Mutex &lock)
    {
        int n = pthread_cond_wait(&_cond, lock.GetMutex());
    }

    void NotifyOne() // 通知一个线程
    {
        int n = pthread_cond_signal(&_cond);
    }

    void NotifyAll() // 通知所有线程
    {
        int n = pthread_cond_broadcast(&_cond);
    }

    ~Cond()
    {
        pthread_cond_destroy(&_cond);
    }

private:
    pthread_cond_t _cond;
};
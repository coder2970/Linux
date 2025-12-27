
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "Mutex/mutex.hpp"
int tickets = 1000;

Mutex lock;
void *routine(void *args)
{
    char *id = (char *)args;
    while (true)
    {
        // 每个资源在访问临界区时必须加锁 从这里开始代码必须是串行的
        // lock.Lock();
        {
            LockGuard lockguard(&lock); // RAII风格加锁
            if (tickets > 0)
            {
                usleep(1000);
                printf("%s 抢到票号: %d\n", id, tickets);
                tickets--;
                // 临界区结束 解锁
                // lock.Unlock();
            }
            else
            {
                // lock.Unlock();
                break;
            }
        }
    }
    return (void *)0;
}
int main()
{

    // 创建四个线程,作为抢票人机
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, nullptr, routine, (void *)"t1");
    pthread_create(&t2, nullptr, routine, (void *)"t2");
    pthread_create(&t3, nullptr, routine, (void *)"t3");
    pthread_create(&t4, nullptr, routine, (void *)"t4");

    // 创建完进行等待
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);

    return 0;
}
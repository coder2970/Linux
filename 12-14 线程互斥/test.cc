#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "Mutex/mutex.hpp"
int tickets = 1000;

// -----------------------------------------------------------------------
// 静态初始化
pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

struct data
{
    std::string name;
    pthread_mutex_t *lockp;
};

void *routine(void *args)
{
    // std::string name = static_cast<const char *>(args);
    data *d = static_cast<data *>(args);
    while (true)
    {
        // 每个资源在访问临界区时必须加锁 从这里开始代码必须是串行的
        pthread_mutex_lock(d->lockp);
        if (tickets > 0)
        {
            usleep(1000);
            printf("%s 抢到票号: %d\n", d->name.c_str(), tickets);
            tickets--;
            // 临界区结束 解锁
            pthread_mutex_unlock(d->lockp);
        }
        else
        {
            pthread_mutex_unlock(d->lockp);
            break;
        }
    }
    return (void *)0;
}
int main()
{
    // 不定义全局
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);

    data d1 = {"thread-1", &lock};
    data d2 = {"thread-2", &lock};
    data d3 = {"thread-3", &lock};
    data d4 = {"thread-4", &lock};

    // 创建四个线程,作为抢票人机
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, nullptr, routine, (void *)&d1);
    pthread_create(&t2, nullptr, routine, (void *)&d2);
    pthread_create(&t3, nullptr, routine, (void *)&d3);
    pthread_create(&t4, nullptr, routine, (void *)&d4);

    // 创建完进行等待
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);

    // 销毁锁
    pthread_mutex_destroy(&lock);
    return 0;
}
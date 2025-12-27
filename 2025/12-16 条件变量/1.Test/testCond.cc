#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>

pthread_cond_t gcond = PTHREAD_COND_INITIALIZER; // 静态初始化
// 还需要一把锁
pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;
// 让这四个线程在条件变量下等
void *routine(void *args)
{
    while (true)
    {
        std::string name = static_cast<const char *>(args);
        // 1.先加锁 不管是拿苹果还是放苹果,都要先申请锁
        // 只有持有锁的线程 未来才允许它打印
        pthread_mutex_lock(&gmutex);
        // 2.直接在条件变量下等待 被唤醒时才执行打印动作
        pthread_cond_wait(&gcond, &gmutex);
        // 多线程向显示器打印时,显示器本身就是临界资源
        // 用加锁和解锁保护显示器 但是不让线程直接打印,等待通知,让你打你再打
        std::cout << name << " active" << std::endl;
        // 解锁
        pthread_mutex_unlock(&gmutex);
    }

    return (void *)0;
}

int main()
{
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, nullptr, routine, (void *)"thread-1");
    pthread_create(&t2, nullptr, routine, (void *)"thread-2");
    pthread_create(&t3, nullptr, routine, (void *)"thread-3");
    pthread_create(&t4, nullptr, routine, (void *)"thread-4");

    sleep(5);
    while (true)
    {
        // pthread_cond_signal(&gcond);
        pthread_cond_broadcast(&gcond);
        sleep(1);
    }

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);
    return 0;
}
#include "Thread.hpp"
#include <vector>
#include <iostream>

void test()
{
    int cnt = 5;
    while (cnt--)
    {
        std::cout << "cnt:" << cnt << std::endl;
        sleep(1);
    }
}
int main()
{
    // 单线程
    // Thread t(test, "threadTest");
    // t.Start();
    // t.Join();
    // 多线程
    std::vector<Thread> threads;
    for (int i = 0; i < 5; i++)
    {
        Thread t(test, "thread-1");
        threads.push_back(t);
    }

    for (auto &thread : threads)
    {
        thread.Start();
    }

    for (auto &thread : threads)
    {
        thread.Join();
    }
}

// --------------------------------------------------------------------------------
// #include <iostream>
// #include <string>
// #include <unistd.h>
// #include <pthread.h>
// #include <vector>

// #include <iostream>
// #include <string>
// #include <unistd.h>
// #include <pthread.h>
// #include <vector>

// void *thread_routine(void *args)
// {
//     std::string threadname = static_cast<const char *>(args);

//     printf("new thread create: name: %s", threadname.c_str());
//     sleep(1);

//     // 释放线程参数申请的空间
//     // free(args);
//     // return (void *)10;
//     pthread_exit((void *)20);
// }

// int main()
// {

//     pthread_t tid;
//     pthread_create(&tid, nullptr, thread_routine, (void *)"thread-1");
//     sleep(2);
//     void *ret;
//     int m = pthread_join(tid, &ret); // 拿二级指针
//     if (m == 0)
//         printf("join success, return val: %lld\n", (long long)ret);

//     printf("main end...\n");
//     return 0;
// }

// // int main()
// // {
// //     std::vector<pthread_t> tids;
// //     for (int i = 0; i < 10; i++)
// //     {
// //         pthread_t tid;
// //         // 动态分配参数
// //         char *idbuffer = (char *)malloc(64);
// //         snprintf(idbuffer, 64, "thread - %d\n", i + 1);
// //         pthread_create(&tid, nullptr, thread_routine, idbuffer);
// //         tids.push_back(tid); // 线程id类型要与pthread_t一致
// //     }
// //     for (auto &tid : tids)
// //         printf("thread create, tid: %lu\n", (unsigned long)tid);
// //     for (auto &tid : tids)
// //     {
// //         pthread_join(tid, nullptr);
// //         printf("new thread over, tid: %lu\n", (unsigned long)tid);
// //     }
// //     printf("main end...\n");
// //     return 0;
// // }

// // // volatile int gval = 100;
// // void *thread_routine(void *args)
// // {
// //     // 新线程
// //     std::string threadname = static_cast<const char *>(args);
// //     // std::string threadname = (const char *)args;
// //     int cnt = 5;
// //     while (cnt--)
// //     // while (true)
// //     {
// //         // printf("new thread...,gval:%d &gval: %p\n", gval++, &gval);
// //         printf("new thread create: name: %s", threadname.c_str());
// //         sleep(1);
// //     }
// //     return nullptr; // 如果线程执行完了自己的入口函数,那么该线程退出
// // }

// // int main()
// // {
// //     std::vector<int> tids;
// //     for (int i = 0; i < 10; i++)
// //     {
// //         pthread_t tid; // 线程tid
// //         char idbuffer[1024];
// //         snprintf(idbuffer, sizeof(idbuffer), "thread - %d\n", i + 1);
// //         int n = pthread_create(&tid, nullptr, thread_routine, idbuffer);
// //         tids.push_back(tid);
// //     }
// //     // printf("%lu\n", tid);
// //     // 主线程
// //     // while (true)
// //     // {
// //     //     printf("main thread...gval:%d &gval: %p\n", gval, &gval);
// //     //     sleep(1);
// //     // }
// //     for (auto &tid : tids)
// //         printf("thread create,tid: %d\n", tid);
// //     for (auto &tid : tids)
// //     {
// //         pthread_join(tid, nullptr);
// //         printf("new thread over,tid: %d", tid);
// //     }
// //     printf("main end...\n");
// //     return 0;
// // }
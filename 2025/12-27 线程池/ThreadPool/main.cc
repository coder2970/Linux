#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>
#include <time.h>

int main()
{
    srand(time(nullptr) ^ getpid());
    EnableConsoleLogStragegy();
    // 使用线程池
    std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>(10);

    tp->Start();
    int cnt = 10;
    // sleep(5);
    while (cnt--)
    {
        int x = rand() % 10 + 1;
        usleep(rand() % 73);
        int y = rand() % 5 + 1;
        Task t(x, y);

        // 任务有了, push到线程池中
        tp->Enqueue(t);
        sleep(1);
    }
    tp->Stop();
    tp->Wait();
    // tp->Enqueue();

    return 0;
}
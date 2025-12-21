#include "RingQueue.hpp"
#include <pthread.h>
#include <unistd.h>

void *consume(void *args)
{
    RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    int data = 0;
    while (true)
    {
        // sleep(1);
        rq->Pop(&data);
        std::cout << "消费者消费数据" << data << std::endl;
    }
}

void *product(void *args)
{
    RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    int data = 1;
    while (true)
    {
        sleep(1);
        rq->Enqueue(data);
        std::cout << "生产者生产数据: " << data++ << std::endl;
    }
}
int main()
{
    // 创建环形队列
    RingQueue<int> *rq = new RingQueue<int>();
    // 创建线程
    pthread_t c[2], p[3];

    pthread_create(c + 0, nullptr, consume, (void *)rq);
    pthread_create(c + 1, nullptr, consume, (void *)rq);

    pthread_create(p + 0, nullptr, product, (void *)rq);
    pthread_create(p + 1, nullptr, product, (void *)rq);
    pthread_create(p + 2, nullptr, product, (void *)rq);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);

    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);
    delete rq;
    return 0;
}
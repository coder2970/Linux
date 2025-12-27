#include "BlockQueue.hpp"

// 定义公共资源,让生产者消费者均能看见
struct ThreadData
{
    BlockQueue<int> *bq;
    std::string name;
};

void *consume(void *args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    while (true)
    {
        // 消费
        // sleep(1);
        int data = 0;
        td->bq->Pop(&data); // 取数据
        std::cout << "消费者消费数据data: " << data << std::endl;
    }
}
void *product(void *args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    int data = 1;
    while (true)
    {
        // 生产
        sleep(1);
        td->bq->Enqueue(data); // 不断入队列
        std::cout << "生产者生产数据data: " << data++ << std::endl;
    }
}

int main()
{
    pthread_t c, p;                              // consumer productor
    BlockQueue<int> *bq = new BlockQueue<int>(); // 交易场所

    ThreadData ctd = {bq, "消费者"}; // 消费者
    ThreadData ptd = {bq, "生产者"}; // 生产者

    pthread_create(&c, nullptr, consume, (void *)&ctd);
    pthread_create(&p, nullptr, product, (void *)&ptd);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    delete bq;
    return 0;
}
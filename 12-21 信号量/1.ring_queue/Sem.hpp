#include <semaphore.h>

class Sem
{
public:
    Sem(int num) : _init_num(num)
    {
        sem_init(&_sem, 0, num);
    }
    // P V 操作
    void P()
    {
        // 先申请信号量
        sem_wait(&_sem);
        // 申请成功,生产数据
    }

    void V()
    {
        sem_post(&_sem);
    }
    ~Sem()
    {
        sem_destroy(&_sem);
    }

private:
    sem_t _sem;
    int _init_num; // 信号量初始值
};
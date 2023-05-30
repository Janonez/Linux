#include "blockQueue.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>
void* consumer(void* args)
{
    BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
    while(1)
    {
        int data = 0;
        // 1. 将数据从blockqueue中获取 -- 获取到了数据
        bq->pop(&data); // 输出参数
        // 2. 结合某种业务逻辑，处理数据
        std::cout << "consumer data: " << data << std::endl;

    }
}

void* productor(void* args)
{
    BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
    while(1)
    {
        // 1. 先通过某种渠道获取数据
        int data = rand() % 10 + 1;
        // 2. 将数据推送到blockqueue -- 完成生产过程
        bq->push(data);
        std::cout << "productor data: " << data << std::endl;
    }
}

int main()
{
    srand((uint64_t)time(nullptr) ^ getpid());
    BlockQueue<int>* bq = new BlockQueue<int>();
    
    // 单生产和单消费 --> 多生产和多消费
    pthread_t c,p;
    pthread_create(&c,nullptr,consumer,nullptr);
    pthread_create(&p,nullptr,productor,nullptr);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);

    delete bq;
    return 0;
}
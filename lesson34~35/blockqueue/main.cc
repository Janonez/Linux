#include "blockQueue.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>
// void* consumer(void* args)
// {
//     BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
//     while(1)
//     {
//         sleep(1);
//         int data = 0;
//         // 1. 将数据从blockqueue中获取 -- 获取到了数据
//         bq->pop(&data); // 输出参数
//         // 2. 结合某种业务逻辑，处理数据
//         std::cout << "consumer data: " << data << std::endl;

//     }
// }

// void* productor(void* args)
// {
//     BlockQueue<int>* bq = static_cast<BlockQueue<int>*>(args);
//     while(1)
//     {
//         // 1. 先通过某种渠道获取数据
//         int data = rand() % 10 + 1;
//         // 2. 将数据推送到blockqueue -- 完成生产过程
//         bq->push(data);
//         std::cout << "productor data: " << data << std::endl;
//     }
// }

// int main()
// {
//     srand((uint64_t)time(nullptr) ^ getpid());
//     BlockQueue<int>* bq = new BlockQueue<int>();
    
//     // 单生产和单消费 --> 多生产和多消费
//     pthread_t c,p;
//     pthread_create(&c,nullptr,consumer,bq);
//     pthread_create(&p,nullptr,productor,bq);

//     pthread_join(c,nullptr);
//     pthread_join(p,nullptr);

//     delete bq;
//     return 0;
// }


// 任务队列
#include "task.hpp"
void* consumer(void* args)
{
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
    while(1)
    {
        Task t;
        // 1. 将数据从blockqueue中获取 -- 获取到了数据
        bq->pop(&t); // 输出参数
        t();
        // 2. 结合某种业务逻辑，处理数据
        std::cout << pthread_self() << " | consumer data: " << t.formatArg() << t.formatRes() << std::endl;

    }
}

void* productor(void* args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    std::string opers = "+-*/%";
    while(1)
    {
        // 1. 先通过某种渠道获取数据
        int x = rand() % 20 + 1;
        int y = rand() % 10 + 1;
        char op = opers[rand() % opers.size()];
        // 2. 将数据推送到blockqueue -- 完成生产过程
        Task t(x, y, op);
        bq->push(t);
        std::cout << pthread_self() << " | productor Task: " <<  t.formatArg() << "?" << std::endl;
    }
}

int main()
{
    srand((uint64_t)time(nullptr) ^ getpid());
    BlockQueue<Task>* bq = new BlockQueue<Task>();
    
    // 单生产和单消费 --> 多生产和多消费
    pthread_t c[2],p[3];
    pthread_create(&c[0],nullptr,consumer,bq);
    pthread_create(&c[1],nullptr,consumer,bq);
    pthread_create(&p[0],nullptr,productor,bq);
    pthread_create(&p[1],nullptr,productor,bq);
    pthread_create(&p[2],nullptr,productor,bq);

    pthread_join(c[0],nullptr);
    pthread_join(c[1],nullptr);
    pthread_join(p[0],nullptr);
    pthread_join(p[1],nullptr);
    pthread_join(p[2],nullptr);

    delete bq;
    return 0;
}
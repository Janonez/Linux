#include "RingQueue.hpp"
#include "Task.hpp"

#include <pthread.h>
#include <memory>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// void* consumerRoutine(void* args)// 消费者
// {
//     RingQueue<int>* rq = static_cast<RingQueue<int>*>(args); // 将传入的参数args强制类型转换
//     while(1)
//     {
//         int data = 0;
//         rq->pop(&data);//输出型参数
//         cout << "consume done: " << data << endl;

//     }
// }

// void* productorRoutine(void* args)// 生产者
// {
//     RingQueue<int>* rq = static_cast<RingQueue<int>*>(args); // 将传入的参数args强制类型转换
//     while(1)
//     {
//         //sleep(1);
        
//         int data = rand() % 10 + 1;
//         rq->push(data);
//         cout << "product done: " << data << endl;
//     }
// }


// int main()
// {
//     // 种一棵随机数种子
//     srand(time(nullptr) ^ getpid());

//     RingQueue<int> *rq = new RingQueue<int>();
//     // 单生产 单消费
//     pthread_t c,p;
//     pthread_create(&c,nullptr,consumerRoutine,rq);
//     pthread_create(&p,nullptr,productorRoutine,rq);

//     // 等待线程
//     pthread_join(c,nullptr);
//     pthread_join(p,nullptr);
//     return 0;
// }

// 任务
const char* ops = "+-*/%";
void* consumerRoutine(void* args)// 消费者
{
    RingQueue<Task>* rq = static_cast<RingQueue<Task>*>(args); // 将传入的参数args强制类型转换
    while(1)
    {
        Task t;
        rq->pop(&t);//输出型参数
        t();// 处理任务也是消费必要的一环
        cout << "consume done, 处理的任务是: " << t.formatRes() << endl;
    }
}

void* productorRoutine(void* args)// 生产者
{
    RingQueue<Task>* rq = static_cast<RingQueue<Task>*>(args); // 将传入的参数args强制类型转换
    while(1)
    {
        int x = rand() % 100;
        int y = rand() % 100;
        char op = ops[(x+y)%strlen(ops)]; // 随机选择运算符
        Task t(x,y,op);
        rq->push(t);
        cout << "product done, 生产的任务是: " << t.formatArg() << endl;
    }
}

int main()
{
    // 种一棵随机数种子
    srand(time(nullptr) ^ getpid());

    RingQueue<Task> *rq = new RingQueue<Task>();
    // 单生产 单消费
    // pthread_t c,p;
    // pthread_create(&c,nullptr,consumerRoutine,rq);
    // pthread_create(&p,nullptr,productorRoutine,rq);

    // // 等待线程
    // pthread_join(c,nullptr);
    // pthread_join(p,nullptr);

    // delete rq;

    // 多生产 多消费
    // 意义：不在于从环形队列区放入和拿出去，而是在于放入前并发构建Task，获取后多线程并发处理Task，这些操作没有加锁
    pthread_t c[3],p[2];
    for(int i = 0; i < 3; i++)
    {
        pthread_create(c+i,nullptr,consumerRoutine,rq);
    }
    for(int i = 0; i < 2; i++)
    {
        pthread_create(p+i,nullptr,productorRoutine,rq);
    }
    // 等待线程
    for(int i = 0; i < 3; i++)
    {
        pthread_join(c[i],nullptr);
    }
    for(int i = 0; i < 2; i++)
    {
        pthread_join(p[i],nullptr);
    }

    delete rq;
    return 0;
}
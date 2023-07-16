#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h> // 信号量
#include <pthread.h>

static const int N = 50;

template<class T>
class RingQueue
{
private:
    void P(sem_t &s)
    {
        sem_wait(&s); // P操作 --
    }
    void V(sem_t &s)
    {
        sem_post(&s); // V操作 ++
    }
    // 多生产，多消费 -- 加锁解锁函数封装
    void Lock(pthread_mutex_t &m)
    {
        pthread_mutex_lock(&m);
    }

    void Unlock(pthread_mutex_t &m)
    {
        pthread_mutex_unlock(&m);
    }
public:
    RingQueue(int num = N)
        : _ring(num)
        , _cap(num)  
    {
        sem_init(&_data_sem,0,0);
        sem_init(&_space_sem,0,num);
        _c_step = _p_step = 0;

        // 多生产，多消费 -- 初始化两把锁
        pthread_mutex_init(&_c_mutex, nullptr);
        pthread_mutex_init(&_p_mutex, nullptr);
    }

    void push(const T& in)
    {        
        // 信号量的意义：可以不用在临界区内部做判断，就可以知道临界资源的使用情况
        //              什么时候用 锁 和用 信号量 取决于对应的临界资源是否被整体使用
        // 生产 -- 前提是有空间，先申请信号量！！！
        P(_space_sem); // P操作 --
        
        Lock(_p_mutex);// 多生产，多消费 -- 生产时先对p锁加锁

        // 走到这里一定有对应的空间资源，不用做判断
        _ring[_p_step++] = in;
        _p_step %= _cap;// 循环

        Unlock(_p_mutex);// 多生产，多消费 -- 生产时先对p锁解锁

        V(_data_sem); // V操作 ++

    }

    void pop(T* out)
    {
        // 消费 -- 前提是有数据
        P(_data_sem);

        Lock(_c_mutex);// 多生产，多消费 -- 生产时先对c锁加锁

        *out = _ring[_c_step++];
        _c_step %= _cap;// 循环

        Unlock(_c_mutex);// 多生产，多消费 -- 生产时先对c锁解锁

        V(_space_sem);
    }

    ~RingQueue()
    {
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);

        // 多生产，多消费 -- 释放两把锁
        pthread_mutex_destroy(&_c_mutex);
        pthread_mutex_destroy(&_p_mutex);
    }
private:
    std::vector<T> _ring;
    int _cap;           // 环形队列的容量大小
    sem_t _data_sem;    // 只有消费者关心
    sem_t _space_sem;   // 只有生产者关心
    int _c_step;        // 消费位置
    int _p_step;        // 生产位置

    // 多生产，多消费 -- 需要两把锁
    pthread_mutex_t _c_mutex;
    pthread_mutex_t _p_mutex;
};
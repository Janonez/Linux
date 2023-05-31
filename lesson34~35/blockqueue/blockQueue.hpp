#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>

const int gcap = 5;

template<class T>
class BlockQueue
{
public:
    BlockQueue(const int cap = gcap)
        : _cap(cap)
    {
        pthread_mutex_init(&_mutex,nullptr);
        pthread_cond_init(&_consumerCond,nullptr);
        pthread_cond_init(&_productorCond,nullptr);
    }
    bool isFull()
    {
        return _q.size() == _cap;
    }
    bool isEmpty()
    {
        return _q.empty();
    }
    void push(const T& in)
    {
        // 生产先加锁
        pthread_mutex_lock(&_mutex);
        // 先检查阻塞队列是否为满，如果满，生产者就阻塞等待
        // 只能在临界区内部，判断临界资源是否就绪（临界资源的增删查改一定在加锁解锁之间）
        // 当前一定是持有锁的
        while(isFull()) 
        {
            // 要让线程进行休眠等待，不能持有锁等待！否则会造成死锁问题
            // 所以pthread_cont_wait需要有解锁的能力，需要传入_mutex参数
            pthread_cond_wait(&_productorCond, &_mutex);
            // 对于系统带说，休眠相当于切换，
            // 当线程醒来时，继续从临界区内执行，因为是从临界区切走的

            // 当线程被唤醒的时候，继续在pthread_cont_wait函数内部向后运行，
            // 需要重新申请锁，申请成功才会返回
        }
        // 走到这里说明队列未满，让生产者生产
        _q.push(in);
        // 生产一个数据后，一定不为空，可以唤醒消费者消费（消费者在队列空时等待）
        pthread_cond_signal(&_consumerCond);
        // 解锁
        pthread_mutex_unlock(&_mutex);

    }
    void pop(T* out) // 输出型参数
    {
        // 消费也需要先申请锁
        pthread_mutex_lock(&_mutex);
        // 消费时先判断队列是否为空，如果为空就等待
        while(isEmpty()) // 循环判断，保证任何时候都符合条件
        {
            pthread_cond_wait(&_consumerCond, &_mutex);
        }
        *out = _q.front();
        _q.pop();
        // 消费一个数据后，一定不为满，可以唤醒生产者生产（生产者在队列满时等待）
        pthread_cond_signal(&_productorCond);
        // 解锁
        pthread_mutex_unlock(&_mutex);

    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumerCond);
        pthread_cond_destroy(&_productorCond);
    }

private:
    std::queue<T> _q;
    int _cap;
    pthread_mutex_t _mutex;
    // 定义两个条件变量，
    // 一个表示是否为满(满--生产者等待)，一个表示是否为空(空--消费者等待)
    pthread_cond_t _consumerCond; // 消费者对应的条件变量 -- 空 -- 等待
    pthread_cond_t _productorCond;// 生产者对应的条件变量 -- 满 -- 等待
};
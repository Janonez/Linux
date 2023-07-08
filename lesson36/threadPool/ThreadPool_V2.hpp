#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "Task.hpp"
#include "Thread.hpp"

const static int N = 5; // 线程池中线程个数

template<class T>
class ThreadPool
{
public:
    ThreadPool(int num = N)
        : _num(num)
    {
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 封装函数
    void lockQueue()
    {
        pthread_mutex_lock(&_lock);
    }
    void unlockQueue()
    {
        pthread_mutex_unlock(&_lock);
    }
    void threadWait()
    {
        pthread_cond_wait(&_cond, &_lock);
    }
    void threadWakeup()
    {
        pthread_cond_signal(&_cond);
    }
    bool isEmpty()
    {
        return _tasks.empty();
    }
    T popTask()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    // pthread_create要求threadRoutine是一个void* Routine(void*)的函数指针
    // 但是类内成员函数还有一个隐藏的参数this
    // 类内成员函数默认有参数this指针，需要static
    // 静态成员函数无法访问类内成员 -- 可以封装函数使用
    static void threadRoutine(void* args)
    {
        // 这里的args是create函数中传递的this，代表当前对象
        ThreadPool<T>* tp = static_cast<ThreadPool<T>*>(args);
        while(1)
        {
            // sleep(1);
            // std::cout << pthread_self() << " running..." << std::endl;

            // 检测有没有任务 -- 有：处理，没有：等待
            // 检测有无任务的本质就是检测队列是否为空，本质就是访问共享资源 -- 必定加锁
            tp->lockQueue();
            
            while(tp->isEmpty()) // 不能只判断一次，要循环判断
            {
                // 条件等待 _cond
                tp->threadWait();
            }
            // 执行任务
            T t = tp->popTask(); // 从公共区域拿到私有区域
            tp->unlockQueue();

            t();// 处理任务时不需要在临界区处理(仿函数处理任务)
            std::cout << "thread handler done, result: " << t.formatRes() << std::endl;
        }
    }

    void init()
    {
        // 启动线程
        for(int i = 0; i < _num; ++i)
        {
            _threads.push_back(Thread(i,threadRoutine,this));
        }
    }

    void start()
    {
        for(auto& t : _threads)
        {
            t.run();
        }
    }

    void check()
    {
        for(auto& t : _threads)
        {
            std::cout << t.name() << "running..." << std::endl;
        }
    }
    void pushTask(const T& t)
    {
        lockQueue();

        _tasks.push(t);
        threadWakeup(); // 唤醒在特定条件下等待的线程
        
        unlockQueue();

    }
    
    ~ThreadPool()
    {
        for(auto& t : _threads)
        {
            t.join();
        }
        
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
        
    }
private:
    std::vector<Thread> _threads;
    int _num;

    std::queue<T> _tasks; // 使用STL的自动扩容特性，不对队列容量做限制

    pthread_mutex_t _lock;
    pthread_cond_t _cond; // 条件变量
};
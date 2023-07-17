#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unistd.h>
#include "Thread.hpp"
#include "Task.hpp"
#include "lockGuard.hpp"
#include "log.hpp"

const static int N = 5;

template <class T>
class ThreadPool
{
private:
    ThreadPool(int num = N) : _num(num)
    {
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    ThreadPool(const ThreadPool<T> &tp) = delete;
    void operator=(const ThreadPool<T> &tp) = delete;

public:
    static ThreadPool<T> *getinstance()
    {
        if(nullptr == instance) // 为什么要这样？提高效率，减少加锁的次数！
        {
            LockGuard lockguard(&instance_lock);
            if (nullptr == instance)
            {
                // std::cout << "线程池单例形成" << std::endl;
                logMessage(Debug, "线程池单例形成");
                instance = new ThreadPool<T>();
                instance->init();
                instance->start();
            }
        }

        return instance;
    }

    pthread_mutex_t *getlock()
    {
        return &_lock;
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
    static void threadRoutine(void *args)
    {
        // pthread_detach(pthread_self());
        ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);
        while (true)
        {
            // 1. 检测有没有任务
            // 2. 有：处理
            // 3. 无：等待
            // 细节：必定加锁
            T t;
            {
                LockGuard lockguard(tp->getlock());
                while (tp->isEmpty())
                {
                    tp->threadWait();
                }
                t = tp->popTask(); // 从公共区域拿到私有区域
            }
            // for test
            t();
        }
    }
    void init()
    {
        for (int i = 0; i < _num; i++)
        {
            _threads.push_back(Thread(i, threadRoutine, this));
            // std::cout << i << " thread running" << std::endl;
            logMessage(Debug, "%d thread running", i);
        }
    }
    void start()
    {
        for (auto &t : _threads)
        {
            t.run();
        }
    }
    void check()
    {
        for (auto &t : _threads)
        {
            std::cout << t.threadname() << " running..." << std::endl;
        }
    }
    void pushTask(const T &t)
    {
        LockGuard lockgrard(&_lock);
        _tasks.push(t);
        threadWakeup();
    }
    ~ThreadPool()
    {
        for (auto &t : _threads)
        {
            t.join();
        }
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    }

private:
    std::vector<Thread> _threads;
    int _num;

    std::queue<T> _tasks; // 使用stl的自动扩容的特性

    pthread_mutex_t _lock;
    pthread_cond_t _cond;

    static ThreadPool<T> *instance;
    static pthread_mutex_t instance_lock;
};

template <class T>
ThreadPool<T> *ThreadPool<T>::instance = nullptr;

template <class T>
pthread_mutex_t ThreadPool<T>::instance_lock = PTHREAD_MUTEX_INITIALIZER;

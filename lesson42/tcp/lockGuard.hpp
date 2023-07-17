#pragma once

#include <iostream>
#include <pthread.h>

class Mutex // 自己不维护锁，有外部传入
{
public:
    Mutex(pthread_mutex_t *mutex):_pmutex(mutex)
    {}
    void lock()
    {
        pthread_mutex_lock(_pmutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(_pmutex);
    }
    ~Mutex()
    {}
private:
    pthread_mutex_t *_pmutex;
};

class LockGuard // 自己不维护锁，有外部传入
{
public:
    LockGuard(pthread_mutex_t *mutex):_mutex(mutex)
    {
        _mutex.lock();
    }
    ~LockGuard()
    {
        _mutex.unlock();
    }
private:
    Mutex _mutex;
};

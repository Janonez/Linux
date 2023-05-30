#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>

template<class T>
class BlockQueue
{
public:
    BlockQueue()
    {}
    void push(const T& in)
    {

    }
    void pop(T* out)
    {

    }
    ~BlockQueue()
    {}

private:
    std::queue<T> _q;
    int _cap;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};
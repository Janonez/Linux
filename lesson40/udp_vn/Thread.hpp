#pragma once

#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdlib>
#include <functional>

class Thread
{
public:
    typedef enum
    {
        NEW = 0,
        RUNNING,
        EXITED
    }ThreadStatus;

    // typedef void (*func_t)(void*);
    using func_t = std::function<void ()>;
public:
    Thread(int num, func_t func)
        : _tid(0)
        , _status(NEW)
        , _func(func)
    {
        char name[128];
        snprintf(name,sizeof(name),"thread-%d",num);
        _name = name;
    }
    int status()
    {
        return _status;
    }
    std::string name()
    {
        return _name;
    }
    pthread_t tid()
    {
        if(_status == RUNNING)
            return _tid;
        else 
        {
            //std::cout << "thread is not running" << std::endl;
            return 0;
        }
    }
    void operator()()// 仿函数
    {
        if(_func != nullptr)
            _func();
    }
    // 类内成员函数默认有参数this指针，需要static
    // 但是static成员函数，无法直接访问类内属性和其他成员函数
    static void* runHelper(void* args)
    {
        Thread* ts = (Thread*)args;//这里的args是create函数中传递的this，代表当前对象
        (*ts)();
        return nullptr;
    }
    void run()
    {
        int n = pthread_create(&_tid,nullptr,runHelper,this);
        if(n != 0)
            exit(1);
        
        _status = RUNNING;
    }
    void join()
    {
        int n = pthread_join(_tid,nullptr);
        if(n != 0)
        {
            std::cerr << "main thread join thread" << " error" << std::endl;
            return;
        }
        _status = EXITED;
    }
    ~Thread()
    {}
private:
    pthread_t _tid;
    std::string _name;
    func_t _func;// 线程未来执行的回调

    ThreadStatus _status;
};
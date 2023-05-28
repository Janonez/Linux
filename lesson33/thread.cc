#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "Thread.hpp"
#include "lockGuard.hpp"
using namespace std;

// class TData
// {
// public:
//     TData(const string& name, pthread_mutex_t* mutex)
//         : _name(name)
//         , _pmutex(mutex)
//     {}
//     ~TData()
//     {}

// public:
//     string _name;
//     pthread_mutex_t* _pmutex;
// };

// int tickets = 1000;// 全局变量，共享对象

// void* threadRoutine(void* args)
// {
//     TData* td = static_cast<TData*>(args);
//     while(1)
//     {
//         pthread_mutex_lock(td->_pmutex);
//         if(tickets>0)
//         {
//             usleep(2000);
//             cout << td->_name << " get a ticket: " << tickets-- << endl;
//             pthread_mutex_unlock(td->_pmutex);
//         }
//         else 
//         {
//             pthread_mutex_unlock(td->_pmutex);
//             break;
//         }
//         usleep(10);
//     }
//     return nullptr;
// }
// int main()
// {
//     pthread_mutex_t mutex;
//     pthread_mutex_init(&mutex,nullptr);

//     pthread_t tids[4];
//     int n = sizeof(tids)/sizeof(tids[0]);
//     for(int i = 0; i < n; ++i)
//     {
//         char name[64];
//         snprintf(name,64,"thread-%d",i+1);
//         TData* td = new TData(name,&mutex);
//         pthread_create(tids+i,nullptr,threadRoutine,td);
//     }
//     for(int i = 0;i<n;++i)
//     {
//         pthread_join(tids[i],nullptr);
//     }

//     pthread_mutex_destroy(&mutex);
//     return 0;
// }

// 线程封装
// void threadRun(void* args)
// {
//     string message = static_cast<const char*>(args);
//     int cnt = 10;
//     while(cnt)
//     {
//         cout << "我是一个线程, " << message << ", cnt: " << cnt-- << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     Thread t1(1,threadRun,(void*)"hello world1");
//     Thread t2(2,threadRun,(void*)"hello world2");

//     cout << "thread name: " << t1.name() << " thread id: " << t1.tid() << " thread status: " << t1.status() << endl;
//     cout << "thread name: " << t2.name() << " thread id: " << t2.tid() << " thread status: " << t2.status() << endl;
    
//     t1.run();
//     t2.run();

//     cout << "thread name: " << t1.name() << " thread id: " << t1.tid() << " thread status: " << t1.status() << endl;
//     cout << "thread name: " << t2.name() << " thread id: " << t2.tid() << " thread status: " << t2.status() << endl;

//     t1.join();
//     t2.join();

//     cout << "thread name: " << t1.name() << " thread id: " << t1.tid() << " thread status: " << t1.status() << endl;
//     cout << "thread name: " << t2.name() << " thread id: " << t2.tid() << " thread status: " << t2.status() << endl;
//     return 0;
// }

// 锁封装
int tickets = 1000;// 全局变量，共享对象
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void threadRoutine(void* args)
{
    string message = static_cast<const char*>(args);
    while(1)
    {
        LockGuard lockguard(&mutex);
        if(tickets>0)
        {
            usleep(2000);
            cout << message << " get a ticket: " << tickets-- << endl;
        }
        else 
        {
            break;
        }
        usleep(10);
    }

}
int main()
{
    Thread t1(1,threadRoutine,(void*)"hello world1");
    Thread t2(2,threadRoutine,(void*)"hello world2");
    Thread t3(3,threadRoutine,(void*)"hello world3");
    Thread t4(4,threadRoutine,(void*)"hello world4");

    
    t1.run();
    t2.run();
    t3.run();
    t4.run();


    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
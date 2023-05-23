#include <iostream>
#include <string>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <thread>

using namespace std;

// string hexAddr(pthread_t tid)
// {
//     char buffer[64];
//     snprintf(buffer,sizeof(buffer),"0x%x",tid);
//     return buffer;
// }

// void* threadRoutine(void* args)
// {
//     // pthread_detach(pthread_self());
//     string name = static_cast<const char*>(args);
//     int cnt = 5;
//     while(cnt)
//     {
//         cout << name << " : " << cnt-- << " : " << hexAddr(pthread_self()) << endl;
//         sleep(1);
//     }
// }
// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid,nullptr,threadRoutine,(void*)"thread 1");// 线程被创建，谁先执行并不确定
//     //pthread_detach(tid);// 线程detach后再join会报错
//     //sleep(1);
//     while(1)
//     {
//         cout << "main thread : " << hexAddr(pthread_self()) << " new thread : " << hexAddr(tid) << endl;
//         sleep(1);
//     }
//     int n = pthread_join(tid,nullptr);
//     if(n != 0)
//     {
//         cerr << "cerr: " << n << strerror(n) << endl;
//     }
//     //sleep(10);
//     return 0;
// }

// void run1()
// {
//     while(1)
//     {
//         cout << "thread 1" << endl;
//         sleep(1);
//     }
// }
// void run2()
// {
//     while(1)
//     {
//         cout << "thread 2" << endl;
//         sleep(1);
//     }
// }
// void run3()
// {
//     while(1)
//     {
//         cout << "thread 3" << endl;
//         sleep(1);
//     }
// }
// int main()
// {
//     thread th1(run1);
//     thread th2(run2);
//     thread th3(run3);

//     th1.join();
//     th2.join();
//     th3.join();
//     return 0;
// }

__thread int g_val = 100;

string hexAddr(pthread_t tid)
{
    char buffer[64];
    snprintf(buffer,sizeof(buffer),"0x%x",tid);
    return buffer;
}

void* threadRoutine(void* args)
{
    // pthread_detach(pthread_self());
    //sleep(1);
    string name = static_cast<const char*>(args);
    int cnt = 5;
    while(cnt)
    {
        //cout << name << " : " << cnt-- << " : " << hexAddr(pthread_self()) << " &cnt : "<< &cnt <<endl;
        cout << name <<  " g_val : " << g_val++ << ", &g_val : " << &g_val << endl;
        sleep(1);
    }
}
int main()
{
    // while(1) 
    // {
    //     cout << g_val++ << endl;
    //     sleep(1);
    // }
    pthread_t t1,t2,t3;
    pthread_create(&t1,nullptr,threadRoutine,(void*)"thread 1");// 线程被创建，谁先执行并不确定
    pthread_create(&t2,nullptr,threadRoutine,(void*)"thread 2");// 线程被创建，谁先执行并不确定
    pthread_create(&t3,nullptr,threadRoutine,(void*)"thread 3");// 线程被创建，谁先执行并不确定
    
    pthread_join(t1,nullptr);
    pthread_join(t2,nullptr);
    pthread_join(t3,nullptr);
    
    return 0;
}
// int tickets = 10000;// 加锁保证共享资源的安全
// pthread_mutex_t mutex;
// void* threadRoutine(void* name)
// {
//     string tname = static_cast<const char*>(name);

//     while(1)
//     {
//         pthread_mutex_lock(&mutex);
        
//         if(tickets>0)
//         {
//             usleep(2000);
//             cout << tname << " get a ticket: " << tickets-- << endl;
//             pthread_mutex_unlock(&mutex);
//         }
//         else 
//         {
//             pthread_mutex_unlock(&mutex);
//             break;
//         }
//         usleep(1000);
//     }
//     return nullptr;
// }
// int main()
// {
//     pthread_mutex_init(&mutex,nullptr);
//     pthread_t t[4];
//     int n = sizeof(t)/sizeof(t[0]);
//     for(int i = 0; i < n; ++i)
//     {
//         char* data = new char[64];
//         snprintf(data,64,"thread-%d",i+1);
//         pthread_create(t+i,nullptr,threadRoutine,data);
//     }
//     for(int i = 0;i<n;++i)
//     {
//         pthread_join(t[i],nullptr);
//     }
//     pthread_mutex_destroy(&mutex);
//     return 0;
// }
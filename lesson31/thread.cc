#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define NUM 10
//test
using namespace std;

// enum{ OK=0, ERROR };
// class ThreadData
// {
// public:
//     ThreadData(const string &name, int id, time_t createTime, int top)
//         :_name(name)
//         , _id(id)
//         , _createTime((uint64_t)createTime)
//         ,_status(OK)
//         , _top(top)
//         , _result(0)
//     {}
//     ~ThreadData()
//     {}
// public:
//     // 输入的
//     string _name;
//     int _id;
//     uint64_t _createTime;

//     // 返回的
//     int _status;
//     int _top;
//     int _result;
//     // char arr[n]
// };

// void *thread_run(void *args)
// {
//     // //char *name = (char *)args;
//     // //exit(11);
//     // ThreadData* td = static_cast<ThreadData*>(args);
//     // while (1)
//     // {
//     //     cout << "new thread running, name: " << td->_name << " create time: " << td->_createTime << " index: " << td->_id  << endl;
//     //     sleep(5);
//     //     //exit(11);// 进程退出，不是线程退出，只要有任何一个线程调用exit，整个进程(所有线程)全部退出！

//     //     break;
//     // }
//     // delete td;
//     // // void pthread_exit(void *retval);
//     // pthread_exit((void*)1);
//     // return nullptr;

//     ThreadData *td = static_cast<ThreadData *>(args);
//     for(int i = 1; i <= td->_top; i++)
//     {
//         td->_result += i;
//     }
//     cout << td->_name << " cal done!" << endl;
//     // pthread_exit(td);
//     return td; // 与pthread_exit(td);功能相同
// }

// // 线程终止
// // 1.函数执行完毕
// int main()
// {
//     // pthread_t t;
//     // // int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
//     // // pthread_t *thread:输出型参数，返回一个整数
//     // pthread_create(&t, nullptr, thread_run, nullptr);

//     // while(1)
//     // {
//     //     cout << "main thread running, new thread id: " << t << endl;
//     //     sleep(1);

//     // }

//     pthread_t tids[NUM];
//     for (int i = 0; i < NUM; ++i)
//     {
//         // char tname[64];//erro
//         char *tname = new char[64];
//         snprintf(tname, 64, "thread-%d", i + 1);
//         ThreadData* td = new ThreadData(tname,i+1,time(nullptr),100+5*i);
//         //pthread_create(&tids[i], nullptr, thread_run, tname);
//         pthread_create(&tids[i], nullptr, thread_run, td);
//         // &tids[i] <==> tids+i
//         sleep(1);
//     }

//     // while(1)
//     // {
//     //     cout << "main thread running, new thread id: " << endl;
//     //     sleep(1);

//     // }
//     // 等待线程
//     void* ret = nullptr;
//     for (int i = 0; i < NUM; ++i)
//     {
//         // int pthread_join(pthread_t thread, void **retval);
//         int n = pthread_join(tids[i], &ret);
//         if(n!=0) cerr<<"pthread_join error" << endl;

//         // cout << "pthread quit: "<< (uint64_t)ret << endl; 
//         ThreadData *td = static_cast<ThreadData *>(ret);
//         if(td->_status == OK)
//         {
//             cout << td->_name << " 计算的结果是: " << td->_result << " (它要计算的是[1, " << td->_top << "])" <<endl;
//         }
//         delete td;

//     }

//     cout << "all thread quit ..." << endl;
//     return 0;
// }


void *threadRun(void* args)
{
    const char*name = static_cast<const char *>(args);

    int cnt = 5;
    while(cnt)
    {
        cout << name << " is running: " << cnt-- << " obtain self id: " << pthread_self() << endl;
        sleep(1);
    }

    pthread_exit((void*)11); 

    // PTHREAD_CANCELED; #define PTHREAD_CANCELED ((void *) -1)
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, threadRun, (void*)"thread 1");
    // sleep(3);

    // pthread_cancel(tid);

    void *ret = nullptr;
    pthread_join(tid, &ret);
    cout << " new thread exit : " << (int64_t)ret << "quit thread: " << tid << endl;
    return 0;
}

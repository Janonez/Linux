#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>
#include <string>
using namespace std;

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// void* threadRoutine(void* args)
// {
//     cout << "I an a new thread " << endl;
//     pthread_mutex_lock(&mutex);
//     cout << "I got a mutex!" << endl;

//     pthread_mutex_lock(&mutex);//申请锁的问题，会导致死锁停下来
//     cout << "I alive again" << endl;
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid,nullptr,threadRoutine,nullptr);

//     sleep(3);
//     cout << "main thread run begin" << endl;
//     pthread_mutex_unlock(&mutex);
//     cout << "main thread unlock done" << endl;
//     sleep(3);
//     return 0;
// }

const int num = 5;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* active(void* args)
{
    string name = static_cast<const char*>(args);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);// pthread_cond_wait，调用的时候，会自动释放锁
        cout << name << " active" << endl;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_t tids[num];
    for(int i = 0; i < num; ++i)
    {
        char *name = new char[32];
        snprintf(name,32,"thread-%d",i+1);
        pthread_create(tids+i,nullptr,active,name);
    }
    sleep(3);
    while(1)
    {
        cout << "main thread wakeup thread..." << endl;
        // pthread_cond_signal(&cond);
        pthread_cond_broadcast(&cond);

        sleep(1);
    }
    for(int i = 0; i < num; ++i)
    {
        pthread_join(tids[i],nullptr);
    }
    return 0;
}

// test

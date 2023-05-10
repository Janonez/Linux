#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
int g_val = 0; // 全局变量，在多线程场景中，我们多个线程看到的是同一个变量
void *threadRun1(void *args)
{
    while(1)
    {
        sleep(1);
        cout << "thread1 thread..." << getpid() << " &g_val: " << &g_val << " , g_val: " << g_val << endl;
    }
}

void *threadRun2(void *args)
{
    // char *s = "hello world";
    while(1)
    {
        sleep(1);
        cout << "thread2 thread..." << getpid() << " &g_val: " << &g_val << " , g_val: " << g_val++ << endl;
        // *s = 'H'; // 让这一个线程崩溃
    }
}

int main()
{
    pthread_t t1,t2,t3;

    pthread_create(&t1,nullptr,threadRun1,nullptr);
    pthread_create(&t1,nullptr,threadRun2,nullptr);

    while(1)
    {
        sleep(1);
        cout << "main thread..." << getpid() << " &g_val: " << &g_val << " , g_val: " << g_val << endl;
    }
    return 0;
}
#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>

using namespace std;

int tickets = 10000;// 加锁保证共享资源的安全
pthread_mutex_t mutex;

void* threadRoutine(void* name)
{
    string tname = static_cast<const char*>(name);
    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        if(tickets>0)
        {
            usleep(2000);
            cout << tname << " get a ticket: " << tickets-- << endl;
            pthread_mutex_unlock(&mutex);
        }
        else 
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        usleep(1000);
    }
    return nullptr;
}
int main()
{
    pthread_mutex_init(&mutex,nullptr);
    pthread_t t[4];
    int n = sizeof(t)/sizeof(t[0]);
    for(int i = 0; i < n; ++i)
    {
        char* data = new char[64];
        snprintf(data,64,"thread-%d",i+1);
        pthread_create(t+i,nullptr,threadRoutine,data);
    }
    for(int i = 0;i<n;++i)
    {
        pthread_join(t[i],nullptr);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
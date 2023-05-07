#include <iostream>
#include <signal.h>
#include <cassert>
#include <unistd.h>
#include <cstring>

using namespace std;
static void PrintPending(const sigset_t &pending);
static void handler(int signo)
{
    cout << "对特定信号："<< signo << "执行捕捉动作" << endl;
    int cnt = 10;
    while(cnt)
    {
        cnt--;

        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);
        PrintPending(pending);
        sleep(1);
    }
}

static void PrintPending(const sigset_t &pending)
{
    cout <<"当前进程的Pending位图:" ;
    for(int signo = 1;signo <= 31;++signo)
    {
        if(sigismember(&pending,signo)) cout << "1";
        else cout << "0";
    }
    cout << endl;
}

int main()
{
    // signal(2,handler);
    // // 1. 屏蔽2号信号
    // sigset_t set,oset;
    // // 1.1 初始化
    // sigemptyset(&set);
    // sigemptyset(&oset);
    // // 1.2 将2号信号添加到set中
    // sigaddset(&set,2);
    // // 1.3 将新的信号屏蔽字设置到进程
    // sigprocmask(SIG_BLOCK,&set,&oset);

    // // 2. while获取进程的pending信号集合，以01的方式打印出来
    // int cnt = 0;
    // while(1)
    // {
    //     // 2.1 获取信号集
    //     sigset_t pending;
    //     sigemptyset(&pending);// 不是必须的
    //     int n = sigpending(&pending);
    //     assert(n == 0);
    //     (void)n;
        
    //     // 2.2 打印
    //     PrintPending(pending);

    //     // 休眠一下
    //     sleep(1);

    //     if(cnt++ == 10)
    //     {
    //         cout << "解除对2号信号的屏蔽" << endl;
    //         sigprocmask(SIG_SETMASK,&oset,nullptr);
    //     }
    // }

    struct sigaction act,oldact;
    memset(&act,0,sizeof(act));
    memset(&oldact,0,sizeof(oldact));
    act.sa_handler=handler;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask,3);
    sigaction(2,&act,&oldact);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
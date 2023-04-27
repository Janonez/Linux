#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void handler(int signo)
{
    cout << "进程收到了：" << signo << " 信号导致崩溃的" << endl;
    exit(1);
}

void showBlock(sigset_t* oset)
{
    int signo = 1;
    for(;signo <= 31;signo++)
    {
        if(sigismember(oset,signo))
        {
            cout << "1";
        }
        else
        {
            cout << "0";
        }
    }
    cout << endl;
}

int main()
{
    // SIGFPE 8 Core Floating point exception
    // signal(SIGFPE,handler);
    // int a = 10;
    // a /= 0;
    // cout << "div zero ... here" << endl;

    // SIGSEGV 11 Core Invalid memory reference
    // signal(SIGSEGV,handler);
    // int *p = nullptr;
    // *p = 100;
    // cout << "野指针问题..." << endl;

    // while(1)
    // {
    //     sleep(1);
    //     cout << "我正在被执行，我的pid是"<<getpid() << endl;
    // }

    // cout << "野指针问题..." << endl;
    // cout << "野指针问题..." << endl;
    // cout << "野指针问题..." << endl;

    // int *p = nullptr;
    // *p = 100;

    // cout << "野指针问题..." << endl;
    // cout << "野指针问题..." << endl;
    // cout << "野指针问题..." << endl;

    // pid_t id = fork();
    // if (id == 0)
    // {
    //     cout << "野指针问题..." << endl;
    //     cout << "野指针问题..." << endl;
    //     cout << "野指针问题..." << endl;

    //     int *p = nullptr;
    //     *p = 100;

    //     cout << "野指针问题..." << endl;
    //     cout << "野指针问题..." << endl;
    //     cout << "野指针问题..." << endl;

    //     exit(0);
    // }
    // int status = 0;
    // waitpid(id,&status,0);
    // cout << "exit code: " <<  ((status>>8) & 0xff) << endl;
    // cout << "exit signal: " <<  (status & 0x7f) << endl;
    // cout << "core dump flag: " <<  ((status>>7) & 0x1) << endl;

    // #define SIG_DFL	((__sighandler_t) 0)		/* Default action.  */
    // #define SIG_IGN	((__sighandler_t) 1)		/* Ignore signal.  */
    //signal(2, SIG_DFL);
    // signal(2, SIG_IGN);

    // while(true)
    // {
    //     sleep(1);
    // }

    // 在用户层面进行设置
    sigset_t set,oset;
    sigemptyset(&set);
    sigemptyset(&oset);
    sigaddset(&set,2);// SIGINT

    // 设置进入进程，谁调用，设置谁
    sigprocmask(SIG_SETMASK,&set,&oset);
    int cnt = 0;
    while(1)
    {
        showBlock(&oset);
        sleep(1);
        cnt++;

        if(cnt == 10)
        {
            cout << "recover block" << endl;
            sigprocmask(SIG_SETMASK,&oset,&set);
            showBlock(&set);
        }

    }
    return 0;
}
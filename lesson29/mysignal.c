#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

// volatile int quit = 0;// 保证内存可见性

// void handler(int signo)
// {
//     printf("change quit from 0 to 1\n");
//     quit = 1;
//     printf("quit:%d\n",quit);
// }
// int main()
// {
//     signal(2,handler);
//     while(!quit);
//     printf("main quit 正常\n");

//     return 0;
// }


// pid_t id;

// void waitProcess(int signo)
// {
//     // 父进程捕捉到的信号
//     printf("捕捉到一个信号: %d, who: %d\n", signo, getpid());
//     sleep(5);
//     while (1)
//     {
//         pid_t res = waitpid(-1, NULL, WNOHANG);
//         if (res > 0)
//         {
//             printf("wait process,res: %d, id: %d\n", res, id);
//         }
//         else break;
//     }
//     printf("handler done...\n");
// }

// int main()
// {
//     signal(SIGCHLD, waitProcess);
//     signal(SIGCHLD, SIG_IGN);
//     int i = 1;
//     for (; i <= 10; i++)
//     {
//         id = fork();
//         if (id == 0)
//         {
//             // child
//             int cnt = 5;
//             while (cnt)
//             {
//                 printf("我是子进程,我的pid是%d,我的ppid是%d\n", getpid(), getppid());
//                 sleep(1);
//                 cnt--;
//             }
//             exit(1);
//         }
//     }
//     while (1)
//     {
//         sleep(1);
//     }
//     return 0;
// }

#include <pthread.h>

void *thread1_run(void *args)
{
    while(1)
    {
        printf("我是线程1， 我正在运行\n");
        sleep(1);
    }
}

void *thread2_run(void *args)
{
    while(1)
    {
        printf("我是线程2， 我正在运行\n");
        sleep(1);
    }
}

void *thread3_run(void *args)
{
    while(1)
    {
        printf("我是线程3， 我正在运行\n");
        sleep(1);
    }
}

int main()
{
    pthread_t t1, t2,t3;
    pthread_create(&t1, NULL, thread1_run, NULL);
    pthread_create(&t2, NULL, thread2_run, NULL);
    pthread_create(&t3, NULL, thread3_run, NULL);


    while(1)
    {
        printf("我是主线程， 我正在运行\n");
        sleep(1);
    }

}
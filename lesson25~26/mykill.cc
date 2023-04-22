#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// void handler(int signo)
// {
//     std::cout << "get a signo : " << signo << std::endl;
// }
// int main()
// {
//     //sighandler_t  signal(int signum, sighandler_t handler);
//     //signal(2,handler);
    
//     for(int i = 1;i<=31;++i)
//     {
//         signal(i,handler);
//     }

//     while(1)
//     {
//         std::cout << "我是一个进程，我正在运行，pid：" << getpid() << std::endl;
//         sleep(1);
//     }
//     return 0;
// }


void Usage(std::string proc)
{
    std::cout << "Usage: \n\t";
    std::cout << proc << "信号编号 目标进程\n" <<std::endl;
}

int count = 0;
void myhandler(int signo)
{
    std::cout << "get a signal, " << signo << " count: " << count <<std::endl;
    //exit(0);
    //alarm(1);
    int n = alarm(10);
    std::cout << "return: " << n << std::endl;
}

//  ./mykill 9 1323
int main(int argc, char* argv[])
{
    // if(argc != 3)
    // {
    //     Usage(argv[0]);
    //     exit(1);
    // }
    // int signo = atoi(argv[1]);
    // int target_id =atoi(argv[2]);
    // int n = kill(target_id, signo);
    // if(n!=0)
    // {
    //     std::cerr<< errno << " : " << strerror(errno) <<   std::endl;
    //     exit(2);
    // }

    // signal(SIGINT, myhandler);
    
    // while(1)
    // {
    //     raise(2);
    //     sleep(1);
    // }

    // std::cout << "begin" << std::endl;
    // sleep(1);
    // abort(); // 给自己发送指定的信号
    // std::cout << "end" << std::endl;

    // signal(SIGABRT,myhandler);
    // while(1)
    // {
    //     std::cout << "begin" << std::endl;
    //     sleep(1);
    //     abort(); // 给自己发送指定的信号, 即使处理了自定义任务，仍然会退出
    //     std::cout << "end" << std::endl;
    // }

    // alarm(1);
    // int count = 0;
    // while(1)
    // {
    //     std::cout << "cout : " << count++ << std::endl;
    // }


    // signal(SIGALRM,myhandler);
    // alarm(1); // alarm的设置是一次性的
    // while(1) ++count;

    std::cout << "pid: " << getpid() << std::endl;
    signal(SIGALRM,myhandler);
    alarm(10);
    while(1)
    {
        sleep(1);
        //alarm(1);
    }
    return 0;
}
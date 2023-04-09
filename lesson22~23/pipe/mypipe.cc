#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <string.h>
#include <cassert>
#include <string>
#include <sys/types.h>

int main()
{
    // 让不同的进程看到同一份资源
    // 任何一种进程间通信中，一定要 先 保证不同进程之间看到同一份资源
    int pipefd[2] = {0};
    // 1. 创建管道
    int n = pipe(pipefd);
    if(n < 0)
    {
        std::cout << "pipe error, " << errno << ": " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "pipefd[0]: " << pipefd[0] << std::endl; // 读端 0->嘴巴->读书
    std::cout << "pipefd[1]: " << pipefd[1] << std::endl; // 写端 1->笔->写东西

    // 2. 创建子进程
    pid_t id = fork();
    assert(id != -1 ); // 正常应该判断
    // 判断错误，意料之中用 assert(release版本不会执行assert), 意料之外用if

    if(id == 0)
    {
        // 子进程
        // 3. 关闭不需要的fd(文件描述符)，父进程读取，子进程写入
        close(pipefd[0]);
        // 4. 开始通信 
        // const std::string namestr = "hello, i am child";
        // int cnt = 1;
        // char buffer[1024];
        // while(1)
        // {
        //     snprintf(buffer,sizeof(buffer),"%s, count: %d,mypid: %d\n",namestr.c_str(),cnt++,getpid());
        //     write(pipefd[1],buffer,strlen(buffer));
        //     //sleep(1);
        // }

        int cnt = 0;
        while(1)
        {
            char x = 'X';
            write(pipefd[1],&x,1);
            std::cout << "cnt: " << cnt++ << std::endl;
            //sleep(5);
            //break;   
            sleep(1);
        }

        close(pipefd[1]);
        exit(0);
    }

    // 父进程
    // 3. 关闭不需要的fd(文件描述符)，父进程读取，子进程写入
    close(pipefd[1]);

    // 4. 开始通信 
    char buffer[1024];
    while(1)
    {
        //sleep(1);
        // ssize_t read(int fd, void *buf, size_t count);  
        // 这里的count 是请求读的字节数，实际读到的可能比count小
        int n = read(pipefd[0],buffer,sizeof(buffer) - 1);
        if(n > 0)
        {
            buffer[n] = '\0';
            std::cout << "i am father,child give me message: " << buffer << std::endl;
        }
        else if(n == 0)
        {
            std::cout << "i am father,i read to the end" << std::endl;
            break;
        }
        else
        {
            std::cout << "i am father,i read something error" << std::endl;
            break;
        }
        sleep(1);
        break;

    }

    close(pipefd[0]);
    return 0;
}
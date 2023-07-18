#pragma once

// 1. setsid()
// 2. 调用进程不能是组长
// 3. 守护进程 a. 忽略异常信号 b. 0,1,2要做特殊处理 c. 进程的工作路径可能要更改

#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.hpp"
#include "err.hpp"

// 守护进程本质：孤儿进程
void Daemon()
{

    // 忽略异常信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // 调用进程不能是组长 -- 退出父进程，让子进程运行
    if (fork() > 0)
        exit(0);

    // 新建会话，自己成为会话的话首进程 -- setsid()
    pid_t ret = setsid();
    if ((int)ret == -1)
    {
        logMessage(Fatal, "daemon error, code: %d, string: %s", errno, strerror(errno));
        exit(SETSID_ERR);
    }

    // 进程的工作路径可能要更改（可选）
    // chdir("/");

    // 0,1,2要做特殊处理
    // /dev/null -- 相当于一个垃圾箱
    int fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
        logMessage(Fatal, "open error, code: %d, string: %s", errno, strerror(errno));
        exit(OPEN_ERR);
    }
    // 重定向
    // int dup2(int oldfd, int newfd);
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    close(fd);
}
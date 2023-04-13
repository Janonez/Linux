#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "comm.hpp"

int main()
{
    // 1. 创建管道文件，只需要创建一次
    // int mkfifo(const char *pathname, mode_t mode);
    // mkfifo() 以pathname命名，创建一个特殊的队列文件，mode权限
    umask(0);
    int n = mkfifo(fifoname.c_str(),mode);
    if(n != 0)
    {
        std::cout << errno << " : " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "create fifo file success" << std::endl;

    // 2. 让服务端直接开启管道文件
    // int open(const char *pathname, int flags);
    int rfd = open(fifoname.c_str(),O_RDONLY);
    // 返回文件描述符
    if(rfd < 0)
    {
        std::cout << errno << " : " << strerror(errno) << std::endl;
        return 2;
    }
    std::cout << "open fifo success, begin ipc" << std::endl;

    // 3. 正常通信
    char buffer[NUM];
    while(1)
    {
        buffer[0] = 0; // 清空
        // ssize_t read(int fd, void *buf, size_t count);
        ssize_t n = read(rfd,buffer,sizeof(buffer) - 1);
        // 返回读取字节数
        if(n > 0)
        {
            buffer[n] = 0;
            std::cout << "client# " << buffer << std::endl;
        }
        else if(n == 0)
        {
            std::cout << "client quit, me too" << std::endl;
            break;
        }
        else 
        {
            std::cout << errno << " : " << strerror(errno) << std::endl;
            break;
        }
    }

    // 关闭不要的fd
    close(rfd);

    unlink(fifoname.c_str());
    return 0;
}


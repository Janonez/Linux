#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include "comm.hpp"

int main()
{
    // 1. 不需创建管道文件，只需要打开对应的文件即可！
    int wfd = open(fifoname.c_str(),O_WRONLY);
    if(wfd < 0)
    {
        std::cerr << errno << " : " << strerror(errno) << std::endl;
        return 1;
    }

    // 常规通信
    char buffer[NUM];
    while(1)
    {

        std::cout << "请输入数据# ";
        char *msg = fgets(buffer,sizeof(buffer),stdin);
        assert(msg);
        (void)msg;
        buffer[strlen(buffer)-1] = 0;
        // 123\n\0
        // ssize_t write(int fd, const void *buf, size_t count);

        if(strcasecmp(buffer, "quit") == 0) break;

        ssize_t n = write(wfd, buffer, strlen(buffer));
        assert(n >= 0);
        (void)n;
    }


    close(wfd);
    return 0;
}


#pragma once

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/epoll.h>

#include "Log.hpp"
#include "Err.hpp"

static const int defalutepfd = -1;
static const int gsize = 128;

class Epoller
{
public:
    Epoller()
        : epfd_(defalutepfd)
    {}
    void Create()
    {
        epfd_ = epoll_create(gsize);
        if(epfd_ < 0)
        {
            logMessage(Fatal, "epoll_create error, code: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }
    // 用户告诉内核 关心哪个文件描述符的哪个事件
    bool AddModEvent(int fd, uint32_t events, int op)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        int n = epoll_ctl(epfd_, op, fd, &ev);
        if(n < 0)
        {
            logMessage(Fatal, "epoll_ctl error, code: %d, errstring: %s", errno, strerror(errno));
            return false;
        }
        return true;
    }
    int Wait(struct epoll_event* revs, int num, int timeout)
    {
        return epoll_wait(epfd_, revs, num, timeout);
    }
    bool DelEvent(int fd)
    {
        return epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
    }
   
    int Fd()
    {
        return epfd_;
    }
    void Close()
    {
        if(epfd_ != defalutepfd)
        {
            close(epfd_);
        }
    }
    ~Epoller()
    {}

private:
    int epfd_;
};
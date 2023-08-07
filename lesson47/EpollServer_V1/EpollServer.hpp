#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <assert.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"
#include "Epoller.hpp"

const static uint16_t gport = 8888;

using func_t = std::function<std::string(std::string)>;

class EpollServer
{
    const static int gnum = 64;

public:
    EpollServer(func_t func, uint16_t port = gport)
        : func_(func), port_(port)
    {
    }
    void InitServer()
    {
        listen_sock_.Socket();
        listen_sock_.Bind(port_);
        listen_sock_.Listen();

        epoller_.Create();

        logMessage(Debug, "init server success");
    }
    int Start()
    {
        // 1. 将listen_sock_添加到epoll中
        bool r = epoller_.AddEvent(listen_sock_.Fd(), EPOLLIN);
        assert(r);
        (void)r;
        int timeout = -1;
        while (true)
        {
            int n = epoller_.Wait(revs_, gnum, timeout);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout...");

                break;
            case -1:
                logMessage(Warning, "epoll_wait failed");

                break;
            default:
                logMessage(Debug, "有%d个事件就绪", n);
                HandlerEvents(n);
                break;
            }
        }
    }
    void HandlerEvents(int num)
    {
        for (int i = 0; i < num; ++i)
        {

            int fd = revs_[i].data.fd;
            uint32_t events = revs_[i].events;
            logMessage(Debug, "当前正在处理%d上的%s事件", fd, (events & EPOLLIN) ? "EPOLLIN" : "OTHER");
            if (events & EPOLLIN)
            {
                // 1. 新连接
                if (fd == listen_sock_.Fd())
                {
                    // logMessage(Debug, "get a new link ...");
                    std::string clientip;
                    uint16_t clientport;
                    int sock = listen_sock_.Accept(&clientip, &clientport);
                    if (sock < 0)
                    {
                        continue;
                    }
                    logMessage(Debug, "%s:%d - 成功连接服务器\n", clientip.c_str(), clientport);
                    bool r = epoller_.AddEvent(sock, EPOLLIN);
                    assert(r);
                    (void)r;
                }
                else
                {
                    // 2. 读取事件
                    char request[1024];
                    ssize_t s = recv(fd, request, sizeof(request) - 1, 0);
                    if (s > 0)
                    {
                        request[s - 1] = 0;
                        request[s - 2] = 0;

                        std::string respense = func_(request);

                        send(fd, respense.c_str(), respense.size(), 0);
                    }
                    else
                    {
                        if (s == 0)
                        {
                            logMessage(Info, "client quit...");
                        }
                        else
                        {
                            logMessage(Warning, "recv error, client quit...");
                        }
                        // epoll_ctl中的fd必须合法，先处理
                        epoller_.DelEvent(fd);
                        close(fd);
                    }
                }
            }
        }
    }
    ~EpollServer()
    {
        listen_sock_.Close();
        epoller_.Close();
    }

private:
    uint16_t port_;
    Sock listen_sock_;
    Epoller epoller_;
    struct epoll_event revs_[gnum];
    func_t func_;
};
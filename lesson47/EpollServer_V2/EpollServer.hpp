#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <assert.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"
#include "Epoller.hpp"

const static uint16_t gport = 8888;

using func_t = std::function<std::string(std::string)>;

class Connection
{
public:
    Connection(int fd)
        : fd_(fd)
    {
    }
    ~Connection()
    {
    }

public:
    int fd_;
    std::string inbuffer_;
    std::string outbuffer_;
};

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
        // 0. 为listen_sock_创建对应的connection对象
        Connection* conn = new Connection(listen_sock_.Fd());
        // 1. 把listen_sock_的connection对象添加到connections中
        connections_.insert(std::pair<int,Connection*>(listen_sock_.Fd(), conn));
        // 2. 将listen_sock_添加到epoll中
        bool r = epoller_.AddEvent(listen_sock_.Fd(), EPOLLIN);
        assert(r);
        (void)r;
    }

    int Start()
    {

        int timeout = -1;
        while (true)
        {
            LoopOnce(timeout);
        }
    }
    void AddConnection()
    {}
    void Accepter()
    {
        // logMessage(Debug, "get a new link ...");
        std::string clientip;
        uint16_t clientport;
        int sock = listen_sock_.Accept(&clientip, &clientport);
        if (sock < 0)
        {
            return;
        }
        logMessage(Debug, "%s:%d - 成功连接服务器\n", clientip.c_str(), clientport);

        // 0. 为sock创建对应的connection对象
        Connection* conn = new Connection(sock);
        // 1. 把sock的connection对象添加到connections中
        connections_.insert(std::pair<int,Connection*>(sock, conn));
        bool r = epoller_.AddEvent(sock, EPOLLIN);
        assert(r);
        (void)r;
    }
    void Reciver(int fd)
    {
        // 2. 读取事件
        char request[1024];
        ssize_t s = recv(fd, request, sizeof(request) - 1, 0);
        if (s > 0)
        {
            request[s - 1] = 0;
            request[s - 2] = 0;
            connections_[fd]->inbuffer_ += request;

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
    void Sender()
    {
    }
    void LoopOnce(int timeout)
    {
        int n = epoller_.Wait(revs_, gnum, timeout);

        for (int i = 0; i < n; ++i)
        {

            int fd = revs_[i].data.fd;
            uint32_t events = revs_[i].events;
            logMessage(Debug, "当前正在处理%d上的%s事件", fd, (events & EPOLLIN) ? "EPOLLIN" : "OTHER");
            if (events & EPOLLIN)
            {
                // 1. 新连接
                if (fd == listen_sock_.Fd())
                {
                    Accepter();
                }
                else
                {
                    Reciver(fd);
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
    std::unordered_map<int, Connection *> connections_;
};
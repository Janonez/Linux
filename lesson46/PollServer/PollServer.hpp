#pragma once

#include <iostream>
#include <string>
#include <sys/poll.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"

const static uint16_t gport = 8888;
typedef struct pollfd type_t;
const static int defaultevent = 0;
const static int N = 4096;

class PollServer
{
public:
    PollServer(uint16_t port = gport)
        : port_(port)
        , fdarray_(nullptr)
    {
    }
    void InitServer()
    {
        listen_sock_.Socket();
        listen_sock_.Bind(port_);
        listen_sock_.Listen();
        fdarray_ = new type_t[N];
        for (int i = 0; i < N; ++i)
        {
            fdarray_[i].fd = defaultfd;
            fdarray_[i].events = defaultevent;
            fdarray_[i].revents = defaultevent;
        }
    }
    void Accepter()
    {
        // std::cout << "有一个新连接来了" << std::endl;
        uint16_t client_port;
        std::string client_ip;
        int sock = listen_sock_.Accept(&client_ip, &client_port);
        if (sock < 0)
        {
            return;
        }
        // 得到了对应的sock，不可以进行read/recv读取sock，如果sock上面没有数据，又进入了阻塞等待
        // 我们需要把sock交给select管理
        logMessage(Debug, "[%s: %d], sock: %d", client_ip.c_str(), client_port, sock);
        // 把sock添加到fdarray_[]中，让selec管理
        int pos = 1;
        for (; pos < N; ++pos)
        {
            if (fdarray_[pos].fd == defaultfd)
            {
                // 找到了数组中的空位跳出
                break;
            }
        }
        if (pos >= N) // 没有空位--动态扩容
        {
            close(sock);// 扩容失败
            logMessage(Warning, "sockfd array[] full");
        }
        else // 找到空位
        {
            fdarray_[pos].fd = sock;
            fdarray_[pos].events = POLLIN;
            fdarray_[pos].revents = defaultevent;
        }
    }
    void HandlerEvent()
    {
        for (int i = 0; i < N; ++i)
        {
            int fd = fdarray_[i].fd;
            short revent = fdarray_[i].revents;
            if (fd == defaultfd)
            {
                continue;
            }
            if (fd == listen_sock_.Fd() && (revent & POLLIN))
            {
                Accepter();
            }
            else if (fd != listen_sock_.Fd() && (revent & POLLIN))
            {  
                char buffer[1024];
                ssize_t s = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (s > 0)
                {
                    buffer[s] = 0;
                    std::cout << "client # " << buffer << std::endl;
                    fdarray_[i].events |= POLLOUT;

                    std::string echo = buffer;
                    echo += "[select server echo]";
                    send(fd, echo.c_str(), echo.size(), 0);
                }
                else
                {
                    if (s == 0)
                    {
                        logMessage(Info, "client quit..., fdarray_[i]->defaultfd: %d->%d", fd, defaultfd);
                    }
                    else
                    {
                        logMessage(Warning, "recv error, client quit..., fdarray_[i]->defaultfd: %d->%d", fd, defaultfd);
                    }
                    close(fd);
                    fdarray_[i].fd = defaultfd;
                    fdarray_[i].events = defaultevent;
                    fdarray_[i].revents = defaultevent;
                }
            }
        }
    }

    void Start()
    {
        // 不能直接获取最新的链接
        // 最开始的时候，服务器只有一个sock，listen_sock_
        // 在网络中，新链接到来被当做读事件就绪
        // listen_sock_.Accept(); // 不能直接连接

        // 因为rfds是一个输入输出型参数，注定了每次都要对rfds进行重置，就一定要知道历史上有哪些fd，在数组fdarray_[]
        // 在服务器运行过程中，sockfd的值一直在动态变化，所以maxfd也一定在变化，maxfd也要动态更新，在数组fdarray_[]
        fdarray_[0].fd = listen_sock_.Fd();
        fdarray_[0].events = POLLIN;

        while (1)
        {
            int timeout = -1;
            int n = poll(fdarray_, N, timeout); // 阻塞等待
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout, %d: %s", errno, strerror(errno));
                break;
            case -1:
                logMessage(Warning, "error, %d: %s", errno, strerror(errno));
                break;

            default:
                logMessage(Debug, "有一个就绪事件发生了，%d", n);
                HandlerEvent();
                DebugPrint();
                break;
            }

            sleep(1);
        }
    }
    void DebugPrint()
    {
        std::cout << "fdarray[]: ";
        for (int i = 0; i < N; ++i)
        {
            if (fdarray_[i].fd == defaultfd)
            {
                continue;
            }
            std::cout << fdarray_[i].fd << " ";
        }
        std::cout << std::endl;
    }

    ~PollServer()
    {
        listen_sock_.Close();
    }

private:
    uint16_t port_;
    Sock listen_sock_;
    type_t* fdarray_;
};
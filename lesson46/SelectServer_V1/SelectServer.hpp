#pragma once

#include <iostream>
#include <string>
#include <sys/select.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"

const static uint16_t gport = 8888;
typedef int type_t;
// static const int defaultfd = -1;
class SelectServer
{
public:
    static const int N = sizeof(fd_set) * 8;
    SelectServer(uint16_t port = gport)
        : port_(port)
    {
    }
    void InitServer()
    {
        listen_sock_.Socket();
        listen_sock_.Bind(port_);
        listen_sock_.Listen();

        for (int i = 0; i < N; ++i)
        {
            fdarray_[i] = defaultfd;
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
            if (fdarray_[pos] == defaultfd)
            {
                // 找到了数组中的空位跳出
                break;
            }
        }
        if (pos >= N) // 没有空位
        {
            close(sock);
            logMessage(Warning, "sockfd array[] full");
        }
        else // 找到空位
        {
            fdarray_[pos] = sock;
        }
    }
    void HandlerEvent(fd_set &rfds)
    {
        for (int i = 0; i < N; ++i)
        {
            if (fdarray_[i] == defaultfd)
            {
                continue;
            }
            if (fdarray_[i] == listen_sock_.Fd() && FD_ISSET(listen_sock_.Fd(), &rfds))
            {
                Accepter();
            }
            else if (fdarray_[i] != listen_sock_.Fd() && FD_ISSET(fdarray_[i], &rfds))
            {
                // 其他套接字
                // ServerIO();
                int fd = fdarray_[i];
                char buffer[1024];
                ssize_t s = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (s > 0)
                {
                    buffer[s] = 0;
                    std::cout << "client # " << buffer << std::endl;

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
                    close(fdarray_[i]);
                    fdarray_[i] = defaultfd;
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
        fdarray_[0] = listen_sock_.Fd();

        while (1)
        {
            fd_set rfds;
            FD_ZERO(&rfds); // 清零
            int max_fd = fdarray_[0];
            for (int i = 0; i < N; ++i)
            {
                if (fdarray_[i] == defaultfd)
                {
                    continue;
                }
                // 合法fd
                FD_SET(fdarray_[i], &rfds);
                if (max_fd < fdarray_[i])
                {
                    max_fd = fdarray_[i];
                }
            }

            // struct timeval timeout = {2, 0};
            // int n = select(listen_sock_.Fd() + 1,&rfds,nullptr,nullptr,&timeout);
            int n = select(max_fd + 1, &rfds, nullptr, nullptr, nullptr); // 阻塞等待
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
                HandlerEvent(rfds);
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
            if (fdarray_[i] == defaultfd)
            {
                continue;
            }
            std::cout << fdarray_[i] << " ";
        }
        std::cout << std::endl;
    }

    ~SelectServer()
    {
        listen_sock_.Close();
    }

private:
    uint16_t port_;
    Sock listen_sock_;
    type_t fdarray_[N];
};
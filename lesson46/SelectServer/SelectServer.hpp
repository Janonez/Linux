#pragma once

#include <iostream>
#include <string>
#include <sys/select.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"

const static uint16_t gport = 8888;

class SelectServer
{
public:
    SelectServer(uint16_t port = gport)
        : port_(port)
    {
    }
    void InitServer()
    {
        listen_sock_.Socket();
        listen_sock_.Bind(port_);
        listen_sock_.Listen();
    }
    void HandlerEvent(fd_set &rfds)
    {
        if(FD_ISSET(listen_sock_.Fd(),&rfds))
        {
            std::cout << "有一个新连接来了" << std::endl;
        }
    }

    void Start()
    {
        // 不能直接获取最新的链接
        // 最开始的时候，服务器只有一个sock，listen_sock_
        // 在网络中，新链接到来被当做读事件就绪
        // listen_sock_.Accept(); // 不能直接连接
        fd_set rfds;
        FD_ZERO(&rfds); // 清零
        FD_SET(listen_sock_.Fd(), &rfds);
        while (1)
        {
            struct timeval timeout = {2, 0};
            // int n = select(listen_sock_.Fd() + 1,&rfds,nullptr,nullptr,&timeout);
            int n = select(listen_sock_.Fd() + 1, &rfds, nullptr, nullptr, nullptr); // 阻塞等待
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
                break;
            }

            sleep(1);
        }
    }
    ~SelectServer()
    {
        listen_sock_.Close();
    }

private:
    uint16_t port_;
    Sock listen_sock_;
};
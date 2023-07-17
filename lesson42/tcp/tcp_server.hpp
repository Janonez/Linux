#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "err.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include "log.hpp"

namespace ns_server
{
    static const uint16_t defaultport = 8081;
    static const int backlog = 32;

    using func_t = std::function<std::string(const std::string&)>;

    class TcpServer;

    class ThreadData
    {
    public:
        ThreadData(int fd, const std::string &ip, const uint16_t &port, TcpServer *ts)
            : sock(fd), clientip(ip), clientport(port), current(ts)
        {
        }

    public:
        int sock;
        std::string clientip;
        uint16_t clientport;
        TcpServer *current;
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, uint16_t port = defaultport)
            : func_(func)
            , port_(port)
            , quit_(true)
        {
        }

        void initServer()
        {
            // 1.创建socket,文件
            listensock_ = socket(AF_INET, SOCK_STREAM, 0);
            if (listensock_ < 0)
            {
                //std::cerr << "create socket error" << std::endl;
                logMessage(Fatal,"create socket error, code: %d, error string: %s",errno, strerror(errno));
                exit(SOCKET_ERR);
            }
            logMessage(Info, "create socket success, code: %d ,error string: %s",errno,strerror(errno));
            // 2.bind
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port_);
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(listensock_, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                // std::cerr << "bind socket error" << std::endl;
                logMessage(Fatal,"bind socket error, code: %d, error string: %s",errno, strerror(errno));
                exit(BIND_ERR);
            }
            logMessage(Info,"bind socket success, code: %d, error string: %s",errno, strerror(errno));
            
            // 3. 监听
            if (listen(listensock_, backlog) < 0)
            {
                // std::cerr << "listen socket error" << std::endl;
                logMessage(Fatal,"listen socket error, code: %d, error string: %s",errno, strerror(errno));
                exit(LISTEN_ERR);
            }
            logMessage(Info,"listen socket success, code: %d, error string: %s",errno, strerror(errno));
            
        }
        void start()
        {
            // signal(SIGCHLD, SIG_IGN); //ok, 我最后推荐的！
            // signal(SIGCHLD, handler); // 回收子进程，不太推荐
            quit_ = false;
            while (!quit_)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                // 4. 获取连接，accept
                int sock = accept(listensock_, (struct sockaddr *)&client, &len);
                if (sock < 0)
                {
                    // std::cerr << "accept error" << std::endl;
                    logMessage(Warning,"accept error, code: %d, error string: %s",errno, strerror(errno));

                    continue;
                }

                // 提取client信息 -- debug
                std::string clientip = inet_ntoa(client.sin_addr); // 将四字节ip转换为字符串
                uint16_t clientport = ntohs(client.sin_port);
                //std::cout << clientip << "-" << clientport << std::endl;

                // 5. 获取新连接成功,就开始进行业务处理
                logMessage(Info,"获取连接成功：%d from %d, who: %s - %d", sock,listensock_,clientip.c_str(),clientport);
                
                // v4:线程池
                // 使用线程池的时候，线程是有限的，一定要处理短任务
                Task t(sock, clientip, clientport, std::bind(&TcpServer::service, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                ThreadPool<Task>::getinstance()->pushTask(t);
            }
        }

        static void *threadRoutine(void *args)
        {
            pthread_detach(pthread_self());

            ThreadData *td = static_cast<ThreadData *>(args);
            td->current->service(td->sock, td->clientip, td->clientport);
            delete td;
            return nullptr;
        }

        void service(int sock, const std::string &clientip, const uint16_t &clientport)
        {
            std::string who = clientip + "-" + std::to_string(clientport);
            char buffer[1024];

            ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
            if (s > 0)
            {
                buffer[s] = 0;
                std::string res = func_(buffer); // 进行回调
                //std::cout << who << " >>> " << res << std::endl;
                logMessage(Debug,"%s# %s",who.c_str(),res.c_str());
                write(sock, res.c_str(), res.size());
            }
            else if (s == 0)
            {
                // 对方将连接关闭了
                //std::cout << who << " quit, me too" << std::endl;
                logMessage(Info,"%s quit , me too", who.c_str());
            }
            else
            {
                //std::cerr << "read error: " << strerror(errno) << std::endl;
                logMessage(Error,"read error, %d : %s",errno,strerror(errno));
            }
            close(sock);
        }

        ~TcpServer()
        {
        }

    private:
        uint16_t port_;
        int listensock_;
        bool quit_;
        func_t func_;
    };
}

#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "err.hpp"


namespace ns_server
{
    static const uint16_t defaultport = 8081;
    static const int backlog = 32;

    using func_t = std::function<std::string (std::string)>;

    class ThreadData
    {
    public:
        ThreadData(int fd, const std::string &ip, const uint16_t &port, TcpServer *ts)
            : sock(fd)
            , clientip(ip)
            , clientport(port)
            , current(ts)
        {}
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
            listensock_ = socket(AF_INET,SOCK_STREAM,0);
            if(listensock_ < 0)
            {
                std::cerr << "create socket error" << std::endl;
                exit(SOCKET_ERR);
            }
            // 2.bind
            struct sockaddr_in local;
            memset(&local,0,sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port_);
            local.sin_addr.s_addr = INADDR_ANY;
            if(bind(listensock_,(struct sockaddr*)&local,sizeof(local)) < 0)
            {
                std::cerr << "bind socket error" << std::endl;
                exit(BIND_ERR);
            }
            // 3. 监听
            if(listen(listensock_, backlog) < 0)
            {
                std::cerr << "listen socket error" << std::endl;
                exit(LISTEN_ERR);
            }
        }
        void start()
        {
            // signal(SIGCHLD, SIG_IGN); //ok, 我最后推荐的！
            // signal(SIGCHLD, handler); // 回收子进程，不太推荐
            quit_ = false;
            while(!quit_)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                // 4. 获取连接，accept
                int sock = accept(listensock_, (struct sockaddr*)&client,&len);
                if(sock < 0)
                {
                    std::cerr << "accept error" << std::endl;
                    continue;
                }

                // 提取client信息 -- debug
                std::string clientip = inet_ntoa(client.sin_addr); // 将四字节ip转换为字符串
                uint16_t clientport = ntohs(client.sin_port);
                std::cout << clientip << "-" << clientport << std::endl;


                // 5. 获取新连接成功,就开始进行业务处理
                std::cout << "获取新连接成功: " << sock << " from " << listensock_ << ", "
                          << clientip << "-" << clientport << std::endl;
                // v1版本
                // service(sock,clientip,clientport);

                // v2版本 - 多进程版本
                // pid_t id = fork();
                // if(id < 0)
                // {
                //     close(sock);
                //     continue;
                // }
                // else if(id == 0) 
                // {
                //     // 子进程，父进程的fd被子进程继承，使用的不是同一张文件描述符表
                //     // 子进程拷贝继承父进程的fd table;
                //     // 子进程只进行业务处理，父进程进行监听。
                //     // 建议子进程关闭掉不需要的fd（监听）
                //     close(listensock_);
                //     if(fork() > 0)
                //         exit(0);
                //     // 走到这里子进程已经退了，孙子进程在运行 -- 孤儿进程提供服务
                //     service(sock, clientip, clientport);
                //     exit(0);
                // }
                // // 父进程一定要关闭sock fd，fd的数量是有限的，导致fd泄露
                // close(sock);
                // pid_t ret = waitpid(id,nullptr,0); // 阻塞的
                // // waitpid(id,nullptr,WNOHANG); // 非阻塞 -- 不推荐
                // if(ret == id)
                // {
                //     std::cout << "wait child " << id << " success" << std::endl;
                // }

                // v3: 多线程 -- 原生多线程
                // 1. 要不要关闭不要的socket？？不能
                // 2. 要不要回收线程？如何回收？会不会阻塞？？
                pthread_t tid;
                ThreadData *td = new ThreadData(sock, clientip, clientport, this);
                pthread_create(&tid, nullptr, threadRoutine, td);
            }
        }

        static void* threadRoutine(void *args)
        {
            pthread_detach(pthread_self());

            ThreadData* td = static_cast<ThreadData*>(args);
            td->current->service(td->sock, td->clientip, td->clientport);
            delete td;
            return nullptr;
        }

        void service(int sock,const std::string& clientip,const uint16_t& clientport)
        {
            std::string who = clientip + "-" + std::to_string(clientport);
            char buffer[1024];
            while(true)
            {
                ssize_t s = read(sock,buffer,sizeof(buffer)-1);
                if(s > 0)
                {
                    buffer[s] = 0;
                    std::string res = func_(buffer); // 进行回调
                    std::cout << who << " >>> "<< res << std::endl;

                    write(sock,res.c_str(),res.size());
                }
                else if(s == 0)
                {
                    // 对方将连接关闭了
                    close(sock);
                    std::cout << who << " quit, me too" << std::endl;
                    break;
                }
                else
                {
                    close(sock);
                    std::cerr << "read error: " << strerror(errno) << std::endl;
                    break;
                }
            }
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
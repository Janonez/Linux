#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <pthread.h>
#include <unordered_map>
#include "err.hpp"
#include "RingQueue.hpp"
#include "lockGuard.hpp"
#include "Thread.hpp"

namespace ns_server
{
    const static uint16_t default_port = 8080;
    using func_t = std::function<std::string(std::string)>; // 定义函数
    class UdpServer
    {
    public:
        // UdpServer(uint16_t port = default_port)
        //     : port_(port)
        // {
        //     std::cout << "server port: " << port_ << std::endl;
        // }
        // UdpServer(func_t cb, uint16_t port = default_port)
        //     : service_(cb)
        //     , port_(port)
        // {
        //     std::cout << "server port: " << port_ << std::endl;
        //     pthread_mutex_init(&lock,nullptr);

        //     p = new Thread(1,std::bind(&UdpServer::Recv,this));
        //     c = new Thread(2,std::bind(&UdpServer::Broadcast,this));
        // }
        UdpServer(uint16_t port = default_port)
            : port_(port)
        {
            std::cout << "server port: " << port_ << std::endl;
            pthread_mutex_init(&lock, nullptr);

            p = new Thread(1, std::bind(&UdpServer::Recv, this));
            c = new Thread(1, std::bind(&UdpServer::Broadcast, this));
        }

        void start()
        {
            // 1.创建socket接口，打开网络文件
            // socket 套接字 - create an endpoint for communication
            // int socket(int domain, int type, int protocol)
            // domain （AF_INET）网络通信还是本地通信
            // return a file descriptor 错误返回-1
            sock_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_ < 0) // 创建套接字失败
            {
                std::cerr << "crete socket error: " << strerror(errno) << std::endl;
                exit(SOCKET_ERR);
            }
            // 文件描述符012对应stdin、stdout、stderr 所以从3开始
            std::cout << "create socket success: " << sock_ << std::endl;

            // 2.给服务器指明IP地址和端口号Port
            struct sockaddr_in local; // 在用户空间的函数栈帧上定义，不在内核中
            bzero(&local, sizeof(local));

            local.sin_family = AF_INET; // 又叫PF_INET
            // 端口号必须以报文的形式发送到网络中，需要将主机序列转换成网络序列（大端）// htons
            local.sin_port = htons(port_);
            // inet_addr(),将这两件事都做了
            // 1.字符串风格的ip地址，转换为4字节int "1.1.1.1" -> uint32_t 不可以强制转换类型
            // 2.需要将主机序列转换成网络序列
            // 3.云服务器，或一款服务器，一般不要指定某一个确定的IP
            local.sin_addr.s_addr = INADDR_ANY; // 让我们的udp_server在启动的时候，bind本主机上的任意IP

            // bind 绑定 - bind a name to a socket （给套接字绑定一个名字）
            // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
            // sockfd 套接字
            // struct sockaddr *addr
            // addrlen 地址长度
            // return 成功0 失败-1
            int n = bind(sock_, (struct sockaddr *)&local, sizeof(local));
            if (n < 0)
            {
                std::cerr << "bind socket error: " << strerror(errno) << std::endl;
                exit(BIND_ERR);
            }
            std::cout << "bind socket success: " << sock_ << std::endl; // 3

            p->run();
            c->run();
        }

        void addUser(const std::string &name, const struct sockaddr_in &peer)
        {
            // onlineuser[name] = peer;

            LockGuard lockguard(&lock);
            auto iter = onlineuser.find(name);
            if (iter != onlineuser.end())
                return;
            onlineuser.insert(std::pair<const std::string, const struct sockaddr_in>(name, peer));
        }

        void Recv()
        {
            char buffer[1024];
            while (true)
            {
                // 接收
                // ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags,
                //                  struct sockaddr* src_addr,socklen_t* addrlen);
                // sockfd - 套接字， buf - 缓冲区, len - 缓冲区长度， flag - 读取方式， return - 实际读到的字节
                // struct sockaddr* src_addr,socklen_t* addrlen -- 输入输出型参数
                // socket：ip+port (客户端的ip和port)
                struct sockaddr_in peer; // peer 远端
                socklen_t len = sizeof(peer);
                int n = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
                if (n > 0)
                    buffer[n] = '\0';
                else
                    continue;

                // 提取client信息 -- debug
                std::string clientip = inet_ntoa(peer.sin_addr); // 将四字节ip转换为字符串
                uint16_t clientport = ntohs(peer.sin_port);
                std::cout << clientip << "-" << clientport << "#" << buffer << std::endl;

                // 构建一个用户，并检查
                std::string name = clientip;
                name += "-";
                name += std::to_string(clientport);

                // 业务处理
                // std::string response = service_(buffer);
                // std::string message = service_(buffer);
                // 如果不存在就插入
                addUser(name, peer);
                std::string message = name + " :-> " + buffer;
                rq.push(message); // 生产消息

                // 发送
                // ssize_t sendio(int sockfd, const void* buf, size_t len,int flags,
                //                const struct sockaddr* dest_addr, socklen_t addrlen);
                // sendto(sock_, buffer, sizeof(buffer), 0, (struct sockaddr*)&peer, sizeof(peer));
                // sendto(sock_, response.c_str(), response.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
                // sendto(sock_, message.c_str(), message.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
            }
        }

        void Broadcast() // 广播(发送)
        {
            while (true)
            {

                std::string sendstring;
                rq.pop(&sendstring);

                std::vector<struct sockaddr_in> v;
                {
                    LockGuard lockguard(&lock);
                    for (auto user : onlineuser)
                    {
                        v.push_back(user.second);
                    }
                }
                for (auto user : v)
                {
                    sendto(sock_, sendstring.c_str(), sendstring.size(), 0, (struct sockaddr *)&user, sizeof(user));
                }
            }
        }
        ~UdpServer()
        {
            pthread_mutex_destroy(&lock);
            c->join();
            p->join();

            delete c;
            delete p;
        }

    private:
        int sock_;
        uint16_t port_;
        // std::string ip_; 不需要自己设置，系统自动分配
        func_t service_; // 网络服务器刚刚解决的是IO问题，现在进行业务处理
        std::unordered_map<std::string, struct sockaddr_in> onlineuser;
        pthread_mutex_t lock;
        RingQueue<std::string> rq;
        Thread *c;
        Thread *p;
    };
} // end ns_server
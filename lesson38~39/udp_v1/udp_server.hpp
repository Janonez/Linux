#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

namespace ns_server
{
    enum{
        SOCKET_ERR = 1,
        BIND_ERR
    };

    const static uint16_t default_port = 8080;

    class UdpServer
    {
    public:
        UdpServer(std::string ip, uint16_t port = default_port)
            : ip_(ip)
            , port_(port)
        {
            std::cout << "server addr: " << ip_ << ": " << port_ << std::endl;
        }

        void InitServer()
        {   
            // 1.创建socket接口，打开网络文件
            // socket 套接字 - create an endpoint for communication
            // int socket(int domain, int type, int protocol)
            // domain （AF_INET）网络通信还是本地通信
            // return a file descriptor 错误返回-1
            sock_ = socket(AF_INET,SOCK_DGRAM,0);
            if(sock_ < 0)// 创建套接字失败
            {
                std::cerr << "crete socket error: " << strerror(errno) << std::endl;
                exit(SOCKET_ERR);
            }
            // 文件描述符012对应stdin、stdout、stderr 所以从3开始
            std::cout << "create socket success: " << sock_ << std::endl;

            // 2.给服务器指明IP地址和端口号Port
            struct sockaddr_in local;// 在用户空间的函数栈帧上定义，不在内核中
            bzero(&local,sizeof(local));
            
            local.sin_family = AF_INET; // 又叫PF_INET
            // 端口号必须以报文的形式发送到网络中，需要将主机序列转换成网络序列（大端）// htons
            local.sin_port = htons(port_);
            // inet_addr(),将这两件事都做了
            // 1.字符串风格的ip地址，转换为4字节int "1.1.1.1" -> uint32_t 不可以强制转换类型
            // 2.需要将主机序列转换成网络序列
            local.sin_addr.s_addr = inet_addr(ip_.c_str());
            
            // bind 绑定 - bind a name to a socket （给套接字绑定一个名字）
            // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
            // sockfd 套接字
            // struct sockaddr *addr
            // addrlen 地址长度
            // return 成功0 失败-1
            int n = bind(sock_, (struct sockaddr*)&local, sizeof(local));
            if(n < 0)
            {
                std::cerr << "bind socket error: " << strerror(errno) << std::endl;
                exit(BIND_ERR);
            }
            std::cout << "bind socket success: " << sock_ << std::endl; // 3
        }

        void Start()
        {

        }

        ~UdpServer()
        {
        }

    private:
        int sock_;
        uint16_t port_;
        std::string ip_;
    };
} // end ns_server
#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <ctime>
#include <assert.h>
#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"
#include "Epoller.hpp"
#include "Util.hpp"
#include "Protocol.hpp"

using namespace protocol_ns;
const static int gport = 8888;
const static int bsize = 1024;
const static int linktimeout = 30;

class Connection;
class EpollServer;

using func_t = std::function<const Response(const protocol_ns::Request &)>;
using callback_t = std::function<void(Connection *)>;

class Connection
{
public:
    Connection(const int &fd, const std::string &clientip, const uint16_t &clientport)
        : fd_(fd), clientip_(clientip), clientport_(clientport)
    {
    }
    void Register(callback_t reciver, callback_t sender, callback_t excepter)
    {
        reciver_ = reciver;
        sender_ = sender;
        excepter_ = excepter;
    }
    ~Connection()
    {
    }

public:
    // IO信息
    int fd_;
    std::string inbuffer_;
    std::string outbuffer_;

    // IO处理
    callback_t reciver_;
    callback_t sender_;
    callback_t excepter_;
    // 用户信息
    std::string clientip_;
    uint16_t clientport_;

    uint32_t events_;

    // 回指指针
    EpollServer *R_;

    time_t lasttime_;
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
        AddConnection(listen_sock_.Fd(), EPOLLIN | EPOLLET);
        logMessage(Debug, "init server success");
    }

    int Dispatch()
    {
        int timeout = -1;
        while (true)
        {
            LoopOnce(timeout);
            checklink();
        }
    } // 事件派发器
    void LoopOnce(int timeout)
    {
        int n = epoller_.Wait(revs_, gnum, timeout);

        for (int i = 0; i < n; ++i)
        {

            int fd = revs_[i].data.fd;
            uint32_t events = revs_[i].events;
            logMessage(Debug, "当前正在处理%d上的%s事件", fd, (events & EPOLLIN) ? "EPOLLIN" : "OTHER");
            // 我们把所有的异常转化为读写异常
            if ((events & EPOLLERR) || (events & EPOLLHUP))
            {
                events |= (EPOLLIN | EPOLLOUT);
            } // 错误
            if ((events & EPOLLIN) && (ConnIsExists(fd)))
            {
                connections_[fd]->reciver_(connections_[fd]);
            } // 读
            if (events & EPOLLOUT && (ConnIsExists(fd)))
            {
                connections_[fd]->sender_(connections_[fd]);
            } // 写
        }
    }
    void AddConnection(int fd, uint32_t events, std::string ip = "127.0.0.1", uint16_t port = gport)
    {
        // 1. 将fd设置为非阻塞
        if (events & EPOLLET)
        {
            Util::SetNonBlock(fd);
        }
        // 1. 创建connection对象, 交给connections_管理
        Connection *conn = new Connection(fd, ip, port);
        if (fd == listen_sock_.Fd())
        {
            // 这里Accepter是类内函数，存在一个隐含的this参数
            conn->Register(std::bind(&EpollServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
        }
        else
        {
            conn->Register(std::bind(&EpollServer::Reciver, this, std::placeholders::_1),
                           std::bind(&EpollServer::Sender, this, std::placeholders::_1),
                           std::bind(&EpollServer::Excepter, this, std::placeholders::_1));
        }
        conn->events_ = events;
        conn->R_ = this;
        conn->lasttime_ = time(nullptr);
        connections_.insert(std::pair<int, Connection *>(fd, conn));
        // 2. 将fd添加到epoll中
        bool r = epoller_.AddModEvent(fd, events, EPOLL_CTL_ADD);
        assert(r);
        (void)r;

        logMessage(Debug, "AddConnection success, fd: %d, clientinfo[%s:%d]", fd, ip.c_str(), port);
    }
    bool EnableReadWrite(Connection *conn, bool readable, bool writeable)
    {
        conn->events_ = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        return epoller_.AddModEvent(conn->fd_, conn->events_, EPOLL_CTL_MOD);
    }
    void Accepter(Connection *conn)
    {
        do
        {
            int err = 0;
            // logMessage(Debug, "get a new link ...");
            std::string clientip;
            uint16_t clientport;
            int sock = listen_sock_.Accept(&clientip, &clientport, &err);
            if (sock > 0)
            {
                logMessage(Debug, "%s:%d - 成功连接服务器\n", clientip.c_str(), clientport);
                AddConnection(sock, EPOLLIN | EPOLLET, clientip, clientport);
            }
            else
            {
                if (err == EAGAIN || err == EWOULDBLOCK)
                {
                    break;
                } // 数据读完
                else if (err == EINTR)
                {
                    continue;
                } // 还有数据
                else
                {
                    logMessage(Warning, "err string: %s, err coede: %d\n", strerror(err), err);
                    continue;
                } // 读取失败
            }

        } while (conn->events_ & EPOLLET);

        logMessage(Debug, "Accepter done ...");

    } // 连接管理器

    void HandlerReuest(Connection *conn)
    {
        bool quit = false;
        while (!quit)
        {
            // 根据协议进行数据分析
            std::string requestStr;
            // 1. 提取完整报文
            int n = protocol_ns::ParsePackage(conn->inbuffer_, &requestStr);
            if (n > 0)
            {
                // 2. 提取有效载荷
                requestStr = protocol_ns::RemoveHeader(requestStr, n);
                // 3. 进行反序列化
                protocol_ns::Request req;
                req.DeSerialize(requestStr);
                // 4. 业务处理
                Response resp = func_(req);
                // 5. 序列化
                std::string RespStr;
                resp.Serialize(&RespStr);
                // 6. 添加报头
                RespStr = AddHeader(RespStr);
                // 7. 进行返回
                conn->outbuffer_ += RespStr;
            }
            else
            {
                quit = true;
            }
        }
    }
    bool ReciverHelper(Connection *conn)
    {
        bool ret = true;
        conn->lasttime_ = time(nullptr);// 更新conn最近访问时间
        do
        {
            char buffer[bsize];
            ssize_t n = recv(conn->fd_, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                conn->inbuffer_ += buffer;

                // logMessage(Debug, "inbuffer: %s,[%d]", conn->inbuffer_.c_str(), conn->fd_);
            }
            else if (n == 0)
            {
                conn->excepter_(conn);
                ret = false;
                break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    conn->excepter_(conn);
                    ret = false;

                    break;
                }
            }

        } while (conn->events_ & EPOLLET);
        return ret;
    }
    void Reciver(Connection *conn)
    {
        if(!ReciverHelper(conn))
        {
            return;
        }
        HandlerReuest(conn);
        if (!conn->outbuffer_.empty())
        {
            conn->sender_(conn);
        }
    }
    void Sender(Connection *conn)
    {
        bool safe = true;
        do
        {
            ssize_t n = send(conn->fd_, conn->outbuffer_.c_str(), conn->outbuffer_.size(), 0);
            if (n > 0)
            {
                conn->outbuffer_.erase(0, n); // 清空数据
                if (conn->outbuffer_.empty())
                {
                    break;
                }
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) // 缓冲区满
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    safe = false;
                    conn->excepter_(conn);
                    break;
                }
            }
        } while (conn->events_ & EPOLLET);
        if (!safe)
        {
            return;
        }
        if (!conn->outbuffer_.empty())
        {
            EnableReadWrite(conn, true, true);
        }
        else
        {
            EnableReadWrite(conn, true, false);
        }
    }
    void Excepter(Connection *conn)
    {
        // 1. 从epoll中移除fd
        epoller_.DelEvent(conn->fd_);
        // 2. 移除unordered_map KV关系
        connections_.erase(conn->fd_);
        // 3. 关闭fd
        close(conn->fd_);
        // 4. 释放conn对象
        delete conn;
        logMessage(Debug, "Excepter done ..., fd: %d, clientinfo[%s:%d]", conn->fd_, conn->clientip_.c_str(), conn->clientport_);

    }
    bool ConnIsExists(int fd)
    {
        return connections_.find(fd) != connections_.end();
    }
    void checklink()
    {
        time_t cur = time(nullptr);
        for(auto &connection : connections_)
        {
            if(connection.second->lasttime_ + linktimeout > cur)
            {
                continue;
            }
            else
            {
                Excepter(connection.second);
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

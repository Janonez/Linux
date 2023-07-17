#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <functional>

using cb_t = std::function<void(int , const std::string&, const uint16_t&)>;

class Task
{
public:
    Task()
    {
    }
    Task(int sock, const std::string &ip, const uint16_t &port, cb_t cb)
        : _sock(sock)
        , _ip(ip)
        , _port(port)
        , _cb(cb)
    {}
    void operator()()
    {
        _cb(_sock, _ip, _port);
    }
    ~Task()
    {
    }

private:
    int _sock;
    std::string _ip;
    uint16_t _port;
    cb_t _cb;
};
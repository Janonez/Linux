#include <iostream>
#include <memory>
#include "EpollServer.hpp"

protocol_ns::Response Calculate(const protocol_ns::Request& req)
{
    // 走到这里一定保证req是有具体数据的
    protocol_ns::Response resp(0,0);
    switch(req._op)
    {
        case '+': 
            resp._result = req._x + req._y;
            break;
        case '-': 
            resp._result = req._x - req._y;
            break;
        case '*': 
            resp._result = req._x * req._y;
            break;
        case '/': 
            if(req._y == 0) 
                resp._code = 1;
            else
                resp._result = req._x / req._y;
            break;
        case '%': 
            if(req._y == 0) 
                resp._code = 2;
            else
                resp._result = req._x % req._y;
            break;

        default:
            resp._code = 3;
            break;
    }

    return resp;
}

int main()
{
    std::unique_ptr<EpollServer> svr(new EpollServer(Calculate));
    svr->InitServer();
    svr->Dispatch();
    return 0;
}
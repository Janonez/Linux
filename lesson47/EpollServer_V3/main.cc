#include <iostream>
#include <memory>
#include "EpollServer.hpp"

protocol_ns::Response CalculateHelper(const protocol_ns::Request& req)
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
void Calculate(Connection *conn, const protocol_ns::Request &req)
{
    protocol_ns::Response resp = CalculateHelper(req);
    std::string sendStr;
    resp.Serialize(&sendStr);
    sendStr = protocol_ns::AddHeader(sendStr);
    // epoll中，关于fd的读取，一般需要常设置，一直让epoll关心
    // 而对于fd的写入，一般是按需设置，只有发送的时候才设置
    conn->outbuffer_ += sendStr;
    // 打开write
    conn->R->EnableReadWrite(conn, true, true);
}

int main()
{
    std::unique_ptr<EpollServer> svr(new EpollServer(Calculate));
    svr->InitServer();
    svr->Dispatch();
    return 0;
}
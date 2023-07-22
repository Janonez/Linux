#include "HttpServer.hpp"
#include <memory>

std::string HandlerHttp(const std::string& request)
{
    // 走到这里，reuquest一定是一个完整的http请求
    // 给别人返回的是一个http response
    return nullptr;
}

int main()
{
    uint16_t port = 8888;
    std::unique_ptr<HttpServer> tsvr(new HttpServer(HandlerHttp, port));

    tsvr->InitServer();
    tsvr->Start();
    return 0;
}
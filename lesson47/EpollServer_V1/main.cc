#include <iostream>
#include <memory>
#include "EpollServer.hpp"

std::string EchoServer(std::string r)
{
    std::string resp = r;
    resp += "[echo]\r\n";

    return resp;
}

int main()
{
    std::unique_ptr<EpollServer> svr(new EpollServer(EchoServer));
    svr->InitServer();
    svr->Start();
    return 0;
}
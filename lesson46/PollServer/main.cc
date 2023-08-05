#include <iostream>
#include <memory>
#include "PollServer.hpp"

int main()
{
    // fd_set fd;
    // std::cout << sizeof(fd) * 8 << std::endl;

    std::unique_ptr<PollServer> svr (new PollServer());
    svr->InitServer();
    svr->Start();
    return 0;
}
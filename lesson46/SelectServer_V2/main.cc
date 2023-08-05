#include <iostream>
#include <memory>
#include "SelectServer.hpp"

int main()
{
    // fd_set fd;
    // std::cout << sizeof(fd) * 8 << std::endl;

    std::unique_ptr<SelectServer> svr (new SelectServer());
    svr->InitServer();
    svr->Start();
    return 0;
}
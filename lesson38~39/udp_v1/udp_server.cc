#include "udp_server.hpp"
#include <memory>

using namespace ns_server;
using namespace std;



int main()
{
    //std::cout << "hello server" << std::endl;
    unique_ptr<UdpServer> usvr(new UdpServer("172.19.54.175", 8082));

    usvr->InitServer(); // 服务器的初始化
    usvr->Start(); // 服务器的启动
    return 0;
}
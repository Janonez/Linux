#include "tcp_server.hpp"
#include <memory>

using namespace std;
using namespace ns_server;

static void usage(string proc)
{
    std::cout << "Usage:\n\t" << proc << " port\n"
              << std::endl;
}

std::string echo(const std::string& message)
{
    return message;
}
// ./tcp_server port
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<TcpServer> tsvr(new TcpServer(echo,port));

    tsvr->initServer();
    tsvr->start();
    return 0;
}
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "err.hpp"

using namespace std;

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << std::endl;
}

// ./tpc_client serverip serverport
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t serverport = atoi(argv[2]);
    std::string serverip = argv[1];

    // 1.create socket
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        cerr << "socket error : " << strerror(errno) << endl;
        exit(SOCKET_ERR);
    }

    // 不需要自己bind
    // 不要listen 不要accept

    // 2.connect
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    inet_aton(serverip.c_str(),&server.sin_addr);
    server.sin_port = htons(serverport);

    int cnt = 5;
    while(connect(sock,(struct sockaddr*)&server,sizeof(server)) != 0)
    {
        sleep(1);
        std::cout << "正在尝试重连，重连次数还有：" << cnt-- << std::endl;
        if(cnt <= 0)
            break;
    }
    if(cnt <= 0)
    {
        
        std::cerr<<"连接失败..." << std::endl;
        exit(CONNECT_ERR);
    }

    char buffer[1024];
    // 3. 连接成功
    while(true)
    {
        std::string line;
        std::cout <<"Enter >> ";
        getline(cin,line);

        write(sock,line.c_str(),line.size());

        ssize_t s = read(sock,buffer,sizeof(buffer)-1);
        if(s > 0)
        {
            buffer[s] = 0;
            std::cout << "server echo >>> " << buffer <<std::endl;
        }
        else if(s == 0)
        {
            cerr << "server quit" << endl;
            break;
        }
        else 
        {
            cerr << "read error: " << strerror(errno) << endl;
            break;
        }
    }
    close(sock);
    return 0;
}
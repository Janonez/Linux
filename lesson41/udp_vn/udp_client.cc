#include "udp_client.hpp"

// 127.0.0.1 ：本地环回，就表示的就是当前主机，通常用来进行本地通信或者测试

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n" << std::endl;
}

void* recver(void* args)
{
    int sock = *static_cast<int*>(args);
    while(true)
    {
        // 接收
        char buffer[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        int n = recvfrom(sock,buffer, sizeof(buffer)-1,0,(struct sockaddr*)&temp, &len);
        if(n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer << std::endl;// 1号文件描述符
        }
    }
}
// ./udp_client serverip serverport
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(SOCKET_ERR);
    }
    // client 这里要bind！socket通信的本质[clientip:clientport, serverip:serverport]
    // 但不需要自己bind，也不要自己bind，OS自动给我们进行bind -- client的port要随机让OS分配防止client出现启动冲突
    // server自己band：服务器端口不能随意改动，端口号需要统一规范化

    // 明确服务器是谁 -- 创建struct sockaddr_in
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverip.c_str());
    server.sin_port = htons(serverport);

    pthread_t tid;
    pthread_create(&tid,nullptr,recver,&sock);


    while(true)
    {
        // 用户输入
        std::string message;
        std::cerr << "please enter your message# ";// 2号文件描述符
        std::getline(std::cin,message);

        // 在我们首次系统调用发送数据的时候，OS会在底层随机选择clientport+自己的IP,
        // 1. bind 2. 构建发送的数据报文
        // 发送
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));

    }

    close(sock);
    return 0;
}
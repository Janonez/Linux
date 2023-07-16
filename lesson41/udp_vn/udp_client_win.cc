#include <iostream>
#include <string>
#include <WinSock2.h>

#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

uint16_t serverport = 8888;
std::string serverip = "39.104.83.110";

int main()
{
    WSADATA WSAData;
    // 检查版本是否一致
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
    {
        std::cerr << "init error" << std::endl;
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "create socket error" << std::endl;
        return -2;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (true)
    {
        // 用户输入
        std::string message;
        std::cout << "Please Enter Your Message# ";
        std::getline(std::cin, message);

        // 发送
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));

        // 接受
        char buffer[2048];
        struct sockaddr_in temp;
        int len = sizeof(temp);
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&temp, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer << std::endl; 
        }
    }

    closesocket(sock);

    WSACleanup();
	return 0;
}
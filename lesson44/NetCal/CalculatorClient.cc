#include "TcpClient.hpp"
#include "Sock.hpp"
#include "Protocol.hpp"

#include <iostream>
#include <string>

using namespace protocol_ns;

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << std::endl;
}

enum
{
    LEFT,
    OPER,
    RIGHT
};

Request ParseLine(const std::string& line)
{
    std::string left, right;
    char op;
    int status = LEFT;
    int i = 0;
    while (i < line.size())
    {
        switch (status)
        {
        case LEFT:
            if (isdigit(line[i]))
                left.push_back(line[i++]);
            else
                status = OPER;
            break;

        case OPER:
            op = line[i++];
            status = RIGHT;
            break;

        case RIGHT:
            if (isdigit(line[i]))
                right.push_back(line[i++]);
            break;
        }
    }
    Request req;
    std::cout << "left: " << left << std::endl;
    std::cout << "right: " << right << std::endl;
    std::cout << "op: " << op << std::endl;
    req._x = std::stoi(left);
    req._op = op;
    req._y = std::stoi(right);

    return req;
}

// ./tcpclient serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    std::string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    Sock sock;
    sock.Socket();

    int n = sock.Connect(serverip, serverport);
    if (n != 0)
        return 1;

    std::string buffer;
    while (true)
    {
        std::cout << "Please Enter# ";
        std::string line;
        std::getline(std::cin, line);

        Request req = ParseLine(line);
        // std::cout << "data1# ";
        // std::cin >> req._x;
        // std::cout << "data2# ";
        // std::cin >> req._y;
        // std::cout << "op# ";
        // std::cin >> req._op;

        std::cout << "test: " << req._x << req._op << req._y << std::endl;

        // 1. 序列化
        std::string sendString;
        req.Serialize(&sendString);
        // 2. 添加报头
        sendString = AddHeader(sendString);
        // 3. 发送
        send(sock.Fd(), sendString.c_str(), sendString.size(), 0);

        // 4. 获取响应
        std::string package;
        int n = 0;
    START:
        n = ReadPackage(sock.Fd(), buffer, &package); // n==有效载荷长度
        if (n == 0)
            goto START;
        else if (n < 0)
            break;
        else
        {
        }

        // 5. 去掉报头
        package = RemoveHeader(package, n);

        // 6. 反序列化
        Response resp;
        resp.DeSerialize(package);

        std::cout << "result: " << resp._result << "[code: " << resp._code << "]" << std::endl;
    }

    sock.Close();
    return 0;
}
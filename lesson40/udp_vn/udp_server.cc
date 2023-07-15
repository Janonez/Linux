#include "udp_server.hpp"

#include <memory>
#include <string>
#include <cstdio>
using namespace ns_server;
using namespace std;

// 编写使用手册
static void usage(string proc)
{
    std::cout << "Usage:\n\t" << proc << " port\n"
              << std::endl;
}

// 上层的业务处理，不关心网络发送，只负责信息处理即可
std::string transactionString(std::string request)
{
    std::string result;
    char c;
    for (auto &r : request)
    {
        if (islower(r))
        {
            c = toupper(r);
            result.push_back(c);
        }
        else
            result.push_back(r);
    }
    return result;
}

static bool isPass(const std::string &command)
{   
    bool pass = true;
    auto pos = command.find("rm");
    if(pos != std::string::npos) pass=false;
    pos = command.find("mv");
    if(pos != std::string::npos) pass=false;
    pos = command.find("while");
    if(pos != std::string::npos) pass=false;
    pos = command.find("kill");
    if(pos != std::string::npos) pass=false;
    return pass;
}

// 在你的本地把命令给我，server再把结果给你！
// ls -a -l
std::string excuteCommand(std::string command) // command就是一个命名
{
    // 1. 安全检查
    if(!isPass(command)) 
        return "you are bad man!";

    // 2. 业务逻辑处理
    FILE *fp = popen(command.c_str(), "r");
    if(fp == nullptr) 
        return "None";
    // 3. 获取结果
    char line[1024];
    std::string result;
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        result += line;
    }
    pclose(fp);

    return result;
}

// 云服务器 不需要自己bind ip地址，需要让服务器自己指定IP地址
// ./udp_sercer port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]); // 端口号，字符串转换为整型

    // std::cout << "hello server" << std::endl;
    //unique_ptr<UdpServer> usvr(new UdpServer(port));
    //unique_ptr<UdpServer> usvr(new UdpServer(transactionString, port));
    // unique_ptr<UdpServer> usvr(new UdpServer(excuteCommand, port));
    unique_ptr<UdpServer> usvr(new UdpServer(port));
    

    //usvr->InitServer(); // 服务器的初始化
    usvr->start();      // 服务器的启动
    return 0;
}
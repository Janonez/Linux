#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <vector>
#include <functional>

void PrintLog()
{
    std::cout << "这是一个打印日志的例程" << std::endl;
}
void OperMySQL()
{
    std::cout << "这是一个操作数据库的例程" << std::endl;
}
void CheckNet()
{
    std::cout << "这是一个检测网络的例程" << std::endl;
}

using func_t = std::function<void (void)>;
std::vector<func_t> funcs;

void LoadTask()
{
    funcs.push_back(PrintLog);
    funcs.push_back(OperMySQL);
    funcs.push_back(CheckNet);
}

void SetNonBlock(int fd)
{
    int f1 = fcntl(fd,F_GETFL);
    if(f1 < 0)
    {
        std::cerr << "error string" <<strerror(errno) <<"error code: " << errno << std::endl;
    }
    fcntl(fd, F_SETFL, f1 | O_NONBLOCK);
}

void HandlerAllTask()
{
    for(const auto& func : funcs)
    {
        func();
    }
}

int main()
{
    
    char buffer[128];
    SetNonBlock(0);
    LoadTask();
    while(true)
    {
        printf(">> ");
        fflush(stdout);
        ssize_t n = read(0, buffer, sizeof(buffer)-1);// 阻塞在这里，等+拷贝
        // 1. 读取成功
        if(n > 0)
        {
            buffer[n-1] = 0;
            std::cout << "echo # " << buffer << std::endl;
        }
        // 2. 读取结束
        else if(n == 0)
        {
            std::cout << "end file" << std::endl;
            break;
        }
        // 3. 读取失败，一旦设置fd为非阻塞，底层没有数据就绪，就以出错返回，但是不算真正的出错
        else
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                
                // 底层没有数据，再次读取
                sleep(1);
                HandlerAllTask();
                std::cout << "data not ready" << std::endl;
                continue;
            }
            else if(errno == EINTR)
            {
                // IO被信号中断，重新读取
                continue;
            }
            else
            {
                std::cerr << "read error - " << "error string: " <<strerror(errno) <<"error code: " << errno << std::endl;
                break;
            }
            
            
        }
    }
    return 0;
}
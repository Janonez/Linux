#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unistd.h>
#include "Task.hpp"

using namespace std;

const int gnum = 5;
Task t;

class EndPoint
{
public:
    EndPoint(int id,int fd)
        :_child_id(id)
        ,_write_fd(fd)
    {}
    ~EndPoint()
    {}
public:
    pid_t _child_id;
    int _write_fd;
};

// 子进程要执行的方法
void WaitCommand()
{
    while(1)
    {
        int command;
        int n = read(0,&command,sizeof(int));
        if(n == sizeof(int))
        {
            t.Execute(command);
        }
        else if(n == 0)
        {
            break;
        }
        else
        {
            break;
        }
    } 
}


void CreateProcesses(vector<EndPoint> *end_points)
{
    for(int i = 0;i<gnum;++i)
    {
        // 1.1 建立管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        assert(n == 0);
        (void)n;
        
        // 1.2 创建进程
        pid_t id = fork();
        assert(id != -1);
        // 走到这里，一定是子进程
        if(id == 0)
        {
            // 1.3 关闭不需要的fd
            close(pipefd[1]);
            // 我们期望，所有的子进程读取“指令”的时候，都从标准输入（0）读取
            // 1.3.1 输入重定向
            dup2(pipefd[0],0);
            // 1.3.2 子进程开始等待获取命令
            WaitCommand();
            close(pipefd[0]);
            exit(0);
        } 
        // 走到这里，一定是父进程
        // 1.3 关闭不要的父进程
        close(pipefd[0]);
            
        // 1.4 将新的 子进程 和 父进程的管道写端，构建对象
        end_points->push_back(EndPoint(id,pipefd[1]));

    }
}

int main()
{
    // 1. 先进行构建控制结构,父进程写入，子进程读取
    vector<EndPoint> end_points;
    CreateProcesses(&end_points);
    
    // 2. 我们得到了end_points
    while(1)
    {
        // 1. 选择任务
        int command = COMMAND_LOG;
        // 2. 选择进程
        int index = rand()%end_points.size();
        // 3. 下发任务
        write(end_points[index]._write_fd,&command,sizeof(command));
        sleep(1);
    }
    
    return 0;
}
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "Task.hpp"

using namespace std;

const int gnum = 3;
Task t;

class EndPoint
{
private:
    static int number; // 进程名序号
public:
    EndPoint(int id,int fd)
        :_child_id(id)
        ,_write_fd(fd)
    {
        // process-0[pid-fd]
        char namebuffer[64];
        snprintf(namebuffer,sizeof(namebuffer),"process-%d[%d-%d]",number++,getpid(),_write_fd);
        processname = namebuffer;
    }
    std::string name() const
    {
        return processname;
    }
    ~EndPoint()
    {}
public:
    pid_t _child_id;// 每一个管道对应的子进程pid
    int _write_fd;// 父进程写端的文件描述符 4、5、6、7、8……
    std::string processname;
};
int EndPoint::number = 0;

// 子进程要执行的方法
void WaitCommand()
{
    while(1)
    {
        int command;
        // 从stdin读取数据到command缓冲区，返回读到的字节数
        int n = read(0,&command,sizeof(int));
        // 管道写端对应的文件描述符被关闭，则read返回0
        if(n == sizeof(int))
        {
            t.Execute(command);
        }
        else if(n == 0)
        {
            std::cout << "父进程让我退出，我就退出了: " << getpid() << std::endl;
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
    vector<int> fds;// 实现管道和子进程真真地一一对应
    // 子进程被fork时会拷贝文件描述符表，之前存在的管道写端也被拷贝了下来，
    for(int i = 0;i<gnum;++i)
    {
        // 1.1 建立管道
        int pipefd[2] = {0};
        //pipe() 函数自动以读写的方式打开同一个管道文件并将文件描述符返回给 pipefd[2]
        //return：创建管道成功返回0；失败返回-1
        int n = pipe(pipefd);
        assert(n == 0);
        (void)n;
        
        // 1.2 创建进程
        pid_t id = fork();
        assert(id != -1);

        if(id == 0)// 走到这里，一定是子进程
        {
            // std::cout << getpid() << " 子进程关闭父进程对应的写端：";
            // for(auto &fd : fds) 
            // {
            //     std::cout << fd << " ";
            //     close(fd);
            // }
            // std::cout << std::endl;
            for(auto &fd : fds) close(fd);
            // 1.3 子进程读取，关闭不需要的fd（写入）
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

        fds.push_back(pipefd[1]);
    }
}

int ShowBoard()
{
    std::cout << "##########################################" << std::endl;
    std::cout << "|   0. 执行日志任务   1. 执行数据库任务  |" << std::endl;
    std::cout << "|   2. 执行请求任务   3. 退出            |" << std::endl;
    std::cout << "##########################################" << std::endl;
    std::cout << "请选择# ";
    int command = 0;
    std::cin >> command;
    return command;
}

void ctrlProcess(const vector<EndPoint>& end_points)
{
    int num = 0;
    int cnt = 0;
    while(1)
    {
        // 1. 选择任务
        int command = ShowBoard();
        if(command == 3) break;
        if(command < 0 || command > 2) continue;
        // 2. 选择进程
        int index = cnt++;
        cnt %= end_points.size(); // 循环
        std::cout << "选择了进程：" << end_points[index].name() << " | 处理任务" << command << std::endl;
        
        // 3. 下发任务
        write(end_points[index]._write_fd,&command,sizeof(command));
        sleep(1);
    }
}

 void waitProcess(const vector<EndPoint>& end_points)
 {
    for(const auto &ep : end_points)
    {
        // 1. 我们需要子进程全部退出 -- 只需要父进程关闭写端_write_fd
        // 管道写端对应的文件描述符被关闭，则read返回0
        close(ep._write_fd);
        std::cout << "父进程让子进程退出:"  << std::endl;
        sleep(1);

        // 2. 父进程回收子进程的僵尸状态
        // 僵尸状态：只要子进程退出，父进程还在运行，但父进程没有读取子进程状态，子进程进入Z状态
        waitpid(ep._child_id,nullptr,0);
        std::cout << "父进程回收了所有的子进程" << std::endl;
        sleep(1);
    }
    
    

 }

int main()
{
    vector<EndPoint> end_points;

    // 1. 先进行构建控制结构,父进程写入，子进程读取
    CreateProcesses(&end_points);
    
    // 2. 我们得到了end_points
    ctrlProcess(end_points);
    
    // 3. 处理所有的退出问题
    waitProcess(end_points);
    return 0;
}
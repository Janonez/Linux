#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>

typedef void (*fun_t)();

void PrintLog()
{
    std::cout <<"pid:" << getpid() << ", 打印日志，正在被执行..." << std::endl;
}

void InsertMySQL()
{
    std::cout << "执行数据库，正在被执行..." << std::endl;
}

void NetRequest()
{
    std::cout << "执行网络请求，正在被执行..." << std::endl;
}

#define COMMAND_LOG 0
#define COMMAND_MYSQL 1
#define COMMAND_REQUEST 2


class Task
{
public:
    Task()
    {
        funcs.push_back(PrintLog);
        funcs.push_back(InsertMySQL);
        funcs.push_back(NetRequest);
    }

    void Execute(int command)
    {
        if(command >= 0 && command < funcs.size())
            funcs[command]();
    }

    ~Task()
    {}
public:
    std::vector<fun_t> funcs;
};
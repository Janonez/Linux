#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdarg>
#include <sys/types.h>
#include <unistd.h>

// 日志是有日志等级的

const std::string filename = "log/tcpserver.log";

enum
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Uknown
};

static std::string toLevelString(int level)
{
    switch (level)
    {
    case Debug:
        return "Debug";
    case Info:
        return "Info";
    case Warning:
        return "Warning";
    case Error:
        return "Error";
    case Fatal:
        return "Fatal";
    default:
        return "Uknown";
    }
}

static std::string getTime()
{
    time_t curr = time(nullptr);
    struct tm* tmp = localtime(&curr);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%d-%d-%d %d:%d:%d", tmp->tm_year + 1900, tmp->tm_mon+1, tmp->tm_mday,
             tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    return buffer;
}

// 日志格式 : 日志等级 时间 文件 行  pid 消息体
// logMessage(DEBUG,"hello: %d, %s",12, s.c_str()); // DEBUG hello:12,world
void logMessage(int level,const char* format, ...)
{
    // 预备
    // va_list p;              // char*
    // int a = va_arg(p,int);  // 根据类型提取参数
    // va_start(p,format);     //p指向可变参数部分的起始地址
    // va_end(p);              // p = NULL;

    char logLeft[1024];     // 日志等级 时间 pid 
    std::string level_string = toLevelString(level);
    std::string curr_time = getTime();
    snprintf(logLeft, sizeof(logLeft), "[%s] [%s] [%d] ", level_string.c_str(), curr_time.c_str(), getpid());

    char logRight[1024];    // 消息体
    va_list p;
    va_start(p,format);
    vsnprintf(logRight,sizeof(logRight),format,p);
    va_end(p);

    // 打印
    printf("%s%s\n",logLeft, logRight);

    // 保存到文件中
    // FILE* fp = fopen(filename.c_str(), "a");
    // if(fp == nullptr)
    //     return;
    
    // fprintf(fp,"%s%s\n",logLeft, logRight);
    // fflush(fp);// 可写也可以不写
    // fclose(fp);
}
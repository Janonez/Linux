#ifndef __COMM_HPP__
#define __COMM_HPP__

#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

// IPC_CREAT and IPC_EXCL
// 单独使用IPC_CREAT: 创建一个共享内存，如果内存不存在，就创建，如果存在就返回已创建好的内存
// IPC_EXCL 不能单独使用，一般是配合IPC_CREAT
// IPC_CREAT | IPC_EXCL 创建一个共享内存，如果共享内存不存在，就创建, 如果已经存在，则立马出错返回 -- 如果创建成功，对应的shm，一定是最新的！

#define PATHNAME "."
#define PROJID 0x6666

const int gsize = 4096;

key_t GetKey()
{
    // key_t ftok(const char *pathname, int proj_id);
    // pathname:用户自己设定的路径字符串 proj_id:用户自己设定的项目id
    key_t k = ftok(PATHNAME,PROJID);
    if(k == -1) 
    {
        cerr << "error: " << errno << " : " << strerror(errno) << endl;
        exit(1);
    }
    return k;
}

string ToHex(int x)
{
    char buffer[64];
    snprintf(buffer,sizeof(buffer),"0x%x",x);
    return buffer;
}

static int createShmHelper(key_t k,int size,int flag)
{
    // int shmget(key_t key, size_t size, int shmflg);
    // 成功返回内存，失败返回-1
    int shmid = shmget(k,gsize,flag);
    if(shmid == -1) 
    {
        cerr << "error: " << errno << " : " << strerror(errno) << endl;
        exit(2);
    }
    return shmid;
}

int CreatShm(key_t k,int size)
{
    umask(0);
    return createShmHelper(k,gsize,IPC_CREAT|IPC_EXCL|0666);
}

int GetShm(key_t k,int size)
{
    return createShmHelper(k,gsize,IPC_CREAT);
}

char* AttachShm(int shmid)
{
    //void *shmat(int shmid, const void *shmaddr, int shmflg);
    // shmaddr: 将新开的共享内存挂接到地址空间的地址，让系统选择
    char* start = (char*)shmat(shmid,nullptr,0);
    return start;
}

void DetachShm(char *start)
{
    // int shmdt(const void *shmaddr);
    int n = shmdt(start);
    assert(n != -1);
    (void)n;
}

void DelShm(int shmid)
{
    // int shmctl(int shmid, int cmd, struct shmid_ds *buf);
    // shmid:共享内存的id  cmd:操作方式
    int n = shmctl(shmid,IPC_RMID, nullptr);
    assert(n != -1);
    (void)n;
}

#define SERVER 1
#define CLIENT 0

class Init
{
public:
    Init(int t)
        :type(t)
    {
        key_t k = GetKey();
        if(type == SERVER) shmid = CreatShm(k, gsize);
        else shmid = GetShm(k, gsize);
        start = AttachShm(shmid);
    }
    char *getStart()
    { 
        return start; 
    }
    ~Init()
    {
        DetachShm(start);
        if(type == SERVER) DelShm(shmid);
    }
private:
    char *start;
    int type; //server or client
    int shmid;
};

#endif
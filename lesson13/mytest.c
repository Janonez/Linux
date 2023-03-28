#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int g_value = 100;// 全局变量

int main()
{
  pid_t id = fork();
  assert(id>=0);
  if(id == 0)
  {
    // child
    while(1)
    {
      printf("我是子进程，我的id是：%d，我的父进程id是：%d，g_value：%d，&g_value：%p\n",
          getpid(),getppid(),g_value,&g_value);
      sleep(1);
    }
  }
  else 
  { 
    // parent
    while(1)
    {
      printf("我是父进程，我的id是：%d，我的父进程id是：%d，g_value：%d，&g_value：%p\n",
          getpid(),getppid(),g_value,&g_value);
      sleep(2);
    }
  }
  return 0;
}

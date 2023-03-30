#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//const char *err_string[] = {
//    "success",
//    "error"
//};

int add_to_up(int top)
{
  printf("enter add_to_top");
  int sum = 0;
  for(int i = 0;i<=top;++i)
  {
    sum += i;
  }
  //exit(123);
  _exit(222);
  printf("out add_to_top");
  return sum;
}

#define TASK_NUM 10

// 预设一批任务
void sync_disk()
{
  printf("这是一个刷新数据的任务。\n");
}

void sync_log()
{
  printf("这是一个同步日志的任务。\n");
}

void net_send()
{
  printf("这是一个网络发送的任务。\n");
}

typedef void (*func_t)();
func_t other_task[TASK_NUM] = {NULL};

int LoadTask(func_t func)
{
  int i = 0;
  for(;i<TASK_NUM;++i)
  {
    if(other_task[i] == NULL)
      break;
  }
  if(i == TASK_NUM)
    return -1;
  else
    other_task[i]=func;

  return 0;
}

void InitTask()
{
  for(int i = 0;i<TASK_NUM;++i)
  {
    other_task[i] = NULL;
  }
  LoadTask(sync_disk);
  LoadTask(sync_log);
  LoadTask(net_send);
}

void RunTask()
{
  for(int i = 0;i < TASK_NUM;++i)
  {
    if(other_task[i] == NULL)
    {
      continue;
    }
    other_task[i]();
  }

}
int main()
{
  /*
  printf("hello world"); // 输出缓冲区
  sleep(2);
  _exit(107);
  

  int ret = add_to_up(100);
  //printf("%d",ret);
  if(ret == 5050)
    return 0;
  else 
    return 1;// 计算结果不正确
  
   
  for(int i = 0;i<200;++i)
  {
    printf("%d:%s\n",i,strerror(i));
  }
  */

  pid_t id = fork();
  if( id == 0 )
  {
    //child
    int cnt = 50;
    while(cnt)
    {
      printf("我是子进程,我还活着呢，我还有%ds,pid:%d,ppid:%d\n",cnt--,getpid(),getppid());
      sleep(1);
    }
    exit(23);
  }
  //走到这里一定是父进程
  
  InitTask();

  while(1)
  {
    int status = 0;
    pid_t ret_id = waitpid(id,&status,WNOHANG);
    //pid_t ret_id = wait(NULL);
    if(ret_id < 0)
    {
      printf("waitpid error\n");
      exit(1);
    }
    else if(ret_id == 0)
    {
      RunTask();
      sleep(1);
      continue;
    }
    else
    {
     // printf("我是父进程，等待子进程成功，pid:%d,ppid:%d,ret_id:%d,chile exit code:%d,chile exit signal:%d\n",getpid(),getppid(),ret_id,(status>>8)&0xFF,status & 0x7F);
      if(WIFEXITED(status)) // 是否收到信号
      {
        printf("wait success, child exit code: %d\n", WEXITSTATUS(status));
      }
      else
      {
        printf("wait success, child exit signal: %d\n", status & 0x7F);
      }
      break;
    }
  }
}

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
    int cnt = 5;
    while(cnt)
    {
      printf("我是子进程,我还活着呢，我还有%ds,pid:%d,ppid:%d\n",cnt--,getpid(),getppid());
      sleep(1);
    }
    exit(23);
  }
  //走到这里一定是父进程
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
      sleep(1);
      continue;
    }
    else{

      printf("我是父进程，等待子进程成功，pid:%d,ppid:%d,ret_id:%d,chile exit code:%d,chile exit signal:%d\n",getpid(),getppid(),ret_id,(status>>8)&0xFF,status & 0x7F);
    }
  }
}

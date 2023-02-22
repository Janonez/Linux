#include <stdio.h>
#include <unistd.h>

int main()
{
  

  //int count = 0;
  //while(1)
  //{
    //int a = 0;
    //scanf("%d",&a);
    //printf("%d\n",a);
    //printf("我在运行吗？？%d\n",count++);
    //sleep(1);
  //}
  //int result  = 30;
  //if(result == 10)  return 0;
  //else return 3;
  //
  pid_t id = fork();
  if(id == 0)
  {
    // 子进程
    while(1)
    {
      printf("我是子进程，我在运行，pid：%d，ppid：%d\n",getpid(),getppid());
      sleep(1);
    }
  }
  else if(id > 0)
  {
    // 父进程
    while(1)
    {
      printf("我是父进程，我在运行，pid：%d，ppid：%d\n",getpid(),getppid());
      sleep(1);
    }
  }
  return 0;
}

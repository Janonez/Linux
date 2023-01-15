#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
  int x = 100;
  pid_t ret = fork();
	if(ret == 0)
  {
    // 子进程
    while(1)
    {
      printf("我是子进程,pid:%d,我的父进程是:%d,x:%d,&x:%p\n", getpid(), getppid(),x,&x);
      sleep(1);
    }
	}
	else if(ret > 0)
  {
  	// 父进程
    while(1)
    {
      printf("我是父进程,pid:%d,我的父进程是:%d,x:%d,&x:%p\n", getpid(), getppid(),x,&x);
      x=4321;
      sleep(1);
    }
	}
  else
  {
    perror("fork");
		return 1;
	}
  
//  printf("aaaaaa,pid:%d,ppid:%d\n",getpid(),getppid());
//  pid_t ret = fork();       
//  printf("bbbbbb,pid:%d,ppid:%d,ret:%d,&ret:%p\n",getpid(),getppid(),ret,&ret);
//  sleep(1);      

//  while(1)
//  {
//    printf("hello process，我是一个进程，我的pid是%d，我的父进程是%d\n",getpid(),getppid());
//    sleep(1);
//  }
  return 0;
}


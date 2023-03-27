#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define NAME "Janonez"
// 使用方法提示
void Usage(const char* name)
{
  printf("\nUsage: %s -[a|b|c]\n\n",name);
  exit(0);//终止进程
}
//int main(int argc,char* argv[],char* envp[])
// ./myproc arg
int main(int argc ,char* argv[])
{
    /*
    while(1)
    {
      printf(".\n");
      //fflush(stdout);
      sleep(1);
    }
    */
  
    /*if(argc != 2) Usage(argv[0]);//如果没有使用选项，就调用函数提示

    if(strcmp(argv[1],"-a") == 0)
        printf("打印当前目录下的文件名\n");
    else if(strcmp(argv[1],"-b") == 0)
        printf("打印当前目录下的文件的详细信息\n");
    else if(strcmp(argv[1],"-c") == 0)
        printf("打印当前目录下的文件名（包含隐藏文件）\n");
    else
        printf("其他功能，待开发\n");*/
    
    /*
    printf("argc: %d\n",argc);
    //for(int i = 0;argv[i];++i)
    int i;
    for(i = 0;i<argc;++i)
    {
      printf("argv[%d]:->%s\n",i,argv[i]);
    }
    */

    /*
    printf("myenv:%s\n",getenv("heelo1"));
    char* own = getenv("USER");
    if(strcmp(own,NAME)==0)
    {
      printf("这个程序已经执行啦。。。\n");
    }
    else 
    {
      printf("当前用户:%s是一个非法用户，无法执行\n",own);
    }
    */

    char *pwd = getenv("PWD");
    if(pwd == NULL) perror("getenv");
    else printf("%s\n", pwd);

    char *user = getenv("USER");
    if(user == NULL) perror("getenv");
    else printf("USER: %s\n", user);
    
    /*
    extern char **environ;
    //// environ[i] *(environ+i)
    for(int i = 0; environ[i]; i++)
    {
        printf("environ[%d]->%s\n", i, environ[i]);
    }
    */
    
  // char *envp[]: 指针数组
    /*
    for(int i = 0;envp[i];i++)
    {
      printf("envp[%d]->%s\n",i,envp[i]);
    }
    */

    /*
    int cnt = 5;
    while(cnt)
    {
      printf("hello %d",cnt--);
      sleep(1);
    }
    pid_t id = fork();
    if(id == 0)
    {
      // child
      while(1)
      {
        printf("我是子进程：pid: %d, ppid: %d\n",getpid(),getppid());
        sleep(1);
      }
    }
    else
    {
      // parent
      int cnt = 10;
      while(1)
      {
        printf("我是父进程：pid: %d, ppid: %d\n",getpid(),getppid());
        sleep(1);
        if(cnt-- <= 0)
          break;
      }
    
    }*/
    return 0;
}

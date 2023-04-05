#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 1024
#define ARGC 64
#define SEP " "

int split(char* commandstr,char* argv[])
{
  assert(commandstr);
  assert(argv);

  argv[0]=strtok(commandstr,SEP);
  if(argv[0] == NULL)
    return -1;
  int i = 1;
  while((argv[i++] = strtok(NULL,SEP)));
  /*while(1)
  {
    argv[i]=strtok(NULL,SEP);
    if(argv[i] == NULL)
      break;
    i++;
  }*/
  return 0;
}

void debugPrint(char *argv[])
{
    for(int i = 0; argv[i]; i++)
    {
        printf("%d: %s\n", i, argv[i]);
    }
}

void ShowEnv()
{
  extern char ** environ;
  for(int i = 0;environ[i];++i)
  {
    printf("%d:%s\n",i,environ[i]);
  }
}

int main()
{
  int last_exit = 0;
  char myenv[32][256];
  int env_index = 0;
  
  while(1)
  {
    char commandstr[MAX] = {0};
    char* argv[ARGC]={NULL};
    printf("[Janonez@machine currpath]# ");
    fflush(stdout);
    
    char *s = fgets(commandstr,sizeof(commandstr),stdin);
    assert(s);
    (void)s; //保证在release方式发布的时候，因为去掉assert了，所以s就没有被使用，
    //而带来的编译告警, 什么都没做，但是充当一次使用
    
    // abcd\n\0
    commandstr[strlen(commandstr)-1]='\0';
    //printf("%s\n",commandstr);
    int n = split(commandstr,argv);
    if(n != 0)
      continue;
   
    // cd .. 让bash自己执行命令，被称为内建命令，内置命令
    if(strcmp(argv[0],"cd") == 0)
    {
      if(argv[1] != NULL)
        chdir(argv[1]);

      continue;
    }

    else if(strcmp(argv[0],"export") == 0) // 其实我们之前学习到的所有的(几乎)环境变量命令，都是内建命令
    {
      if(argv[1] != NULL)
      {
        strcpy(myenv[env_index],argv[1]);
        putenv(myenv[env_index++]);
      }
      continue;
    }

    else if(strcmp(argv[0], "env") == 0)
    {
      ShowEnv();
      continue;
    }
    
    else if(strcmp(argv[0], "echo") == 0)
    {
      // echo $PATH
      const char *target_env = NULL;
      if(argv[1][0] == '$') 
      {
        if(argv[1][1] == '?')
        {
          printf("%d\n", last_exit);
          continue;
        }
        else 
        {
          target_env = getenv(argv[1]+1); // "abcdefg
        }
                
        if(target_env != NULL) 
        {
          printf("%s=%s\n", argv[1]+1, target_env);
        }
      }
      continue;
    }

    // version2 -- 不同文件类型增加颜色区分
    if(strcmp(argv[0],"ls") == 0)// 是否为ls命令
    {
      int pos = 0;// 从第一个字符串开始遍历
      while(argv[pos])// 直到找到最后一条命令
        pos++;
      argv[pos++] = (char*)"--color=auto";// 配色
      argv[pos] = NULL;
    }
    
    // version1
    pid_t id = fork();
    assert(id >= 0);
    (void)id;
    
    if(id == 0)
    {
      //child
      execvp(argv[0],argv);
      exit(1);
    }
    int status = 0;
    pid_t ret = waitpid(id,&status,0);
    if(ret > 0)
    {
      last_exit = WEXITSTATUS(status);
    }
  }
  //return 0;
}

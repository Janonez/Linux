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

int main()
{
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
    commandstr[strlen(commandstr)-1]='\0';
    //printf("%s\n",commandstr);
    int n = split(commandstr,argv);
    if(n != 0)
      continue;
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
    waitpid(id,&status,0);
  }
  //return 0;
}

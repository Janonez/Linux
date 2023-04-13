#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
  extern char **environ;
  
  pid_t id = fork();
  if(id == 0)
  {
    // chile process
    printf("我是子进程：%d\n",getpid());
    //----------------------execl----------------------------------------
    //execl:如果替换成功，没有返回值，如果替换失败，一定有返回值
    //不需要判断该函数的返回值，只要继续向后运行就一定是失败的
    //execl("/bin/ls","ls","-a","-l",NULL);
    //execl("./exec/otherproc","otherproc",NULL);
    /*char *const myenv[] = {
          "MYENV=YouCanSeeMe",
          NULL
        };
    execle("./exec/otherproc","otherproc",NULL,myenv);*/
    //execle("./exec/otherproc","otherproc",NULL,environ);
    
    //putenv("MYENV=YouCanSeeMe");
    //execle("./exec/otherproc","otherproc",NULL,environ);

    execl("./exec/shell.sh", "shell.sh", NULL);

    //----------------------execlp--------------------------------------
    //execlp("ls","ls","-a","-l","-n",NULL);
    
    //----------------------execv---------------------------------------
    char *const myargv[] = {
      "-ls",
      "-a",
      "-l",
      "-n",
      NULL
    };
    //execv("/bin/ls",myargv);
    
    //---------------------execvp------------------------------------------
    //execvp("ls",myargv);
    




    //如果走到这里，一定是替换失败
    exit(15);
  }
  sleep(1);
  // father process
  int status = 0;
  printf("我是父进程：%d\n",getpid());
  waitpid(id,&status,0);
  printf("child exit code:%d\n",WEXITSTATUS(status));
  return 0;
}

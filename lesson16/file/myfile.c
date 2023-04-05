#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define LOG "log.txt"

// 系统方案
int main()
{
  umask(0);
  //int fd = open(LOG,O_WRONLY|O_CREAT,0666);
  //int fd = open(LOG,O_WRONLY|O_CREAT|O_TRUNC,0666);
  //int fd = open(LOG,O_WRONLY|O_CREAT|O_APPEND,0666);

  int fd = open(LOG,O_RDONLY);
  if(fd == -1)
  {
    printf("fd:%d,errno:%d,errstring:%s\n",fd,errno,strerror(errno));
  }
  else 
  {
    printf("fd:%d,errno:%d,errstring:%s\n",fd,errno,strerror(errno));
  }
  
  char buffer[1024];
  // 整体读取，无法按行读取
  size_t n = read(fd,buffer,sizeof(buffer)-1);// 去掉字符串结尾的 \0 存在文件中
  if(n>0)
  {
    buffer[n] = '\0';
    printf("%s\n",buffer);
  }
  close(fd);
  
  return 0;
}

// 语言方案
/*
int main()
{
  // w: 默认写方式打开文件，如果文件不存在，就创建它
  // 1. 默认如果只是打开，文件内容会自动被清空
  // 2. 同时，每次进行写入的时候，都会从最开始进行写入
  //FILE* fp = fopen(LOG,"w");

  // a: 不会清空文件，而是每一次写入都是从文件结尾写入的， 追加
  FILE *fp = fopen(LOG, "a");
  //FILE *fp = fopen(LOG, "r");
  if(fp == NULL)
  {
    perror("fopen");
    return 1;
  }


  //const char *msg = "hello world\n";
  const char *msg = "hello world";
  int cnt = 5;
  while(cnt)
  {
    // int fputs(const char *s, FILE *stream);
    // fputs() writes the string s to stream, without its terminating null byte ('\0').
    
    //fputs(msg, fp);
    
    // 写到文件中
    //fprintf(fp, "%s: %d: Janonez\n", msg, cnt);
    
    // 打印到显示器
    //fprintf(stdout, "%s: %d: Janonez\n", msg, cnt); // Linux一切皆文件，stdout也对应一个文件, 显示器文件
    
    
    char buffer[256];// 缓冲区字符串
    snprintf(buffer, sizeof(buffer), "%s:%d:Janonez\n", msg, cnt);
    fputs(buffer, fp);
    
    // int snprintf(char *str, size_t size, const char *format, ...);
    // write at most size bytes (including the terminating null byte ('\0')) to str.

    //fputs(msg, fp);
    cnt--;
  }

  fclose(fp);
  return 0;
}
*/

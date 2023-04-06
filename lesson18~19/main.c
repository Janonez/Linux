#include "mystdio.h"
#include <string.h>
#include <unistd.h>

#define MYFILE "log.txt"

int main()
{
  MY_FILE *fp = my_fopen(MYFILE,"w");
  if(fp == NULL) return 1;

  const char* str = "hello world";
  int cnt = 5;
  while(cnt)
  {
    char buffer[NUM];
    snprintf(buffer,sizeof(buffer),"%s:%d\n",str,cnt--);
    size_t size = my_fwrite(buffer,strlen(buffer),1,fp);
    sleep(1);
    printf("当前成功写入: %lu个字节\n", size);
  }
  
  my_fclose(fp);

  return 0;
}

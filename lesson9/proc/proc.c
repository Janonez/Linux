#include "proc.h"

#define SIZE 102
#define BARSTYLE '='
#define ARR '>'
#define GREEN "\033[1;32m"
#define NONE "\033[m"

void process()
{
  //printf("hello demo process bar\n");
  char bar[SIZE];
  // 初始化字符串为'\0'
  memset(bar,'\0',sizeof(bar));
  const char* lable = "|/-\\";
  int i = 0;
  while(i <= 100)
  {
    printf(GREEN"[%-100s][%d%%][%c]\r" NONE,bar,i,lable[i%4]);
    fflush(stdout);
    bar[i++] = BARSTYLE;
    if(i != 100)
      bar[i] = ARR;
    usleep(10000);

  }
  printf("\n");
}

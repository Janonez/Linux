#include <stdio.h>
#include <unistd.h>

int main()
{
  //printf("hello world\r");
  //fflush(stdout);
  //sleep(3);
  
  // 倒计时程序
  int i = 10;
  for(;i>=0;i--)
  {
    printf("%2d\r",i);
    fflush(stdout);
    sleep(1);
  }
  printf("\n");
  
  return 0;
}

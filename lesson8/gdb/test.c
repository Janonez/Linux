#include <stdio.h>

int addTop(int top)
{

  printf("enter addTop\n");
  int sum = 0;
  for(int i = 1;i<= top;i++)
  {
    sum += i;
  }

  printf("quit addTop\n");
  return sum;
}

int main()
{
  int top = 100;

  int result = addTop(top);

  printf("result:%d\n",result);
  return 0;
}

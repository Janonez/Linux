#pragma once 

#include <stdio.h>

#define NUM 1024

#define BUFF_NONE 0x1
#define BUFF_LINE 0x2
#define BUFF_ALL  0x4

typedef struct _MY_FILE
{
  int fd;
  char outputbuffer[NUM];
  int current;
  int flags; //刷新策略
}MY_FILE;

MY_FILE *my_fopen(const char *path,const char *mode);

size_t my_fwrite(const void *ptr,size_t size,size_t nmemb,MY_FILE *stream);

int my_fclose(MY_FILE *fp);

int my_fflush(MY_FILE *fp);

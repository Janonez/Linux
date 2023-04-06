#include "mystdio.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

// fopen("/a/b/c.txt","a");
// fopen("/a/b/c.txt","w");
// fopen("/a/b/c.txt","r");
MY_FILE *my_fopen(const char *path,const char *mode)
{
  // 1. 识别标志位 "a" "w" "r"
  int flag = 0;
  if(strcmp(mode,"r") == 0) flag |= O_RDONLY;
  else if(strcmp(mode,"w") == 0) flag |= (O_CREAT | O_WRONLY | O_TRUNC);
  else if(strcmp(mode,"a") == 0) flag |= (O_CREAT | O_WRONLY | O_APPEND);
  else {
    // 其他操作符
    // "r+" "w+" "a+" ……
  }

  // 2. 尝试打开文件
  mode_t m = 0666; // 默认权限
  int fd = 0;
  if(flag & O_CREAT) fd = open(path,flag,m); // mode 是创建文件的权限
  else fd = open(path,flag);

  if(fd < 0) return NULL;

  // 3. 构建
  MY_FILE *mf = (MY_FILE*)malloc(sizeof(MY_FILE));
  if(mf == NULL)
  {
    close(fd);
    return NULL;
  }

  // 4. 初始化对象
  mf->fd = fd;
  mf->flags = 0;
  mf->flags |= BUFF_LINE;
  memset(mf->outputbuffer,'\0',sizeof(mf->outputbuffer));
  mf->current = 0;

  // 5. 返回打开的文件
  return mf;
}

int my_fflush(MY_FILE *fp)
{
  assert(fp);
  write(fp->fd, fp->outputbuffer,fp->current);
  fp->current = 0;
  
  fsync(fp->fd);// 强制刷新
  
  return 0;
}

size_t my_fwrite(const void *ptr,size_t size,size_t nmemb,MY_FILE *stream)
{
  // 1. 缓冲区如果已经满了，直接写入
  if(stream->current == NUM) my_fflush(stream);

  // 2. 根据缓冲区剩余情况，进行数据拷贝即可
  size_t user_size = size * nmemb;
  size_t my_size = NUM - stream->current;

  size_t writen = 0;
  if(my_size >= user_size)
  {
    memcpy(stream->outputbuffer + stream->current,ptr,user_size);
    // 更新计数器字段
    stream->current += user_size;
    writen = user_size;
  }
  else 
  {
    memcpy(stream->outputbuffer + stream->current, ptr, my_size);
    // 更新计数器字段
    stream->current += my_size;
    writen = my_size;
  }
  // 3. 开始刷新计划
  if(stream->flags & BUFF_ALL)
  {
    if(stream->current == NUM) my_fflush(stream);
  }
  else if(stream->flags & BUFF_LINE)
  {
    if(stream->outputbuffer[stream->current - 1] == '\n') my_fflush(stream);
  }
  else
  {

  }
  return writen;
}

int my_fclose(MY_FILE *fp)
{
  assert(fp);
  // 1. 冲刷缓冲区
  if(fp->current > 0) my_fflush(fp);
  // 2. 关闭文件
  close(fp->fd);
  // 3. 释放堆空间
  free(fp);
  // 4. 指针置空
  fp = NULL;

  return 0;
}

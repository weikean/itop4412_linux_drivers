#include <stdio.h>
#include <sys/types.h> //基本系统数据类型
#include <sys/stat.h> //系统调用函数头文件
#include <fcntl.h>  //定义open 函数
#include <unistd.h> //定义close函数
#include <sys/ioctl.h> //定义ioctl函数

int  main() {
  int fd;
  unsigned char buffer[2];
  char *read_key = "/dev/pollkey_ctl";
  //O_RDWR 只读打开  O_NDELAY 非阻塞方式打开
  if((fd = open(read_key,O_RDWR|O_NDELAY))<0)
  {
    printf("APP open %s failed\n",read_key);
    return -1;
  } //只读，阻塞方式
  else
  {
  printf("APP open %s success\n",read_key);
  while(1)
  {
    read(fd,buffer,sizeof(buffer));
    if(!buffer[0] || !buffer[1])
    {
      printf("HOME key value is %s\n", buffer[0]);
      printf("BACK key value is %s\n", buffer[1]);
    }
  }
  }
  close(fd);
  return 0;
}

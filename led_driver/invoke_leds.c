#include <stdio.h>
#include <sys/types.h> //基本系统数据类型
#include <sys/stat.h> //系统调用函数头文件
#include <fcntl.h>  //定义open 函数
#include <unistd.h> //定义close函数
#include <sys/ioctl.h> //定义ioctl函数

#define GPIOS 32

int  main(int argc , int **argv) {
  int fd,i,cmd = 2;
  char *hello_node = "/dev/hello_ctl";
  char *cmd0 = "0";
  char *cmd1 = "1";
  if (strcmp(argv[1],cmd0) == 0) {
    cmd = 0;
    printf("cmd = 0 !\n");
  }

  if (strcmp(argv[1],cmd1) == 0) {
    cmd = 1;
    printf("cmd = 1 !\n");
  }

  //O_RDWR 只读打开  O_NDELAY 非阻塞方式打开
  if((fd = open(hello_node,O_RDWR|O_NDELAY))<0)
  {
    printf("APP open %s failed\n",hello_node);
  } //只读，阻塞方式
  else
  {
  printf("APP open %s success\n",hello_node);
  for(i = 0;i< GPIOS ;i++)
  {
    ioctl(fd,cmd,i);
    printf("APP ioctl %s, cmd is %d,i is %d!\n",hello_node,cmd,i );
  }
  }
  close(fd);
  return 0;
}

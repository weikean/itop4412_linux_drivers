#include <stdio.h>
#include <sys/types.h> //基本系统数据类型
#include <sys/stat.h> //系统调用函数头文件
#include <fcntl.h>  //定义open 函数
#include <unistd.h> //定义close函数
#include <sys/ioctl.h> //定义ioctl函数

int  main() {
  int fd;
  char *hello_node0 = "/dev/chardevnode0";
  char *hello_node1 = "/dev/chardevnode1";
  //O_RDWR 只读打开  O_NDELAY 非阻塞方式打开
  if((fd = open(hello_node0,O_RDWR|O_NDELAY))<0)
  {
    printf("APP open %s failed\n",hello_node0);
  } //只读，阻塞方式
  else
  {
  printf("APP open %s success\n",hello_node1);
  // ioctl(fd,1,1);
  // sleep(3);
  // ioctl(fd,0,1);
  // sleep(3);
  // ioctl(fd,1,1);
  }
close(fd);
  if((fd = open(hello_node0,O_RDWR|O_NDELAY))<0)
  {
    printf("APP open %s failed\n",hello_node0);
  } //只读，阻塞方式
  else
  {
  printf("APP open %s success\n",hello_node1);
  // ioctl(fd,1,1);
  // sleep(3);
  // ioctl(fd,0,1);
  // sleep(3);
  // ioctl(fd,1,1);
  }

  close(fd);
  return 0;
}

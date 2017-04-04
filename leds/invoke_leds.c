#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

main(){
	int fd;
	char *hello_node = "/dev/hello_ctl";
	
/*O_RDWR只读打开,O_NDELAY非阻塞方式*/	
	if((fd = open(hello_node,O_RDWR|O_NDELAY))<0){
		printf("APP open %s failed",hello_node);
	}
	else{
		printf("APP open %s success",hello_node);
		ioctl(fd,1,1);
		sleep(3);
		ioctl(fd,0,1);
		sleep(3);
		ioctl(fd,1,1);
	}
	
	close(fd);
}
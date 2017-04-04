#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
	int fd,cmd = 2;
	char *read_gpio_node = "/dev/gpioo_read_ctl";
	char *cmd0 = "0";
	char *cmd1 = "1";
	printf("arg[0] is %s,arg[1] is %s;",argv[0],argv[1]);

	if(strcmp(argv[1],cmd0) == 0)
	{
		cmd = 0;
	}
	if(strcmp(argv[1],cmd1) == 0)
	{
		cmd = 1;
	}
	if(fd = open(read_gpio_node,O_RDWR | O_NDELAY))
	{
		printf("APP open %s is failed \n",read_gpio_node);
	}
	else
	{
		printf("APP open %s is success \n",read_gpio_node);
		printf("%d io value is %d",cmd,ioctl(fd,cmd,0));
	}
	close(fd);
}

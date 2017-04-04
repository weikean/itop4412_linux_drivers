#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

void main(int argc, char* argv[])
{
	char *relay_ctl = "/dev/relay_ctl";
	
	int fd, ret;

	if(argc < 2)
	{
		printf("usage: ././relaytest 1 1\n");

		exit(1);
	}

	if((fd = open(relay_ctl, O_RDWR|O_NOCTTY|O_NDELAY))<0)
        {
                printf("Open %s faild\n", relay_ctl);
                //close(fd1);

                exit(1);
        }

	ret = ioctl(fd, atoi(argv[1]), atoi(argv[2]));

	return ;
}

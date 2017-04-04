#include <linux/init.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
//#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <asm/io.h>
#include <linux/regulator/consumer.h>
//#include "gps.h"
#include <linux/delay.h>

//copy_to_user的头文件
#include <asm/uaccess.h>

#define DPRINTK(x...) printk("POLLKEY_CTL DEBUG:" x)
#define DRIVER_NAME "pollkey"

static int key_gpios[] = {
	EXYNOS4_GPX1(1),
	EXYNOS4_GPX1(2),
};

int pollkey_open(struct inode *inode,struct file *filp)
{
	DPRINTK("Device Opened Success!\n");
	return nonseekable_open(inode,filp);
}

int pollkey_release(struct inode *inode,struct file *filp)
{
	DPRINTK("Device Closed Success!\n");
	return 0;
}

static ssize_t pollkey_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
	unsigned char key_value[2];
	
	if(size != sizeof(key_value)){
		return -1;
	}
	
	key_value[0] = gpio_get_value(key_gpios[0]);
	key_value[1] = gpio_get_value(key_gpios[1]);
	
	copy_to_user(buff,key_value,sizeof(key_value));
	return 0;
}

static struct file_operations pollkey_ops = {
	.owner 	= THIS_MODULE,
	.open 	= pollkey_open,
	.release= pollkey_release,
	.read 	= pollkey_read,
};

static struct miscdevice pollkey_dev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.fops	= &pollkey_ops,
	.name	= "pollkey",
};


static int pollkey_probe(struct platform_device *pdev)
{
	int ret,i;
	char *banner = "pollkey Initialize\n";
	printk(banner);
	
	//设置为输入模式
	for(i=0;i<2;i++){
		ret = gpio_request(key_gpios[i],"key_gpio");
		s3c_gpio_cfgpin(key_gpios[i],S3C_GPIO_INPUT); 
		s3c_gpio_setpull(key_gpios[i],S3C_GPIO_PULL_NONE);
	}
	
	ret = misc_register(&pollkey_dev);
	return 0;
}

static int pollkey_remove (struct platform_device *pdev)
{
	misc_deregister(&pollkey_dev);	
	return 0;
}

static int pollkey_suspend (struct platform_device *pdev, pm_message_t state)
{
	DPRINTK("pollkey suspend:power off!\n");
	return 0;
}

static int pollkey_resume (struct platform_device *pdev)
{
	DPRINTK("pollkey resume:power on!\n");
	return 0;
}

static struct platform_driver pollkey_driver = {
	.probe = pollkey_probe,
	.remove = pollkey_remove,
	.suspend = pollkey_suspend,
	.resume = pollkey_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static void __exit pollkey_exit(void)
{
	platform_driver_unregister(&pollkey_driver);
}

static int __init pollkey_init(void)
{
	return platform_driver_register(&pollkey_driver);
}

module_init(pollkey_init);
module_exit(pollkey_exit);
MODULE_LICENSE("Dual BSD/GPL");

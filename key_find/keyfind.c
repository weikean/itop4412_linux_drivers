/*
 硬件准备：
 	两个按键 Home和Back
 	UART_RING和SIM-DET
 	GPX1_1和GPX1_2
 	EXYNOS4_GPX(1),EXYNOS4_GPX(2)

 	设置KEY为输入模式
 	添加驱动中的read函数
*/

#include <linux/init.h>
#include <linux/module.h>


#include <linux/kernel.h>

/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>
/*注册杂项设备头文件*/
#include <linux/miscdevice.h>
/*注册设备节点的文件结构体*/
#include <linux/fs.h>

/*Linux中申请GPIO的头文件*/
#include <linux/gpio.h>
/*三星平台的GPIO配置函数头文件*/
/*三星平台EXYNOS系列平台，GPIO配置参数宏定义头文件*/
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
/*三星平台4412平台，GPIO宏定义头文件*/
#include <mach/gpio-exynos4.h>

#include <mach/regs-gpio.h>
#include <asm/io.h>
#include <linux/regulator/consumer.h>

//copy_to_user  头文件
#include <asm/uaccess.h>

#define DRIVER_NAME "pollkey_ctl"
#define DEVICE_NAME "pollkey_ctl"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

static int key_gpios[] = 
{
	EXYNOS4_GPX1(1),
	EXYNOS4_GPX1(2),
};

static ssize_t pollkey_read(struct file *files, char __user *buff, size_t size, loff_t *ppos){

	unsigned char key_value[2];
	if(size != sizeof(key_value))
	{
		return -1;
	}	

	key_value[0] = gpio_get_value(key_gpios[0]);
	key_value[0] = gpio_get_value(key_gpios[1]);
	
	copy_to_user(buff,key_value,sizeof(key_value));//驱动给应用传值
	return 0;
}

static int pollkey_release(struct inode *inode, struct file *file){
	printk(KERN_EMERG "hello release\n");
	return 0;
}

static int pollkey_open(struct inode *inode, struct file *file){
	printk(KERN_EMERG "hello open\n");
	return 0;
}

static struct file_operations pollkey_ops = {
	.owner = THIS_MODULE,
	.open = pollkey_open,
	.release = pollkey_release,
	.read = pollkey_read,
};

static  struct miscdevice pollkey_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &pollkey_ops,
};


static int pollkey_probe(struct platform_device *pdv){
	int ret,i;
	
	printk(KERN_EMERG "\tinitialized\n");

	for (i = 0; i < 2; ++i)
	{
		ret = gpio_request(key_gpios[i],"key_gpio");
    	s3c_gpio_cfgpin(key_gpios[i],S3C_GPIO_INPUT);
    	s3c_gpio_setpull(key_gpios[i],S3C_GPIO_PULL_NONE);
 	}
	misc_register(&pollkey_dev);
	
	return 0;
}

static int pollkey_remove(struct platform_device *pdv){
	
	printk(KERN_EMERG "\tremove\n");
	misc_deregister(&pollkey_dev);
	return 0;
}

static void pollkey_shutdown(struct platform_device *pdv){
	
	;
}

static int pollkey_suspend(struct platform_device *pdv,pm_message_t pmt){
	
	return 0;
}

static int pollkey_resume(struct platform_device *pdv){
	
	return 0;
}

struct platform_driver pollkey_driver = {
	.probe = pollkey_probe,
	.remove = pollkey_remove,
	.shutdown = pollkey_shutdown,
	.suspend = pollkey_suspend,
	.resume = pollkey_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};


static int pollkey_init(void)
{
	int DriverState;
	
	printk(KERN_EMERG "HELLO WORLD enter!\n");
	DriverState = platform_driver_register(&pollkey_driver);
	
	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}


static void pollkey_exit(void)
{
	printk(KERN_EMERG "HELLO WORLD exit!\n");
	
	platform_driver_unregister(&pollkey_driver);	
}

module_init(pollkey_init);
module_exit(pollkey_exit);

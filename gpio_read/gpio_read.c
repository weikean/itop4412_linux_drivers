/*
拨码开关
AP_SLEEP---GPC03---EXYNOS4_GPC0(3)
XEINT6----GPX0_6---EXYNOS4_GPX0(6)

1.寄存器设置
  1.需要设置为输入
  2.读寄存器状态
  3.不上拉，不下拉
2.GPIO 输入需要的函数
  1.gpio_request  //申请
  2.gpio_get_value
  3.s3c_gpio_cfgin(S3C_GPIO_INPUT/S3C_GPIO_OUTPUT)
  4.s3c_gpio_setpull(S3C_GPIO_PULL_NONE)//设置上拉下拉
  5.gpio_free
3.平台文件中的设备注册

  struct platform_device s3c_device_read_gpio_ctl = {
      .name   ="read_gpio_ctl",
      .id             =-1,
  };
释放GPIO
  gpio_free(EXYNOS4_GPC0(3));
  gpio_free(EXYNOS4_GPX0(6));
4.Makefile修改

5.驱动的修改

  &s3c_device_read_gpio_ctl,
/*

#include <linux/init.h>
#include <linux/module.h>

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

#define DRIVER_NAME "gpio_read_ctl"
#define DEVICE_NAME "gpioo_read_ctl"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");


//获取管脚电平
static long gpio_read_ioctl( struct file *files, unsigned int cmd, unsigned long arg){
	printk("cmd is %d,arg is %d\n",cmd,arg);
  //参数cmd 0 ,1
	if(cmd > 1){
		printk(KERN_EMERG "cmd is 0 or 1\n");
	}
	if(arg > 1){
		printk(KERN_EMERG "arg is only 1\n");
	}
  //如果cmd = 0,返回EXYNOS4_GPX0 拨码开关3的值
  //..........1.................拨码开关4的值
  if(0 == cmd)
    return gpio_get_value(EXYNOS4_GPC0(3));
  if(0 == cmd)
    return gpio_get_value(EXYNOS4_GPX0(6));
	return 0;
}

static int gpio_read_release(struct inode *inode, struct file *file){
	printk(KERN_EMERG "gpio_read release\n");
	return 0;
}

static int gpio_read_open(struct inode *inode, struct file *file){
	printk(KERN_EMERG "gpio_read open\n");
	return 0;
}

static struct file_operations gpio_read_ops = {
	.owner = THIS_MODULE,
	.open = gpio_read_open,
	.release = gpio_read_release,
	.unlocked_ioctl = gpio_read_ioctl,
};

static  struct miscdevice gpio_read_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &gpio_read_ops,
};


static int gpio_read_probe(struct platform_device *pdv){
	int ret;

	printk(KERN_EMERG "\tinitialized\n");

	ret = gpio_request(EXYNOS4_GPC0(3),"SWITCH3");
	if(ret < 0){
		printk(KERN_EMERG "gpio_request EXYNOS4_GPC0(3) failed!\n");
		return ret;
	}
  else
  {
    s3c_gpio_cfgpin(EXYNOS4_GPC0(3),S3C_GPIO_INPUT);
    s3c_gpio_setpull(EXYNOS4_GPC0(3),S3C_GPIO_PULL_NONE);

  }
  ret = gpio_request(EXYNOS4_GPX0(6),"SWITCH4");
  if(ret < 0){
		printk(KERN_EMERG "gpio_request EXYNOS4_GPX0(6) failed!\n");
		return ret;
	}
  else
  {
    s3c_gpio_cfgpin(EXYNOS4_GPX0(6),S3C_GPIO_INPUT);
    s3c_gpio_setpull(EXYNOS4_GPX0(6),S3C_GPIO_PULL_NONE);
  }

	s3c_gpio_cfgpin(EXYNOS4_GPL2(0),S3C_GPIO_OUTPUT);

	gpio_set_value(EXYNOS4_GPL2(0),0);

	misc_register(&gpio_read_dev);

	return 0;
}

static int gpio_read_remove(struct platform_device *pdv){

	printk(KERN_EMERG "\tremove\n");
	misc_deregister(&gpio_read_dev);
	return 0;
}

static void gpio_read_shutdown(struct platform_device *pdv){

	;
}

static int gpio_read_suspend(struct platform_device *pdv,pm_message_t pmt){

	return 0;
}

static int gpio_read_resume(struct platform_device *pdv){

	return 0;
}

struct platform_driver gpio_read_driver = {
	.probe = gpio_read_probe,
	.remove = gpio_read_remove,
	.shutdown = gpio_read_shutdown,
	.suspend = gpio_read_suspend,
	.resume = gpio_read_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};


static int gpio_read_init(void)
{
	int DriverState;

	printk(KERN_EMERG "gpio_read WORLD enter!\n");
	DriverState = platform_driver_register(&gpio_read_driver);

	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}


static void gpio_read_exit(void)
{
	printk(KERN_EMERG "gpio_read WORLD exit!\n");

	platform_driver_unregister(&gpio_read_driver);
}

module_init(gpio_read_init);
module_exit(gpio_read_exit);

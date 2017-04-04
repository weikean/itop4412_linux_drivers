/*
流程： 按键按下产生中断---> 跳转到异常向量入口，执行中断函数

中断函数做什么:
	1.保存现场
	2.执行中断处理函数
	3.恢复现场

学会使用中断注册函数，了解注册中断相关函数和结构体
	中断注册函数(通用)  request_irq 
	request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
	参数1：irq,中断号。（和平台架构相关，结合datasheet以及平台文件）
	参数2：中断处理函数
	参数3：中断标记。上升/下降沿，高/低电平……
	参数4：中断名字。cat /proc/interrupts
	参数5：使用设备的设备结构体或者NULL。
	free_irq
	free_irq(irq,*dev_id) 
	
request_irg的调用

4412上外部中断号如何对应

EXYNOS4_GPX1[1] 和 EXYNOS4_GPX1[2]
-->KP_COL[1]-->XEINT_9
-->KP_COL[2]-->XEINT_10
-->IRQ_EINT(9) 和 IRQ_EINT(10)
IRQ_EINT()和datasheet 相对应

在平台文件中注册设备 keyirq

struct platform_device s3c_device_keyirq_ctl = {
        .name   ="keyirq",
        .id             =-1,
};

 &s3c_device_keyirq_ctl,

*/

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

//中断头文件
#include <linux/irq.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "keyirq_ctl"
#define DEVICE_NAME "keyirq_ctl"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

static long keyirq_ioctl( struct file *files, unsigned int cmd, unsigned long arg){
	printk("cmd is %d,arg is %d\n",cmd,arg);
	
	if(cmd > 1){
		printk(KERN_EMERG "cmd is 0 or 1\n");
	}
	if(arg > 1){
		printk(KERN_EMERG "arg is only 1\n");
	}
	
	gpio_set_value(EXYNOS4_GPL2(0),cmd);
	
	return 0;
}

static int keyirq_release(struct inode *inode, struct file *file){
	printk(KERN_EMERG "keyirq release\n");
	return 0;
}

static int keyirq_open(struct inode *inode, struct file *file){
	printk(KERN_EMERG "keyirq open\n");
	return 0;
}

static struct file_operations keyirq_ops = {
	.owner = THIS_MODULE,
	.open = keyirq_open,
	.release = keyirq_release,
	.unlocked_ioctl = keyirq_ioctl,
};

static  struct miscdevice keyirq_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &keyirq_ops,
};


static	irqreturn_t eint9_interrupt(int irq, void *dev_id)
{
	printk("recieve a interrupt 9");
	return IRQ_HANDLED;  //返回原来执行的地址
}

static	irqreturn_t eint10_interrupt(int irq, void *dev_id)
{
	printk("recieve a interrupt 10");
	return IRQ_HANDLED;  //返回原来执行的地址
}

static int keyirq_probe(struct platform_device *pdv){
	int ret;
	printk(KERN_EMERG "\tinitialized\n");

	request_irq(IRQ_EINT9,eint9_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint9",pdv);
	request_irq(IRQ_EINT9,eint10_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint10",pdv);
	//下降沿触发

	//注册中断

	
	return 0;
}

static int keyirq_remove(struct platform_device *pdv){
	
	printk(KERN_EMERG "\tremove\n");
	free_irq(IRQ_EINT9,pdv);
	free_irq(IRQ_EINT10,pdv);
	//misc_deregister(&keyirq_dev);
	return 0;
}

static void keyirq_shutdown(struct platform_device *pdv){
	
	;
}

static int keyirq_suspend(struct platform_device *pdv,pm_message_t pmt){
	
	return 0;
}

static int keyirq_resume(struct platform_device *pdv){
	
	return 0;
}

struct platform_driver keyirq_driver = {
	.probe = keyirq_probe,
	.remove = keyirq_remove,
	.shutdown = keyirq_shutdown,
	.suspend = keyirq_suspend,
	.resume = keyirq_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};


static int keyirq_init(void)
{
	int DriverState;
	
	printk(KERN_EMERG "keyirq WORLD enter!\n");
	DriverState = platform_driver_register(&keyirq_driver);
	
	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}


static void keyirq_exit(void)
{
	printk(KERN_EMERG "keyirq WORLD exit!\n");
	
	platform_driver_unregister(&keyirq_driver);	
}

module_init(keyirq_init);
module_exit(keyirq_exit);





/*
  静态申请字符设备号申请

  include/linux/fs.h
  extern int register_chrdev_region(dev_t, unsigned, const char *);
  //dev_t  cdev
    unsigned 次设备号个数
    const   注册的设备名
 //知道主次设备号,然后向系统注册
  alloc_chrdev_region(dev_t *, unsigned, unsigned, const char *)
  //系统自动注册主次设备号
  extern void unregister_chrdev_region(dev_t, unsigned);
//注销设备号


宏定义NKDEV 的头文件 include/linux/kdev_t.h
    --有一系列设备号处理的宏定义
#define MKDEV(ma,mi)    ((ma)<<8 | (mi))


include/linux/cdev.h

struct cdev {
        struct kobject kobj;
        struct module *owner;
        const struct file_operations *ops;
        struct list_head list;
        dev_t dev;
        unsigned int count;
};


cdev类型是字符设备描述的结构，设备号必须用cdev来描述，
高12位为主设备号，低20位为次设备号
*/
#include <linux/init.h>
#include <linux/module.h>

/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>

//内核模块传参数  定义module_param,module_param_array
#include <linux/moduleparam.h>

//模块perm参数的访问权限
#include <linux/stat.h>

//字符设备注册函数
#include <linux/fs.h>
//转换设备号数据类型的宏定义
#include <linux/kdev_t.h>
//定义字符设备的结构体
#include <linux/cdev.h>


#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "scdev"
#define DEVICE_MINOR_NUM 2 //次设备号 个数
#define DEV_MAJOR 0  //主设备号
#define DEV_MINOR 0 //次设备号


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

int numdev_major = DEV_MAJOR,numdev_minor = DEV_MINOR;

module_param(numdev_major,int,S_IRUSR);
module_param(numdev_minor,int,S_IRUSR);


static int hello_probe(struct platform_device *pdv){

	printk(KERN_EMERG "\tinitialized\n");


	return 0;
}

static int hello_remove(struct platform_device *pdv){

	return 0;
}

static void hello_shutdown(struct platform_device *pdv){

	;
}

static int hello_suspend(struct platform_device *pdv){

	return 0;
}

static int hello_resume(struct platform_device *pdv){

	return 0;
}

struct platform_driver hello_driver = {
	.probe = hello_probe,
	.remove = hello_remove,
	.shutdown = hello_shutdown,
	.suspend = hello_suspend,
	.resume = hello_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};


static int hello_init(void)
{
	int DriverState,ret;
  dev_t num_dev;

  printk(KERN_EMERG "module numdev_major is %d\n",numdev_major);
  printk(KERN_EMERG "module numdev_minor is %d\n",numdev_minor);

  if(numdev_major)
  {
    num_dev = MKDEV(numdev_major,numdev_minor);
    ret = register_chrdev_region(num_dev,DEVICE_MINOR_NUM,DEVICE_NAME);
  }
  else
  {
    printk(KERN_EMERG "numdev_major %d is failed \n",numdev_major);
  }
  if (ret < 0) {
    printk(KERN_EMERG "register_chrdev_region %d is failed\n",numdev_major);
  }

	printk(KERN_EMERG "CDEV WORLD enter!\n");
	//DriverState = platform_driver_register(&hello_driver);

	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_EMERG "CDEV WORLD exit!\n");
  unregister_chrdev_region(MKDEV(numdev_major,numdev_minor),DEVICE_MINOR_NUM);
	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);

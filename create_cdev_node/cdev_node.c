/*
  生成字符类设备设备节点

  class 是设备类，提供给用户接口相似设备的集合(input ,usb,串口tty 块设备)
  misc 设备对class做了打包，不需要创建

  函数class_create 创建class文件
  #define class_create(owner, name)               \
({                                              \
        static struct lock_class_key __key;     \
        __class_create(owner, name, &__key);    \
})

  参数1 :一般是THIS_MODULE
      2:设备名称
      3:创建一个设备类
      4:返回一个结构体变量

  class结构体变量
  --设备驱动模型中通用的设备类结构
  --include/linux/device.h
  struct class {
        const char              *name;
        struct module           *owner;

        struct class_attribute          *class_attrs;
        struct device_attribute         *dev_attrs;
        struct bin_attribute            *dev_bin_attrs;
        struct kobject                  *dev_kobj;
        ....

   释放内存
   kfree
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
//定义字符设备的结构体 ,字符设备初始化，注册，卸载函数
#include <linux/cdev.h>

//分配(小)内存空间的函数
#include <linux/slab.h>

#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "ascdev"
#define DEVICE_MINOR_NUM 2 //次设备号 个数
#define DEV_MAJOR 0  //主设备号
#define DEV_MINOR 0 //次设备号
#define REGDEV_SIZE 3000

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

int numdev_major = DEV_MAJOR,numdev_minor = DEV_MINOR;

module_param(numdev_major,int,S_IRUSR);
module_param(numdev_minor,int,S_IRUSR);

struct reg_deg
{
  char *data;
  unsigned long size;
  struct cdev cdev;
};

struct reg_deg *mydevices;

struct file_operations my_fops = {
  .owner = THIS_MODULE,
  //设备节点
};
  //设备注册到系统
static void reg_init_cdev(struct reg_deg *dev,int index){
  int err;
  int devno = MKDEV(numdev_major,numdev_minor + index);
  //数据初始化
  cdev_init(&(dev->cdev),&my_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &my_fops;
  //设备注册到系统
  err = cdev_add(&dev->cdev,devno,1);
  if (err) {
    	printk(KERN_EMERG "CDEV_ADD %d is failed err is %d\n",index,err);
  }
  else{
      printk(KERN_EMERG "CDEV_ADD  %d is success\n",index);
  }
  printk(KERN_EMERG "\tdevno is %d, num is %d\n",devno,DEVICE_MINOR_NUM);
}

static int hello_probe(struct platform_device *pdv){

	printk(KERN_EMERG "\thello innitial\n");


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
	int DriverState,i,ret;
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
		ret = alloc_chrdev_region(&num_dev,DEV_MINOR,DEVICE_MINOR_NUM,DEVICE_NAME);
		numdev_major = MAJOR(num_dev);
    printk(KERN_EMERG "adev_region is %d  \n",numdev_major);
  }
  if (ret < 0) {
    printk(KERN_EMERG "register_chrdev_region %d is failed\n",numdev_major);
  }

  mydevices = kmalloc(DEVICE_MINOR_NUM * (sizeof(struct reg_deg)),GFP_KERNEL);
  if(!mydevices)
  {
    ret = -1;
    goto fail;
  }
  memset(mydevices,0,DEVICE_MINOR_NUM * sizeof(struct reg_deg));
  for ( i = 0; i < DEVICE_MINOR_NUM; i++) {
    mydevices[i].data = kmalloc(REGDEV_SIZE,GFP_KERNEL);
    memset(mydevices[i].data,0,REGDEV_SIZE);

    reg_init_cdev(&mydevices[i],i);
   }

	printk(KERN_EMERG "Scdv init!\n");
	//DriverState = platform_driver_register(&hello_driver);
	return 0;
fail:
    unregister_chrdev_region(MKDEV(numdev_major,numdev_minor),DEVICE_MINOR_NUM);
    printk(KERN_EMERG "KMALLOC FAILED!\n");
    return ret;
}



static void hello_exit(void)
{
  int i;
	printk(KERN_EMERG "CDEV WORLD exit!\n");

  for (i = 0; i < DEVICE_MINOR_NUM; i++) {
  cdev_del(&(mydevices[i].cdev));
  }

  unregister_chrdev_region(MKDEV(numdev_major,numdev_minor),DEVICE_MINOR_NUM);
	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);

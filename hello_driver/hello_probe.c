/*
Linux 驱动头文件
  #include <linux/module.h> //include/linux/module.h

   //所有驱动代码必须遵循GPL协议
   MODULE_LICENSE(_licence)//GPL
   MODULE_AUTHOR(_author)

   #inlcude<linux/init.h>
   --包含初始化宏定义的头文件
   入口函数module_init(x),
   出口函数module_exit(x),

   make--makefile--linux_module--linux源码--收集信息--模块编译(.c --.ko)

驱动注册
  1.#include<linux/platform_device.h>
  2.所需结构体
  struct platform_driver {
        int (*probe)(struct platform_device *);//初始化
        int (*remove)(struct platform_device *);//移除
        void (*shutdown)(struct platform_device *);//关闭
        int (*suspend)(struct platform_device *, pm_message_t state);//休眠
        int (*resume)(struct platform_device *);//休眠后恢复
        struct device_driver driver;//name要和设备一致,owner一般赋值THIS_MODULE
        const struct platform_device_id *id_table;
};
extern int platform_driver_register(struct platform_driver *);
extern void platform_driver_unregister(struct platform_driver *);


杂项设备
  1.初始化文件
    -driver/char/misc.c(简单的驱动更容易实现)
  2.注册头文件
    -#include/linux/miscdevice.h
    相关结构体
    struct miscdevice  {
        int minor;//设备号
        const char *name;//设备节点名称
        const struct file_operations *fops;//指向一个设备节点文件
        struct list_head list;
        struct device *parent;
        struct device *this_device;
        const char *nodename;
        mode_t mode;
};


  3.内核文件的结构体
  注册设备节点，本质上是新建一个特殊的文件，包含文件名，打开，关闭，操作等函数
  头文件include/linux/fs.h
  文件的结构体(部分)
  struct file_operations {
        struct module *owner;(一般参数是THIS_MODULE)
        loff_t (*llseek) (struct file *, loff_t, int);
        ...
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        //应用向底层驱动传值
        int (*open) (struct inode *, struct file *);//open打开文件函数
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);//关闭文件
        ...
*/


#include <linux/module.h>
#include <linux/init.h>

/*驱动注册头文件，包含结构体和注册，卸载等函数*/
#include<linux/platform_device.h>

/*注册杂项设备头文件*/
#include <linux/miscdevice.h>
/*注册设备节点的头文件结构体*/
#include <linux/fs.h>

#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "hello_ctl123"

MODULE_LICENSE("Dual BSD/GPL");//开源，没有内核版本的限制
MODULE_AUTHOR("xiaoan");

static long hello_ioctl(struct file *files, unsigned int cmd, unsigned long arg){
  printk("cmd is %d,arg is %ld\n",cmd,arg);
  return 0;
}

static int hello_release(struct inode *inode ,struct file *file){
  printk(KERN_EMERG "hello release\n");
  return 0;
}

static int hello_open(struct inode *inode ,struct file *file){
  printk(KERN_EMERG "hello open\n");
  return 0;
}

static struct file_operations hello_ops = {
  .owner = THIS_MODULE,
  .open = hello_open,
  .release = hello_release,
  .unlocked_ioctl = hello_ioctl,
};

//杂项设备注册
static struct  miscdevice hello_dev = {
  .minor = MISC_DYNAMIC_MINOR ,
  .name = DEVICE_NAME,
  .fops = &hello_ops,
};

static int hello_probe(struct platform_device *pdv)
{
  printk(DRIVER_NAME "\t initialized");
  misc_register(&hello_dev);
  return 0;
}

static int hello_remove(struct platform_device *pdv)
{
  printk(DRIVER_NAME "\t has removed\n");
  misc_deregister(&hello_dev);
  return 0;
}

static void hello_shutdown(struct platform_device *pdv)
{
  ;
}

static int hello_resume(struct platform_device *pdv)
{
  return 0;
}

static int hello_suspend(struct platform_device *pdv,pm_message_t pmt){

	return 0;
}

struct platform_driver hello_driver = {
    .probe = hello_probe,
    .remove = hello_remove,
    .shutdown = hello_shutdown,
    .suspend = hello_suspend,
    .resume = hello_resume,
    .driver =
    {
      .name = DRIVER_NAME,
      .owner = THIS_MODULE,
    }
};


static int hello_init(void)
{
  int DriverState = 0;
  printk(KERN_EMERG "HELLO WORLD ENTER"); //KERN_EMERG添加权限，紧急事件消息,
                                         // 系统崩溃之前提示,表示系统不可用
  DriverState = platform_driver_register(&hello_driver);
  printk(DRIVER_NAME "\tDriverState is %d\n",DriverState);
  return 0;
}


static void hello_exit(void)
{
  printk(KERN_EMERG "HELLO WORLD EXIT");
  platform_driver_unregister(&hello_driver);
}
module_init(hello_init);
module_exit(hello_exit);

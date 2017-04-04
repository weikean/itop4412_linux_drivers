/*
  内核模块传参数

  -单个参数
  include/linux/moduleparam.h
  module_param(name, type, perm)
  name : 模块参数名称
  type : 模块参数的数据类型
  perm : 模块参数的访问权限(S_IRUSR参数表示所有文件所有者可读)
  #include/linux/stat.h

  -多个参数
  module_param_array(name, type, nump, perm)
  nump - 保存参数个数的地址

*/

#include <linux/init.h>
#include <linux/module.h>

/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>

//内核模块传参数  定义module_param,module_param_array
#include <linux/moduleparam.h>

//模块perm参数的访问权限
#include <linux/stat.h>

#define DRIVER_NAME "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("TOPEET");

static int module_arg1,module_arg2;
static int int_array[50];

static int int_num;


module_param(module_arg1,int,S_IRUSR);
module_param(module_arg2,int,S_IRUSR);
module_param_array(int_array,int,&int_num,S_IRUSR);

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
	int DriverState,i;
  printk(KERN_EMERG "module arg1 is %d\n",module_arg1);
  printk(KERN_EMERG "module arg2 is %d\n",module_arg2);
  for (i = 0 ; i < int_num ;i++) {
    printk(KERN_EMERG "int array[%d] is %d\n",i,int_array[i]);
  }
	printk(KERN_EMERG "HELLO WORLD enter!\n");
	DriverState = platform_driver_register(&hello_driver);

	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_EMERG "HELLO WORLD exit!\n");

	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);

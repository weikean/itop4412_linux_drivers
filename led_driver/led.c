/*
申请GPIO头文件
  -include/linux/gpio.h
三星平台申请GPIO头文件 三星平台EXYNOS系列平台，GPIO配置参数宏定义头文件
  arch/arm/plat-samsung/include/plat/gpio-cfg.h
  --包括三星平台处理器的配置
三星平台EXYNOS系列平台，GPIO配置参数宏定义头文件
  --arch/arm/mach-exynos/include/mach/gpio.h
  --GPIO管脚拉高拉低配置参数等
三星4412平台，GPIO宏定义头文件
  --arch/arm/mach-exynos/include/mach/gpio-exynos4.h
*/
#include <linux/module.h>
#include <linux/init.h>

/*驱动注册头文件，包含结构体和注册，卸载等函数*/
#include<linux/platform_device.h>

/*注册杂项设备头文件*/
#include <linux/miscdevice.h>
/*注册设备节点的头文件结构体*/
#include <linux/fs.h>

//GPIO头文件
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>


#define DRIVER_NAME "hello_ctl"
#define DEVICE_NAME "hello_ctl"

MODULE_LICENSE("Dual BSD/GPL");//开源，没有内核版本的限制
MODULE_AUTHOR("xiaoan");

/*led的两个IO，网络是KP_COL0，VDD50_EN*/
/*蜂鸣器的1个IO，网络是MOTOR_PWM*/

/*矩阵键盘的8个IO，网络是CHG_FLT，HOOK_DET，CHG_UOK，XEINT14_BAK，
GM_INT1，6260_GPIO1，CHG_COK，XEINT29/KP_ROW13/ALV_DBG25*/

/*摄像头的14个IO，网络是CAM_MCLK，CAM2M_RST，CAM2M_PWDN，
CAM_D5，CAM_D7，CAM_D6，CAM_D4，CAM_D3，CAM_D2，CAM_D1，
CAM_PCLK，CAM_D0，CAM_VSYNC，CAM_HREF。
I2C_SDA7，I2C_SCL7也是可以设置为GPIO，不过总线一般不要去动它*/

/*WIFI模块的7个IO，WIFI_D3，WIFI_CMD，WIFI_D1，WIFI_CLK，WIFI_D0，WIFI_D2，GPC1_1*/
/*串口RX和TX等也是可以设置为GPIO，一般不要动它*/

/*数组中有32个引出到端子或者模块的IO，还有类似sd卡等也是可以作为GPIO，
其它引到连接器但是没有使用的GPIO等等*/
/*SCP管脚编号和POP的稍微有点不同，下面是SCP的*/
static int led_gpios[] = {
	EXYNOS4_GPL2(0),EXYNOS4_GPK1(1),
	EXYNOS4_GPD0(0),

	EXYNOS4_GPX1(0),EXYNOS4_GPX1(3),EXYNOS4_GPX1(5),EXYNOS4_GPX1(6),
	EXYNOS4_GPX3(0),EXYNOS4_GPX2(6),EXYNOS4_GPX2(7),EXYNOS4_GPX3(5),

	EXYNOS4212_GPJ1(3),EXYNOS4_GPL0(1),EXYNOS4_GPL0(3),EXYNOS4212_GPJ1(0),
	EXYNOS4212_GPJ1(2),EXYNOS4212_GPJ1(1),EXYNOS4212_GPJ0(7),EXYNOS4212_GPJ0(6),
	EXYNOS4212_GPJ0(5),EXYNOS4212_GPJ0(4),EXYNOS4212_GPJ0(0),EXYNOS4212_GPJ0(3),
	EXYNOS4212_GPJ0(1),EXYNOS4212_GPJ0(2),

	EXYNOS4_GPK3(6),EXYNOS4_GPK3(1),EXYNOS4_GPK3(4),EXYNOS4_GPK3(0),
	EXYNOS4_GPK3(3),EXYNOS4_GPK3(5),EXYNOS4_GPC1(1),
};

#define LED_NUM		ARRAY_SIZE(led_gpios)

static long hello_ioctl(struct file *files, unsigned int cmd, unsigned long arg){
  printk("cmd is %d,arg is %ld\n",cmd,arg);
  switch (cmd) {
    case 0:
    case 1:
      if(arg > LED_NUM)
      {
        return -EINVAL;
      }
      gpio_set_value(led_gpios[arg],cmd);
      break;

      default:
      return -EINVAL;
  }
  gpio_set_value(led_gpios[2],0);
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

static int hello_probe(struct platform_device *pdv)//->生成设备节点
{
  int ret,i;
  printk(DRIVER_NAME "\t initialized \n");
  //测试GPIO是否能用
  for(i = 0;i<LED_NUM;i++)
  {
  ret = gpio_request(led_gpios[i],"LEDS"); //linux GPIO申请
  if(ret)
  {
    printk("%s :request GPIO %d for LED failed ret = %d\n",DRIVER_NAME,i,ret);
    //printk(KERN_EMERG "gpio_request EXYNOS4_GPL2(0) failed\n");
  //  return ret;
  }
  else
  {
  s3c_gpio_cfgpin(led_gpios[i],S3C_GPIO_OUTPUT); //三星 GPIO 配置
  gpio_set_value(led_gpios[i],1);//linux 赋值函数
  }
  }
  gpio_set_value(led_gpios[2],0);//linux 赋值函数
  misc_register(&hello_dev);
  if(ret < 0 )
  {
    printk(KERN_EMERG "gpio_request EXYNOS4_GPL2(0) failed\n");
    goto exit;
  }
  return 0;
  exit:
	misc_deregister(&hello_dev);
	return ret;
	return 0;
}

static int hello_remove(struct platform_device *pdv)
{
  int i;
  printk(DRIVER_NAME "\t has removed\n");
  for (i=0; i <LED_NUM ;i++) {
    gpio_free(led_gpios[i]); //linux GPIO函数
  }
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
  printk(KERN_EMERG "HELLO WORLD EXIT\n");
  platform_driver_unregister(&hello_driver);
}
module_init(hello_init);
module_exit(hello_exit);

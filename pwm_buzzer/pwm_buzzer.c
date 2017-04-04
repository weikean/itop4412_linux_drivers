/*
  pwm = 定时器+定时器中断+IO输出
  GPIO-GPD0_0
  XpwmTOUT0  定时器0
  设置内容:
  XpwmTOUT0 = PRESCALER0+MUX+TCMPB0+TCNTB0
  步骤：
    1.开启自动重载
    2.设置TCNTB0和TCMPB0
    3.设置手动重载
    4.打开自动翻转，更新TCNTB0,TCMPB0寄存器
    5.再次设置TCNTB0,TCMPB0的值(双缓冲再次更新)
    6.开启定时器,通过设置TCON
    7.如果TCNTBn--,如果TCNTBn = TCMPBn，翻转
    8.TCNTn=0,产生中断
    9.自动重载第二次设置的值
    10.如果TCNTBn--,如果TCNTBn = TCMPBn，翻转
    11.TCNTn=0,产生中断
    12.再次自动重载，产生中断，然后关闭自动重载
    13.TCNTBn--,TCNTBn=TCMPBn,翻转
    14.关闭了自动重载，不再产生中断
    15.时钟停止

     配置GPIO pwm  0x02
    (预分频)PRESCALER0--TCFG0(1-255)--Frequency = PCLK/({prescaler value + 1})/{divider value}
    (分频)MUX-TCFG1(1,2,4,8,16)
    (设置占空比) TCMPB0和TCNTB0(--)
    设置自动重载，定时器开启(TCON)

*/
#include <linux/init.h>
#include <linux/module.h>

/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>
/*注册杂项设备头文件*/
#include <linux/miscdevice.h>
/*注册设备节点的文件结构体*/
#include <linux/fs.h>

#include <linux/kernel.h>

/*Linux中申请GPIO的头文件*/
#include <linux/gpio.h>
/*三星平台的GPIO配置函数头文件*/
/*三星平台EXYNOS系列平台，GPIO配置参数宏定义头文件*/
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
/*三星平台4412平台，GPIO宏定义头文件*/
#include <mach/gpio-exynos4.h>


//#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <asm/io.h>
#include <linux/regulator/consumer.h>

//#include "gps.h"
#include <linux/delay.h>

MODULE_LICENSE("Dual BSD/GPL");


struct {
    unsigned int    TCFG0;
    unsigned int    TCFG1;
    unsigned int    TCON;
    unsigned int    TCNTB0;
    unsigned int    TCMPB0;
    unsigned int    TCNTO0;
    unsigned int    TCNTB1;
    unsigned int    TCMPB1;
    unsigned int    TCNTO1;
    unsigned int    TCNTB2;
    unsigned int    TCMPB2;
    unsigned int    TCNTO2;
    unsigned int    TCNTB3;
    unsigned int    TCMPB3;
    unsigned int    TCNTO3;
    unsigned int    TCNTB4;
    unsigned int    TCNTO4;
    unsigned int    TINT_CSTAT;
}*PWM;

volatile unsigned long virt_addr,virt_addr_gpio,phys_addr,phys_addr_gpio;
//用于存放物理地址和虚拟地址
volatile unsigned long *GPD0CON,*GPD0PUD;

void addr_init(void)
{
	phys_addr = 0x139D0000;
	virt_addr = (unsigned long)ioremap(phys_addr,0x32);
	PWM = (unsigned long*)(virt_addr+0x00);

	phys_addr_gpio = 0x11400000+0xA0;
	virt_addr_gpio = (unsigned long)ioremap(phys_addr_gpio,0x10);
	GPD0CON = (unsigned long*)(virt_addr_gpio+0x00);
	GPD0PUD = (unsigned long*)(virt_addr_gpio+0x00A8-0x00A0);
}

void pwm_init(void)
{
	addr_init();
	*GPD0CON =  *GPD0CON&(~(0xf))|0x2;
	*GPD0PUD =  *GPD0PUD&(~(0xf));

	//Ԥ��Ƶ1-254 + 1
	(*PWM).TCFG0 = (*PWM).TCFG0 &(~(0xff))|0xf9;
	//��Ƶ1.2.4.8.16
	(*PWM).TCFG1 = (*PWM).TCFG1 &(~(0xf))|0x2;
	//����ռ�ձ�
	(*PWM).TCMPB0 = 50;
	(*PWM).TCNTB0 = 100;
	//�����ֶ�����,������ʱ��
	(*PWM).TCON = (*PWM).TCON & (~(0xf)) | 0x1 | 0x2;
}

static void beep_on(void)
{
  (*PWM).TCON = (*PWM).TCON & (~(0xf)) | 0x1 | 0x8;
}

static int iTop4412_PWM_init(void)
{
	pwm_init();
	beep_on();
	return 0;
}

static void beep_off(void)
{
  (*PWM).TCON = (*PWM).TCON & (~(0xf)) | 0X0;
  //定时器结束以后输出高还是低电平
  *GPD0CON = *GPD0CON&(~(0xf)) | 0x0;//设置为输入
}

static void iTop4412_PWM_exit(void)
{
	beep_off();
}

module_init(iTop4412_PWM_init);
module_exit(iTop4412_PWM_exit);

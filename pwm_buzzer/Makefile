#!/bin/bash

obj-m += pwm_buzzer.o
KDIR := /home/xiaoan/android4.0/iTop4412_Kernel_3.0

PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf *.o

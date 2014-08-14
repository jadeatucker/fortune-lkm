ifneq ($(KERNELRELEASE),)
	obj-m := fortune.o
else
	KERNELDIR ?= /usr/src/linux-headers-`uname -r`/

default:
		$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

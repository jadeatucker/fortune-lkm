obj-m := fortune.o
KERNELDIR ?= /usr/src/linux-headers-`uname -r`/

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

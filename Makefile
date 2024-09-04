# Compile module under Linux 2.6.x using something like:
# make -C /lib/modules/$(uname -r)/build M=$PWD modules
# make load
# gcc -o ioctl_test ioctl_test.c
# ./ioctl_test

obj-m += ioctl_module.o

build_module: ioctl_module.c
	make -C /usr/src/linux-`uname -r` SUBDIRS=$(PWD) modules

load: build_module
	insmod ioctl_module.ko

unload:
	rmmod ioctl_module.ko

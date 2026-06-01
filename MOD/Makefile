obj-m	:= hello_world.o hello_world_param.o hello_kthread.o consumer_producer.o
KDIR	?= /lib/modules/$(shell uname -r)/build
all: modules
install: modules_install
modules modules_install help clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) $(@)

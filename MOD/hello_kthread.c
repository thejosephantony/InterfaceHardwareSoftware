#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/types.h>
#include <linux/wait.h>
MODULE_AUTHOR("Bruno Prado");
MODULE_DESCRIPTION("Linux Kernel Thread example");
MODULE_LICENSE("GPL");
static int my_counter = 0;
static struct task_struct* my_kt;
static DECLARE_WAIT_QUEUE_HEAD(my_wait_queue);
static bool my_queue_condition = false;
static int my_kthread(void* parameter) {
	int* counter = (int*)(parameter);
	printk("my_kthread: waiting for event...\n");
	wait_event_interruptible(my_wait_queue, my_queue_condition);
	printk("my_kthread: starting loop counter = %u!\n", (*counter));
	while(!kthread_should_stop()) {
		(*counter)++;
		schedule();
		//ssleep(1);
	}
	printk("my_kthread: exiting\n");
	return (*counter);
}
static __init int hello_kthread_init(void) {
	int status = 0;
	printk("init: starting my_kthread!\n");
	my_kt = kthread_run(my_kthread, &my_counter, "My kthread");
	if(IS_ERR(my_kt)) {
		printk("init: error creating kthread!\n");
		status = PTR_ERR(my_kt);
	}
	msleep(100);
	my_queue_condition = true;
	printk("init: notifying my_kthread!\n");
	wake_up_interruptible(&my_wait_queue);
	return status;
}
static __exit void hello_kthread_exit(void) {
	printk("exit: stopping my_kthread!\n");
	printk("exit: counter = %u\n", kthread_stop(my_kt));
}
module_init(hello_kthread_init);
module_exit(hello_kthread_exit);

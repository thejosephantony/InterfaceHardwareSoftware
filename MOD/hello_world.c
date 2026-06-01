#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_AUTHOR("Bruno Prado");
MODULE_DESCRIPTION("Hello world module example");
MODULE_INFO(difficulty, "VERY EASY");
MODULE_LICENSE("GPL");
static __init int hello_world_init(void) {
	pr_info("Hello world!\n");
	return 0;
}
static __exit void hello_world_exit(void) {
	pr_info("Goodbye world!\n");
}
module_init(hello_world_init);
module_exit(hello_world_exit);

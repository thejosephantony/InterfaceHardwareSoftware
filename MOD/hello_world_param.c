#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
MODULE_AUTHOR("Bruno Prado");
MODULE_DESCRIPTION("Hello world module with parameters example");
MODULE_INFO(difficulty, "VERY EASY");
MODULE_LICENSE("GPL");
static int array[] = { 1, 2, 3 };
static char* string = "easy";
static int value = 123;
module_param_array(array, int, NULL, S_IRUGO);
module_param(string, charp, S_IRUGO);
module_param(value, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(array, "Integer array");
MODULE_PARM_DESC(string, "String message");
MODULE_PARM_DESC(value, "Integer value");
static __init int hello_world_init(void) {
	pr_info("Hello world with parameters!\n");
	pr_info("array = { %i, %i, %i }\n", array[0], array[1], array[2]);
	pr_info("string = %s\n", string);
	pr_info("value = %i\n", value);
	return 0;
}
static __exit void hello_world_exit(void) {
	pr_info("Goodbye world with parameters!\n");
}
module_init(hello_world_init);
module_exit(hello_world_exit);

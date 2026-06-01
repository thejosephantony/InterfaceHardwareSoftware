// Dispositivos de caractere
#include <linux/cdev.h>
// Inicialização e finalização
#include <linux/init.h>
// Operações em arquivo
#include <linux/fs.h>
// Definições de módulo
#include <linux/module.h>
// Funções basicas do núcleo
#include <linux/kernel.h>
// Semáforo
#include <linux/semaphore.h>
// Alocação dinâmica de memória
#include <linux/slab.h>
// Troca de dados (núcleo <-> usuário)
#include <linux/uaccess.h>
// Número de dispositivos
#define NUM_DEVICES 2
// Autor do módulo
MODULE_AUTHOR("Bruno Prado");
// Descrição do módulo
MODULE_DESCRIPTION("Consumer-producer example");
// Licença GPL
MODULE_LICENSE("GPL");
// Número principal (major)
static int DEVICE_MAJOR = 0;
// Declaração de protótipos de funções
static int consumer_producer_open(struct inode* inode, struct file* file);
static int consumer_producer_release(struct inode* inode, struct file* file);
static ssize_t consumer_producer_read(struct file* file, char __user* buffer, size_t count, loff_t* offset);
static ssize_t consumer_producer_write(struct file* file, const char __user* buffer, size_t count, loff_t* offset);
// Ligação da interface de operações por arquivos
static const struct file_operations consumer_producer_fops = {
    .owner   = THIS_MODULE,
    .open    = consumer_producer_open,
    .release = consumer_producer_release,
    .read    = consumer_producer_read,
    .write   = consumer_producer_write
};
// Estrutura do dispositivo
struct consumer_producer {
	// Dispositivo de caractere
	struct cdev cdev;
	// Estrutura de semáforo
	struct semaphore sema;
};
// Vetor de dispositivos consumidor-produtor
static struct consumer_producer cp_device[NUM_DEVICES];
// Classe de dispositivo (/sys/)
static struct class* consumer_producer_class = NULL;
static int consumer_producer_open(struct inode* inode, struct file* file) {
	printk("%s: open device\n", file->f_path.dentry->d_iname);
	return 0;
}
static int consumer_producer_release(struct inode* inode, struct file* file) {
	printk("%s: release device\n", file->f_path.dentry->d_iname);
	return 0;
}
static ssize_t consumer_producer_read(struct file* file, char __user* buffer, size_t count, loff_t* offset) {
	int dminor = 0;
	ssize_t nbytes = 0;
	char* output = NULL;
	size_t size = 0;
	ssize_t status = 0;
	if(*offset == 0) {
		printk("%s: read device\n", file->f_path.dentry->d_iname);
		dminor = iminor(file->f_path.dentry->d_inode);
		output = kzalloc(count, GFP_KERNEL);
		sprintf(output, "%u\n", cp_device[dminor].sema.count);
		size = strlen(output) + 1;
		nbytes = copy_to_user(buffer, output, size);
		*offset += size - nbytes;
		printk("%s: counter = %s",file->f_path.dentry->d_iname, output);
		kfree(output);
		status = *offset;
	}
	return status;
}
static ssize_t consumer_producer_write(struct file* file, const char __user* buffer, size_t count, loff_t* offset) {
	char* command = NULL; int dminor = 0;
	ssize_t nbytes = 0;
	ssize_t status = count;
	printk("%s: write device\n", file->f_path.dentry->d_iname);
	dminor = iminor(file->f_path.dentry->d_inode);
	command = kzalloc(count, GFP_KERNEL);
	nbytes = copy_from_user(command, buffer, count);
	if(strcmp(command, "consumer\n") == 0) {
		printk("%s: decrementing counter\n", file->f_path.dentry->d_iname);
		if(down_trylock(&cp_device[dminor].sema)) printk("Error while trying to acquire semaphore lock!\n");
	}
	else if(strcmp(command, "producer\n") == 0) {
		printk("%s: incrementing counter\n", file->f_path.dentry->d_iname);
		up(&cp_device[dminor].sema);
	}
	else {
		printk("Unrecognized command: %s", command);
		status = count - nbytes;
	}
	kfree(command);
	return status;
}
static __init int consumer_producer_init(void) {
	dev_t device;
	int i, status = 0;
	status = alloc_chrdev_region(&device, 0, NUM_DEVICES, "consumer_producer");
	DEVICE_MAJOR = MAJOR(device);
	consumer_producer_class = class_create("consumer_producer");
	for(i = 0; i < NUM_DEVICES; i++) {
		cdev_init(&cp_device[i].cdev, &consumer_producer_fops);
		cp_device[i].cdev.owner = THIS_MODULE;
		cdev_add(&cp_device[i].cdev, MKDEV(DEVICE_MAJOR, i), 1);
		device_create(consumer_producer_class, NULL, MKDEV(DEVICE_MAJOR, i), NULL, "consumer_producer_%d", i);
		sema_init(&cp_device[i].sema, 0);
	}
	return status;
}
static __exit void consumer_producer_exit(void) {
	int i;
	for(i = 0; i < NUM_DEVICES; i++)
		device_destroy(consumer_producer_class, MKDEV(DEVICE_MAJOR, i));
	class_unregister(consumer_producer_class);
	class_destroy(consumer_producer_class);
	unregister_chrdev_region(MKDEV(DEVICE_MAJOR, 0), MINORMASK);
}
module_init(consumer_producer_init);
module_exit(consumer_producer_exit);

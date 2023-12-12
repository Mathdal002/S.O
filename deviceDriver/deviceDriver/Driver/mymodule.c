#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

/*meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pedro Oliveira/linux");
MODULE_DESCRIPTION("HELLO WORLD LINUX add callback func");





static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - Open was called! \n");
	return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - Close was called! \n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};


// vai ser chamado quand o modulo for iniciado
static int __init ModuleInit(void) {
	int retval;
	printk("Hello , kernel aki !\n");
	/*Registrar dispositivo*/
	retval = register_chrdev(
	return 0;
}

// vai ser chamado quando o modulo for removido 
static void __exit ModuleExit(void) {
	printk("Goodbye, kernel \n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

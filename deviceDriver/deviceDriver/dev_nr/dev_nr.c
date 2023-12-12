#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

/*meta information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pedro Oliveira/linux");
MODULE_DESCRIPTION("Register device nr adn implemente some callbacks");


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

#define MYMAJOR 91


// vai ser chamado quand o modulo for iniciado
static int __init ModuleInit(void) {
	int retval;
	printk("Hello , kernel aki !\n");
	/*Registrar dispositivo*/
	retval = register_chrdev(MYMAJOR , "my_dev_nr",&fops);
	if (retval == 0) {
		printk("dev_nr - register Device number Major: %d,Minor: %d\n",MYMAJOR,0);
	}
	else if (retval > 0) {
		printk("dev_nr - register Device number Major: %d,Minor: %d\n",retval>>20,retval&0xfffff);
	}
	else {
		printk("could not register device number!!! \n");
		return -1;
	}
	return 0;
}

// vai ser chamado quando o modulo for removido 
static void __exit ModuleExit(void) {
	unregister_chrdev(MYMAJOR,"my_dev_nr");
	printk("Goodbye, kernel \n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

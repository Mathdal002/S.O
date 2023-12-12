#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pedro Oliveira/linux");
MODULE_DESCRIPTION("Registrar dispositivo número e implementar alguns callbacks");

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyModuleClass"
#define MAX_BUFFER_SIZE 255

static char buffer[MAX_BUFFER_SIZE + 1];  // +1 para o caractere nulo no final da string
static int buffer_pointer;

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


static ssize_t driver_read(struct file *file, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    to_copy = min(count, buffer_pointer);
    not_copied = copy_to_user(user_buffer, buffer, to_copy);
    delta = to_copy - not_copied;

    return delta;
}

static ssize_t driver_write(struct file *file, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    // Obter a quantidade de dados a serem copiados
    to_copy = min(count, sizeof(buffer));

    // Copiar dados do usuário para o buffer
    not_copied = copy_from_user(buffer, user_buffer, to_copy);

    // Aplicar cifra de César no buffer

    static int i = 0;
    for (i = 0; i < to_copy; i++) {
        if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z')) {
            buffer[i] += 3;  // Deslocamento de 3 posições no alfabeto
            if ((buffer[i] > 'Z' && buffer[i] <= 'Z' + 3) || buffer[i] > 'z') {
                buffer[i] -= 26;  // Voltar ao início do alfabeto se necessário
            }
        }
    }

    // Atualizar o ponteiro do buffer
    buffer_pointer = to_copy;

    // Calcular a quantidade de dados copiados
    delta = to_copy - not_copied;

    return delta;
}

static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_nr - Open foi chamado!\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_nr - Close foi chamado!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

static int __init ModuleInit(void) {
    int retval = 0;

    printk("Olá, kernel aki!\n");

    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Número do dispositivo não pôde ser alocado!\n");
        retval = -1;
        return -1;
    }

    printk("read_write - Número do dispositivo Major: %d, Minor: %d foi registrado!\n", MAJOR(my_device_nr), MINOR(my_device_nr));

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Classe do dispositivo não pode ser criada!\n");
        retval = -1;
        goto unregister_chrdev;
    }

    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Não é possível criar o arquivo do dispositivo!\n");
        retval = -1;
        goto class_destroy;
    }

    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) ==  -1) {
        printk("Registro do dispositivo no kernel falhou!\n");
        retval = -1;
        goto device_destroy;
    }

    return 0;

device_destroy:
    device_destroy(my_class, my_device_nr);
class_destroy:
    class_destroy(my_class);
unregister_chrdev:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}

static void __exit ModuleExit(void) {
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Adeus, kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


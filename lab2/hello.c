/* hello.c – Простой загружаемый модуль ядра */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define EOK 0

#define CODE_AMOUNT 1
#define MAX_TASK_LEN 29 /* 'Task:' = 6 +
                        PID = 5 + 
                        TASK_LEN = 16 + 
                        ' \n' = 2 */
#define MESSAGE_LENGTH 1024

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;
struct task_struct * p;

char * hello_str;
static int counter;
static int len;

void print_process_info(void);
static ssize_t hello_read(struct file *filp, char *buffer, 
    size_t length, loff_t * offset);
static int hello_release( struct inode *n, struct file *f );




void print_process_info(void) {

    char *buff;
    counter = 0;
    len = 0;

    hello_str = (char *)kmalloc(MESSAGE_LENGTH * sizeof(char), GFP_KERNEL);
    buff = (char *)kmalloc(MAX_TASK_LEN * sizeof(char), GFP_KERNEL);

    for_each_process(p)
    {
        if (len > MESSAGE_LENGTH - MAX_TASK_LEN - 1)
        {
            printk(KERN_INFO "Not enough memory in array\n");
            kfree(buff);

            return EOK; /* Without this line program falls */
        }

        sprintf(buff, "Task: %s %i\n", p->comm, (int)p->pid);
        len += strlen(buff);

        strcat(hello_str, buff);
    }

    kfree(buff);
    return EOK; /* Without this line program falls */
}

static ssize_t hello_read(struct file *filp, 
    char *buffer, 
    size_t length, 
    loff_t * offset)
{
    if (!(counter < len))
        return EOK;

    if (copy_to_user(buffer, hello_str + counter, sizeof(char)))
        return -EFAULT;

    counter++;

    return len - counter - 1;
}

static int hello_open( struct inode *n, struct file *f ) {

    print_process_info();

    return EOK;
}

static int hello_release( struct inode *n, struct file *f ) {

    return EOK;
}

static const struct file_operations hello_fops = {

    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .read  = hello_read,
};

static int __init hello_init(void) /* Инициализация */
{

    int retval;
    bool allocated = false;
    bool created = false;
    cl = NULL;

    retval = alloc_chrdev_region(&dev, 0, CODE_AMOUNT, "hello");
    if (retval)
        goto err;

    allocated = true;

    printk(KERN_INFO "Major number = %d Minor number = %d\n", 
        MAJOR(dev), MINOR(dev));

    cl = class_create(THIS_MODULE, "teach_devices");

    if (!cl) {
        retval = -1;
        goto err;
    }

    if (device_create(cl, NULL, dev, NULL, "hello") == NULL)
    {
        retval = -1;
        goto err;
    }
    created = true;

    cdev_init(&c_dev, &hello_fops);
    retval = cdev_add(&c_dev, dev, 1);
    if (retval)
        goto err;
    printk(KERN_INFO "Hello: regisered");
    return 0;
err:
    printk("Hello: initialization failed withcode 08h\n");

    if (created)
        device_destroy(cl, dev);
    if (allocated)
        unregister_chrdev_region(dev, CODE_AMOUNT);
    if (cl)
        class_destroy(cl);
    return retval;
}

static void __exit hello_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "Hello: unregistered\n");
    unregister_chrdev_region(dev, CODE_AMOUNT);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Gutnikov");
MODULE_DESCRIPTION("Simple loadable kernel module");

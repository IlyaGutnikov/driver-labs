#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include "hello.h"

#define PARALLEL_PORT 0x378
#define EOK 0

#define PORT_EMULATION

#ifdef PORT_EMULATION
#define inb(port) prn_data_em
#define outb(data, port) prn_data_em = ((unsigned char)data);
unsigned char prn_data_em;
#endif

#define MAX_MESSAGE_LENGTH 65536
#define START_MINOR_CODE 0
#define ALLOC_MINOR 1
#define UNREG_DEV 1

char message[MAX_MESSAGE_LENGTH];
int count;

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

static int hello_open(struct inode *inode, struct file *file);
static int hello_close(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char *buffer,
                       size_t length, loff_t * offset);
static ssize_t hello_write(struct file *filp, const char *buff,
                        size_t len, loff_t * off);

static long hello_ioctl(struct file *f, unsigned int cmd,
                        unsigned long arg);

static long hello_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int retval = -EINVAL;
    led_t led;

    unsigned char state;
    char mask = 0xff;
    int ret;

    switch (cmd) {

        case LEDCMD_GET_LED_STATE:

            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t))) {
                retval = -EACCES;
            }

            else if ((led.pin < 0) || (led.pin > 7))
                retval = -EINVAL;
            else {

                led.value = (inb(PARALLEL_PORT) >> led.pin) & 1;

                ret = copy_to_user((led_t *)arg, &led, sizeof(led_t));

                if(ret)
                    retval = -EACCES;
                else
                    retval = EOK;
            }
            break;

        case LEDCMD_GET_STATE:

            state = inb(PARALLEL_PORT);

            if(copy_to_user((unsigned char *)arg, &state, sizeof(state)))
                retval = -EACCES;
            else
                retval = EOK;
            break;

        case LEDCMD_RESET_STATE:
            outb(INITIAL_STATE, PARALLEL_PORT);
            retval = EOK;
            break;

        case LEDCMD_SET_LED_STATE:
            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t)))
                retval = -EACCES;
            else
                if ((led.pin < 0) || (led.pin > 7))
                    retval = -EINVAL;
                else {
                    state = inb(PARALLEL_PORT);

                    /* mask = 0b00010000 */
                    mask = (1 << led.pin);

                    state &= ~ mask;

                    mask = (led.value << led.pin);

                    state |= mask;

                    outb(state, PARALLEL_PORT);
                    retval = 0;
                }
            break;

        default:
            printk("Bad ioctl command!\n");
            break;
    }

    return retval;
}

static int hello_open(struct inode *inode, struct file *file) {
    return EOK;
}

static ssize_t hello_read(struct file *filp, char *buffer,
                       size_t length, loff_t * offset) {
    return EOK;
}

static ssize_t hello_write(struct file *filp, const char *buff,
                        size_t len, loff_t * off) {
        return -EINVAL;
}

static int hello_close(struct inode *inode, struct file *file) {
    return EOK;
}

static struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_close,
    .read = hello_read,
    .write = hello_write,
    .unlocked_ioctl = hello_ioctl
};

static int __init hello_init(void) {
    int retval;
    bool allocated = false;
    bool created = false;
    cl = NULL;

    retval = alloc_chrdev_region(&dev, START_MINOR_CODE,
                                 ALLOC_MINOR, "hello");
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

    if (device_create(cl, NULL, dev, NULL, "hello") == NULL) {
        retval = -1;
        goto err;
    }

    created = true;
    cdev_init(&c_dev, &hello_fops);
    retval = cdev_add(&c_dev, dev, 1);
    if (retval)
        goto err;

    printk(KERN_INFO "Hello: regisered\n");
    return EOK;

err:
    printk(KERN_INFO "Hello: initialization failed with code %d\n",
           retval);
    if (created)
        device_destroy(cl, dev);

    if (allocated)
        unregister_chrdev_region(dev, UNREG_DEV);

    if (cl)
        class_destroy(cl);

    return retval;
}


static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, UNREG_DEV);
    class_destroy (cl);
}



module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Gutnikov");
MODULE_DESCRIPTION("Simple loadable kernel module");

/* char_driver.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#define DEVICE_NAME "char_dev"
#define CLASS_NAME  "char_class"

static int major_number;
static char message[256] = {0};
static short message_size;
static struct kparam_string message_param = {
    .maxlen = sizeof(message),
    .string = message,
};

static int message_param_set(const char *val, const struct kernel_param *kp)
{
    int ret = param_set_copystring(val, kp);

    if (ret == 0)
        message_size = strnlen(message, sizeof(message));

    return ret;
}

static const struct kernel_param_ops message_ops = {
    .set = message_param_set,
    .get = param_get_string, /* Use default getter */
};

module_param_cb(message, &message_ops, &message_param, 0644);
MODULE_PARM_DESC(message, "Message exposed by the char driver");

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    size_t bytes_to_read;
    int error_count;

    if (*offset > 0 || message_size == 0)
        return 0;

    bytes_to_read = min(len, (size_t)message_size);
    /* This is required because kernel code cannot safely write directly into a user-space pointer. */
    error_count = copy_to_user(buffer, message, bytes_to_read);

    if (error_count == 0) {
        *offset = bytes_to_read;
        printk(KERN_INFO "Sent %zu characters to the user\n", bytes_to_read);
        return bytes_to_read;
    } else {
        printk(KERN_INFO "Failed to send %d characters\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep,
                         const char __user *buffer,
                         size_t len,
                         loff_t *offset)
{
    size_t to_copy = min(len, sizeof(message) - 1);

    if (copy_from_user(message, buffer, to_copy)) {
        printk(KERN_ERR "Failed to copy from user\n");
        return -EFAULT;
    }

    message[to_copy] = '\0';
    message_size = to_copy;

    printk(KERN_INFO "Received %zu characters from the user\n", to_copy);

    return to_copy;
}

static int __init char_init(void)
{
    printk(KERN_INFO "Char: Initializing\n");
    message_size = strnlen(message, sizeof(message));

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    printk(KERN_INFO "Registered correctly with major number %d\n", major_number);
    return 0;
}

static void __exit char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char: Goodbye\n");
}


module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel");
MODULE_DESCRIPTION("Linux char driver");
MODULE_VERSION("0.1");

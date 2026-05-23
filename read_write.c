#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>

static char *text=NULL; // kernel buffer to write after reading from user

static ssize_t data_size =0; //size of data to be read/write

static ssize_t buff_size =0; //kernel buffer size

static int major;

static ssize_t my_read(struct file *filp, char __user *buff, size_t len, loff_t *off)
{
    size_t to_copy,no_copy,delta;

    printk(KERN_INFO "READ entered offset=%lld data=%zu\n",*off,data_size);

    if(*off >= data_size)
    return 0;

    to_copy = (len + *off)<data_size?len:(data_size - *off); 

     printk(KERN_INFO "READ requested=%zu copied=%zu offset=%lld\n",len, to_copy,*off);

    no_copy = copy_to_user(buff, text+*off,to_copy);

    delta = to_copy - no_copy;

    *off += delta;
    printk("Successfully read %zu bytes\n",delta);

    return delta;
}

static ssize_t my_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    size_t no_copy, delta;

    if(len + *off > buff_size)
    {
        char *temp = krealloc(text, (len + *off), GFP_KERNEL);
        if(!temp)
        return -ENOMEM;

        text = temp;
        buff_size = len + *off;
    }

    printk(KERN_INFO "WRITE requested=%zu offset=%lld\n",len,*off);

    no_copy = copy_from_user(text + *off, buff, len);

    delta = len-no_copy;

    *off += delta;

    if(*off > data_size)
    data_size = *off;

    printk(KERN_INFO "Successfully wrote %zu bytes\n",delta);

    return delta;
}


static int my_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "File opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "File closed\n");
    return 0;
}

static struct file_operations fops ={
    .read=my_read,
    .write=my_write,
    .open=my_open,
    .release=my_release,
    .llseek=default_llseek,
};

static int __init my_init(void)
{
    major = register_chrdev(0,"read_write_dynamic",&fops);

    if(major<0){
    printk(KERN_INFO "Unable to register the device in kernel");}
    else{
        printk(KERN_INFO "Registered successfully and the major number is: %d",major);
    }

    return 0;
}

static void __exit my_exit(void)
{
    kfree(text);
    unregister_chrdev(major,"read_write_dynamic");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
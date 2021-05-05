#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 32
#define PROC_NAME "jiffies"

ssize_t proc_read(struct file *file, char __user *usr_buf, 
        size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

int proc_init(void){
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);

    return 0;
}

void proc_exit(void){
    remove_proc_entry(PROC_NAME, NULL);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, 
        size_t count, loff_t *pos){
    static unsigned completed = 0;
    int rv = 0;
    char buffer[BUFFER_SIZE];
    
    if(completed){
        completed = 0;
        return 0;
    }

    rv = sprintf(buffer, "%lu\n", jiffies);
    copy_to_user(usr_buf, buffer, rv);
    completed = 1;
    return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Display $jiffies to /proc/jiffies");
MODULE_AUTHOR("purewhite");

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 512
#define PROC_NAME "pid"

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static pid_t pid = -1;
static int valid = 0;

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write
};

static int proc_init(void){
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "Proc inited.\n");
    return 0;
}

static void proc_exit(void){
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Proc exited.\n");
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos){
    static unsigned completed = 0;
    int rv = 0;
    char buffer[BUFFER_SIZE];
    
    if(completed){
        completed = 0;
        return 0;
    }

    if(valid == 0)
        rv = sprintf(buffer, "Error: PID not loaded.\n");
    else{
        struct task_struct* info;
        if((info = pid_task(find_vpid(pid), PIDTYPE_PID)) == NULL)
            rv = sprintf(buffer, "Error: PID information access failed, maybe invalid PID.\n");
        else{
            rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%ld]\n", info->comm, info->pid, info->state);
        }
    }
    copy_to_user(usr_buf, buffer, rv);
    completed = 1;
    return rv;
}

static ssize_t proc_write(struct file *file, char __user *usr_buf, size_t count, loff_t *pos){
    int status = 0;
    char *k_mem;

    k_mem = kmalloc(count, GFP_KERNEL);
    copy_from_user(k_mem, usr_buf, count);
    // status  = kstrtol(k_mem, 10, &pid);
    status = sscanf(k_mem, "%d", &pid);
    if(pid > 0 && status > 0){
        printk(KERN_INFO "PID initialized: %d\n", pid);
        valid = 1;
    }
    else{
        printk(KERN_INFO "Invalid PID. Transformation status: %d\n", pid, status);
        valid = 0;
    }
    kfree(k_mem);

    return count;
}


module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Display process information of a given PID");
MODULE_AUTHOR("purewhite");

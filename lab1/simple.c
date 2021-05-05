#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <asm/param.h>
#include <linux/jiffies.h>

u64 init_time = 0;

int simple_init(void){
    printk(KERN_INFO "Loading Kernel Module\n");
    printk(KERN_INFO "%lu\n", GOLDEN_RATIO_PRIME);
    printk(KERN_INFO "jiffies: %llu, HZ: %d\n", (init_time = jiffies), HZ);
    return 0;
}

void simple_exit(void){
    printk(KERN_INFO "%lu\n", gcd(3300ul, 24ul));
    printk(KERN_INFO "Removing Kernel Module\n");
    printk(KERN_INFO "jiffies: %lu\n", jiffies);
    printk(KERN_INFO "Total running time: %llu\n", (jiffies - init_time) / HZ);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("purewhite");

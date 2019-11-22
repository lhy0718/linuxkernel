#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>

int *ptr;
typedef struct{
	int old;
	int new;
}args;

int th(void *_a){
	args *a;
	a = (args *)_a;
	__sync_val_compare_and_swap(ptr, a->old, a->new);
	printk("value: %d\n", *ptr);
	return 0;
}

int __init _module_init(void){
	args a, b, c, d;
	ptr = (int *)kmalloc(sizeof(int), GFP_KERNEL);
	*ptr = a.old = 0;
	a.new = b.old = 1; b.new = c.old = 2; c.new = d.old = 3; d.new = 4;

	kthread_run(&th, &a, "th1");
	kthread_run(&th, &b, "th2");
	kthread_run(&th, &c, "th3");
	kthread_run(&th, &d, "th4");
	mdelay(20);
	return 0;
}

void __exit _module_cleanup(void){
}

module_init(_module_init);
module_exit(_module_cleanup);

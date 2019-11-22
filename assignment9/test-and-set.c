#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>

int *intptr;

int th(void *_p){
	int *p;
	p = (int *)_p;
	__sync_lock_test_and_set(intptr, *p);
	printk("value: %d\n", *intptr);
	return 0;
}

int __init _module_init(void){
	int a=1, b=2, c=3, d=4;
	intptr = (int *)kmalloc(sizeof(int), GFP_KERNEL);
	*intptr = 0;

	kthread_run(&th, &a, "thread1");
	kthread_run(&th, &b, "thread2");
	kthread_run(&th, &c, "thread3");
	kthread_run(&th, &d, "thread4");
	mdelay(20);
	return 0;
}

void __exit _module_cleanup(void){
	printk("module_exit\n");
}

module_init(_module_init);
module_exit(_module_cleanup);
MODULE_LICENSE("GPL");

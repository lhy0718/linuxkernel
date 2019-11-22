#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>

int n=0;
int test_thread(void *_num){
	int *num = (int *)_num;
	__sync_fetch_and_add(num, 1);
	printk("num: %d\n", *num);
	return 0;
}

int thread_create(void){
	int i=0;
	for (; i<4; i++){
		kthread_run(&test_thread, (void*)&n, "test_thread");
	}
	return 0;
}

int __init _module_init(void) {
	thread_create();
	return 0;
}

void __exit _module_cleanup(void){
}

module_init(_module_init);
module_exit(_module_cleanup);

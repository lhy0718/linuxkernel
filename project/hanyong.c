#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "custom_rbtree.h"
#include "custom_timer.h"

void test(void){
	TIMER_START;
	TIMER_END;
	printk("timer : %ld\n", TIMER); 
}

int __init _module_init(void){
	test();
	return 0;
}

void __exit _module_cleanup(void){

}

module_init(_module_init);
module_exit(_module_cleanup);

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#define TIMER_START long TIMER; struct timespec start, end, difftime; getnstimeofday(&start);
#define TIMER_END getnstimeofday(&end); timespec_diff(&start, &end, &difftime); TIMER = difftime.tv_nsec;
#define WHEN_FOUR_KTHREADS_STOPED while(ts[0]->state<=0||ts[1]->state<=0||ts[2]->state<=0||ts[3]->state<=0){\
		ndelay(100);\
	}
	

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result) {
	if ((stop->tv_nsec - start->tv_nsec) < 0) {
		result->tv_sec = stop->tv_sec - start->tv_sec - 1;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
	} else {
		result->tv_sec = stop->tv_sec - start->tv_sec;
		result->tv_nsec = stop->tv_nsec - start->tv_nsec;
	}
	return;
}

typedef struct{
	struct list_head list;
	int data;
}my_node;

spinlock_t sl;
struct rw_semaphore rwse;
struct mutex mt;
struct list_head *pos, *temp;

static int sl_insert_thread(void *arg){
	int loop=0;
	struct list_head *list = (struct list_head *)arg;
	for(;loop<25000;loop++){
		my_node *new = kmalloc(sizeof(my_node), GFP_KERNEL);
		spin_lock(&sl);
		new->data = loop;
		list_add(&new->list, list);
		spin_unlock(&sl);
	}
	do_exit(0);
}

void sl_insert(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&sl_insert_thread, list, "sl_insert_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Spinlock linked list insert time :\t%ld ns\n", TIMER);
}

static int sl_search_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	spin_lock(&sl);
	list_for_each(pos, list){
		cur_node = list_entry(pos, my_node, list);
	}
	spin_unlock(&sl);
	do_exit(0);
}

void sl_search(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&sl_search_thread, list, "sl_search_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Spinlock linked list search time :\t%ld ns\n", TIMER);
}

static int sl_delete_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	spin_lock(&sl);
	list_for_each_safe(pos, temp, list){
			cur_node = list_entry(pos, my_node, list);
			list_del(&cur_node->list);
			kfree(cur_node);
	}
	spin_unlock(&sl);
	do_exit(0);
}

void sl_delete(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&sl_delete_thread, list, "sl_delete_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Spinlock linked list delete time :\t%ld ns\n", TIMER);
}

static int mt_insert_thread(void *arg){
	int loop=0;
	struct list_head *list = (struct list_head *)arg;
	for(;loop<25000;loop++){
		my_node *new = kmalloc(sizeof(my_node), GFP_KERNEL);
		mutex_lock(&mt);
		new->data = loop;
		list_add(&new->list, list);
		mutex_unlock(&mt);
	}
	do_exit(0);
}

void mt_insert(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&mt_insert_thread, list, "mt_insert_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Mutex linked list insert time :\t%ld ns\n", TIMER);
}

static int mt_search_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	mutex_lock(&mt);
	list_for_each(pos, list){
		cur_node = list_entry(pos, my_node, list);
	}
	mutex_unlock(&mt);
	do_exit(0);
}

void mt_search(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&mt_search_thread, list, "mt_search_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Mutex linked list search time :\t%ld ns\n", TIMER);
}

static int mt_delete_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	mutex_lock(&mt);
	list_for_each_safe(pos, temp, list){
			cur_node = list_entry(pos, my_node, list);
			list_del(&cur_node->list);
			kfree(cur_node);
	}
	mutex_unlock(&mt);
	do_exit(0);
}

void mt_delete(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&mt_delete_thread, list, "mt_delete_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Mutex linked list delete time :\t%ld ns\n", TIMER);
}
static int rwse_insert_thread(void *arg){
	int loop=0;
	struct list_head *list = (struct list_head *)arg;
	for(;loop<25000;loop++){
		my_node *new = kmalloc(sizeof(my_node), GFP_KERNEL);
		down_write(&rwse);
		new->data = loop;
		list_add(&new->list, list);
		up_write(&rwse);
	}
	do_exit(0);
}

void rwse_insert(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&rwse_insert_thread, list, "rwse_insert_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Read Write Semaphore linked list insert time :\t%ld ns\n", TIMER);
}

static int rwse_search_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	down_read(&rwse);
	list_for_each(pos, list){
		cur_node = list_entry(pos, my_node, list);
	}
	up_read(&rwse);
	do_exit(0);
}

void rwse_search(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&rwse_search_thread, list, "rwse_search_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Read Write Semaphore linked list search time :\t%ld ns\n", TIMER);
}

static int rwse_delete_thread(void *arg){
	struct list_head *list = (struct list_head *)arg;
	my_node *cur_node;
	down_write(&rwse);
	list_for_each_safe(pos, temp, list){
			cur_node = list_entry(pos, my_node, list);
			list_del(&cur_node->list);
			kfree(cur_node);
	}
	up_write(&rwse);
	do_exit(0);
}

void rwse_delete(struct list_head *list){
	int loop=0;
	struct task_struct* ts[4];
	TIMER_START;
	for(;loop<4;loop++){
		ts[loop]=kthread_run(&rwse_delete_thread, list, "rwse_delete_thread");
	}
	WHEN_FOUR_KTHREADS_STOPED;
	TIMER_END;
	printk("Read Write Semaphore linked list delete time :\t%ld ns\n", TIMER);
}
void example(void){
	struct list_head sl_list, mt_list, rwse_list;
	INIT_LIST_HEAD(&sl_list);
	INIT_LIST_HEAD(&mt_list);
	INIT_LIST_HEAD(&rwse_list);

	sl_insert(&sl_list);
	sl_search(&sl_list);
	sl_delete(&sl_list);
	mt_insert(&mt_list);
	mt_search(&mt_list);
	mt_delete(&mt_list);
	rwse_insert(&rwse_list);
	rwse_search(&rwse_list);
	rwse_delete(&rwse_list);
}

int __init list_module_init(void){
	spin_lock_init(&sl);
	init_rwsem(&rwse);
	mutex_init(&mt);

	example();
	return 0;
}

void __exit list_module_cleanup(void){

}

module_init(list_module_init);
module_exit(list_module_cleanup);
MODULE_LICENSE("GPL");

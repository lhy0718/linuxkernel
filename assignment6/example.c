#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
struct my_node{
	struct list_head list;
	int data;
};

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

void add_list(struct list_head* list, int num){
	int i;
	struct timespec start, end, difftime;
	getnstimeofday(&start);
	for(i=0; i<num; i++){
		struct my_node* newnode = kmalloc(sizeof(struct my_node),GFP_KERNEL);
		newnode->data = i;
		list_add(&newnode->list, list);
	}
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("add_list %d: %lld.%.9ld\n", num, (long long)difftime.tv_sec, difftime.tv_nsec);
}

void search_list(struct list_head* list, int num){
	struct timespec start, end, difftime;
	struct list_head *p;
	getnstimeofday(&start);
	list_for_each(p, list){};
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("search_list %d: %lld.%.9ld\n", num, (long long)difftime.tv_sec, difftime.tv_nsec);
}

void delete_list(struct list_head* list, int num){
	struct timespec start, end, difftime;
	struct my_node *tmp, *current_node;
	getnstimeofday(&start);
	list_for_each_entry_safe(current_node, tmp, list, list){
       		list_del(&current_node->list);
		kfree(current_node);
	}
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("delete_list %d: %lld.%.9ld\n", num, (long long)difftime.tv_sec, difftime.tv_nsec);
}

void example(void){
	struct list_head list_1000;
	struct list_head list_10000;
	struct list_head list_100000;
	INIT_LIST_HEAD(&list_1000);
	INIT_LIST_HEAD(&list_10000);
	INIT_LIST_HEAD(&list_100000);
	
	add_list(&list_1000, 1000);
	add_list(&list_10000, 10000);
	add_list(&list_100000, 100000);
	
	search_list(&list_1000, 1000);
	search_list(&list_10000, 10000);
	search_list(&list_100000, 100000);

	delete_list(&list_1000, 1000);
	delete_list(&list_10000, 10000);
	delete_list(&list_100000, 100000);
}

int __init list_module_init(void){
	example();
	return 0;
}

void __exit list_module_cleanup(void){

}

module_init(list_module_init);
module_exit(list_module_cleanup);

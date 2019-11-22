#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h> //for kmalloc
#include <linux/time.h>

#define FALSE 0
#define TRUE 1

struct my_type{
	struct rb_node node;
	int key;
	int value;
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

int rb_insert(struct rb_root *root, struct my_type *data) {
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	/* Figure out "where" to put new node */ 
	while (*new) {
		struct my_type *this = container_of(*new, struct my_type, node);
		parent = *new;
		if (this->key > data->key)
			new = &((*new)->rb_left); 
		else if (this->key < data->key)
			new = &((*new)->rb_right);
		else
		       return FALSE;	
	}
	rb_link_node(&data->node, parent, new); 
	rb_insert_color(&data->node, root);
	return TRUE;
}

struct my_type *rb_search(struct rb_root *root, int key){
	struct rb_node *node = root->rb_node;

	while (node){
		struct my_type *data = container_of(node, struct my_type, node);
		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node=node->rb_right;
		else
			return data;
	}
	return NULL;
}

int rb_delete(struct rb_root *mytree, int key){
	struct my_type *data = rb_search(mytree, key);
	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
	}
	return TRUE;
}

void rb_insert_iterate(struct rb_root *my_tree, int n){
	int i, ret;
	struct timespec start, end, difftime;
	getnstimeofday(&start);
	for(i=0; i<n; i++){
		struct my_type *new = kmalloc(sizeof(struct my_type),GFP_KERNEL);
		if(!new)
			return NULL;
		new->value=i*10;
		new->key=i;

		ret = rb_insert(my_tree, new);
	}
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("rb_insert_iteration %d:\t%9ld nanoseconds\n", n, difftime.tv_nsec);
}

void rb_traversal(struct rb_root *my_tree, int n){
	struct timespec start, end, difftime;
	getnstimeofday(&start);
	struct rb_node *node;
	for (node = rb_first(my_tree); node; node=rb_next(node)){
		int temp=rb_entry(node, struct my_type, node)->key;
		if(temp>10){}
	}
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("rb_traversal and search %d:\t%9ld nanoseconds\n", n, difftime.tv_nsec);
}

void rb_delete_iterate(struct rb_root *my_tree, int n){
	int i=0;
	struct timespec start, end, difftime;
	getnstimeofday(&start);
	for (;i<n;i++){
		rb_delete(my_tree, i);
	}
	getnstimeofday(&end);
	timespec_diff(&start, &end, &difftime);
	printk("rb_delete_iteration %d:\t%9ld nanoseconds\n", n, difftime.tv_nsec);
}

void example(void){
	struct rb_root tr1000 = RB_ROOT;
	struct rb_root tr10000 = RB_ROOT;
	struct rb_root tr100000 = RB_ROOT;
	rb_insert_iterate(&tr1000, 1000);
	rb_insert_iterate(&tr10000, 10000);
	rb_insert_iterate(&tr100000, 100000);
	rb_traversal(&tr1000, 1000);	
	rb_traversal(&tr10000, 10000);	
	rb_traversal(&tr100000, 100000);
	rb_delete_iterate(&tr1000, 1000);	
	rb_delete_iterate(&tr10000, 10000);	
	rb_delete_iterate(&tr100000, 100000);	
}

int __init list_module_init(void){
	example();
	return 0;
}

void __exit list_module_cleanup(void){

}

module_init(list_module_init);
module_exit(list_module_cleanup);

#ifndef H_QUEUE_H_
#define H_QUEUE_H_

#define QUEUE_FOREACH(var, hd)					\
	for((var)=(hd)->head; (var); (var)=(var)->next)

#define QUEUE_COUNT(hd)	((hd)->count)
	
#define QUEUE_IS_EMPTY(hd)	(NULL==(hd)->head)

#define QUEUE_SET_MAX(hd, theMax)			\
	do {					\
		if((theMax)>0)			\
			(hd)->max=(theMax);	\
	} while(0)
	
#define QUEUE_IS_FULL(hd)	((hd)->max>0&&QUEUE_COUNT(hd)>=(hd)->max)

#define DELETE_QUEUE(queue)			\
	do {					\
		if((queue)) {			\
			delete_queue((queue));	\
			(queue)=NULL;		\
		}				\
	} while(0)

#define NEW_QUEUE_ITEM(new_item, the_data, queue, the_next)			\
	do {					\
		MALLOC_SAFE(new_item, sizeof(struct queue_node_t));	\
		(new_item)->data=(the_data);	\
		(new_item)->next=(the_next);	\
		QUEUE_COUNT(queue)++;	\
	} while(0)

/* queue node */
struct queue_node_t {
	void *data;
	struct queue_node_t *next;
};

/* queue */
struct queue_t {
	struct queue_node_t *head;
	struct queue_node_t *tail;
	unsigned long count;
	unsigned long max;
	int (*compar)(const void *, const void *);
};

void enqueue_unique_to_twin_queues
(twin_queues twin_q, 
struct queue_node_t* fixed_name_prev,
struct queue_node_t* can_prev, 
void* item,
int prot);

void delete_queue(struct queue_t *hd);

int enqueue_unique_to_sorted_queue
(struct queue_t *hd, struct queue_node_t *start,
 struct queue_node_t **found, void *data);

struct queue_t * new_queue(unsigned long max, 
	int (*compar)(const void *, const void *));

int enqueue_to_hash
(twin_queues hash[MAXN*MAXN], 
 struct queue_node_t* fixed_name_start,
 struct queue_node_t* can_start,
 protocol_state_t* s,
 struct queue_node_t** found, 
 int prot);

int search_in_twin_queues 
(twin_queues twin_q, 
 struct queue_node_t* fixed_name_start,
 struct queue_node_t* can_start,
 struct queue_node_t** fixed_name_prev,
 struct queue_node_t** can_prev,
 struct queue_node_t** found,  
 child_t* child, int prot);
	
#endif /* H_QUEUE_H_ */

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

#define NEW_QUEUE_ITEM(newItem, theData, queue, theNext)			\
	do {					\
		MALLOC_SAFE(newItem, sizeof(struct queue_node_t));	\
		(newItem)->data=(theData);	\
		(newItem)->next=(theNext);	\
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
(twin_queues twin_q, struct queue_node_t** found, void* item,
int protocol_name);

void delete_queue(struct queue_t *hd);

int search_in_sorted_queue
	(struct queue_t *hd, struct queue_node_t **found, const void *data);

struct queue_t * new_queue(unsigned long max, 
	int (*compar)(const void *, const void *));

int enqueue_unique_to_sorted_queue(struct queue_t *hd,
	struct queue_node_t **dataPtr, void *data);

int enqueue_to_hash
(twin_queues hash[MAXN*MAXN], protocol_state_t* s,
 struct queue_node_t** found, int protocol_name);

	
#endif /* H_QUEUE_H_ */

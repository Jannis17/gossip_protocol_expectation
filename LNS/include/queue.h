#ifndef H_QUEUE_H_
#define H_QUEUE_H_

#define QUEUE_FOREACH(var, hd)					\
	for((var)=(hd)->head; (var); (var)=(var)->next)

#define QUEUE_COUNT(hd)		((hd)->count)
	
#define QUEUE_IS_EMPTY(hd)	(NULL==(hd)->head)

#define DELETE_QUEUE(queue)			\
	do {					\
		if((queue)) {			\
			delete_queue((queue));	\
			(queue)=NULL;		\
		}				\
	} while(0)


#define NEW_QUEUE_ITEM(newItem, data, hd, p)			\
	do {					\
		MALLOC_ITEMS(newItem, 1, struct queue_node_t);	\
		(newItem)->data= (data);	\
		(newItem)->next= (p);	\
		QUEUE_COUNT(hd)++;	\
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
	int (*compar)(const void *, const void *);
};


void delete_queue(struct queue_t *);
unsigned long reset_queue(struct queue_t *);

int enqueue_unique_to_sorted_queue
	(struct queue_t *, struct queue_node_t **, const void *);

#endif /* H_QUEUE_H_ */

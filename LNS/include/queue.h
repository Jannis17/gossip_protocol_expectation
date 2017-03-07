/* queue.h */

#ifndef H_QUEUE_H_
#define H_QUEUE_H_


#define QUEUE_FOREACH(var, hd)					\
	for((var)=(hd)->head; (var); (var)=(var)->next)
#define QUEUE_FOREACH_START(var, node)				\
	for((var)=(node); (var); (var)=(var)->next)

#define QUEUE_SET_MAX(hd, max)			\
	do {					\
		if((max)>0)			\
			(hd)->max=(max);	\
	} while(0)
#define QUEUE_IS_EMPTY(hd)	(NULL==(hd)->head)
#define QUEUE_COUNT(hd)		((hd)->count)
#define QUEUE_IS_FULL(hd)	((hd)->max>0&&QUEUE_COUNT(hd)>=(hd)->max)

#define DELETE_QUEUE(queue)			\
	do {					\
		if((queue)) {			\
			delete_queue((queue));	\
			(queue)=NULL;		\
		}				\
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
	int (*compar)(const void *, const void *);
	void (*print_data_fun)(FILE *fp, const void *data);
	unsigned long count;
	unsigned long max;
};


unsigned long		count_queue_data(const struct queue_t *hd,
				const void *data);
void			delete_queue(struct queue_t *hd);
int			dequeue_from_queue(struct queue_t *hd, void **data);
int			enqueue_to_queue(struct queue_t *hd, void *data);
int			enqueue_to_queue_unique(struct queue_t *hd,
				void *data);
int			fprint_queue(FILE *fp, const struct queue_t *hd);
struct queue_t *	new_queue(const unsigned long max,
				int (*compar)(const void *, const void *),
				void (*print_data_fun)(
					FILE *fp, const void *
				));
struct queue_t *	new_queue_from_array(void **data,
				const unsigned long numdata,
				const unsigned long max,
				int (*compar)(const void *, const void *),
				void (*print_data_fun)(
					FILE *fp, const void *
				));
void **			queue_to_array(const struct queue_t *hd,
				unsigned long *siz);
unsigned long		reset_queue(struct queue_t *hd);
unsigned long		run_queue_prog(struct queue_t *hd,
				void (*prog)(void **, void *), void *param);
int			search_queue(struct queue_t *hd,
				struct queue_node_t **found,
				struct queue_node_t **prev,
				struct queue_t **all,
				int *where,
				const void *data);
size_t			sizeof_queue(const struct queue_t *hd);
int			swap_queue_data(struct queue_t *hd,
				struct queue_node_t *node1,
				struct queue_node_t *node2);


/*
 * get_queue_head_data: get head data (accessor)
 */
static inline void *
get_queue_head_data(const struct queue_t *hd)
{
	if(hd->head)
		return hd->head->data;

	return NULL;
}

/*
 * get_queue_tail_data: get tail data (accessor)
 */
static inline void *
get_queue_tail_data(const struct queue_t *hd)
{
	if(hd->tail)
		return hd->tail->data;

	return NULL;
}


#endif /* H_QUEUE_H_ */

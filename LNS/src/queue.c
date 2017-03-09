#include "memory.h"
#include "queue.h"

/*
 * dequeue_from_queue: dequeues an item from queue
 *   hd: queue (this)
 *   data: dequeued item (optional)
 *
 * returns 1 if successful, 0 otherwise
 */
int
dequeue_from_queue(struct queue_t *hd, void **data)
{
	struct queue_node_t *p;
	void *n;

	if(QUEUE_IS_EMPTY(hd)) {
		if(data)
			*data=NULL;

		return 0;
	}

	n=hd->head->data;
	p=hd->head;
	hd->head=hd->head->next;
	free(p);

	hd->count--;

	if(data)
		*data=n;

	return 1;
}


/*
 * reset_queue: resets a queue
 *   hd: queue (this)
 * returns queue length
 */
unsigned long
reset_queue(struct queue_t *hd)
{
	unsigned long i;

	if(!hd)
		return 0;

	i=0;
	while(!QUEUE_IS_EMPTY(hd)) {
		dequeue_from_queue(hd, NULL);
		i++;
	}
	hd->head=NULL;
	hd->count=0;

	return i;
}

/*
 * new_queue: create a queue (ctor)
 *   max: maximum number of nodes (optional)
 *   compar: comparison function for node data
 *   print_data_fun: node data pretty printing function (optional)
 *
 * returns the queue created, NULL on error
 */
struct queue_t * new_queue(int (*compar)(const void *, const void *))
{
	struct queue_t *hd;
	
	MALLOC_1DARRAY(hd, 1, struct queue_t);
	hd->compar=compar?compar:default_compar_fun;

	return hd;
}

/*
 * delete_queue: destroys a queue
 *   hd: queue (this)
 */
void
delete_queue(struct queue_t *hd)
{
	reset_queue(hd);
	FREE(hd);
}

/* inserts data to the correct place in a sorted queue, if data did
 * not exist before in the queue
 *  hd : the queue
 *  dataPtr : a pointer to the node of the queue that contains the data
 * returns 1 if data is not in queue and 0 if data is already
 * in the queue */
int enqueue_unique_to_sorted_queue(struct queue_t *hd,
	struct queue_node_t **dataPtr, const void *data)
{
	struct queue_node_t *p;
	struct queue_node_t *newItem;
	
	if(QUEUE_IS_EMPTY(hd)) {
		MALLOC_1DARRAY(newItem, 1, struct queue_node_t);
		newItem->data=data;
		newItem->next=NULL;
		hd->count++;
		hd->head=hd->tail=newItem;
		*dataPtr=newItem;
							
		return 1;
	}

	p=hd->head;
	
	switch(hd->compar(p->data, data){
		case -1:
			break;
		case 0:
			*dataPtr=p;
			return 0;
		case 1:
			NEW_QUEUE_ITEM(newItem, data, hd, p);
			hd->head=newItem;
			*dataPtr=newItem;
							
			return 1;					
	}
	
	QUEUE_FOREACH(p, hd) {
		if(p->next)
			switch(hd->compar(p->next->data, data))
			{
				case -1:
					break;
				case 0:
					*dataPtr=p->next;
					return 0;
				case 1:
					NEW_QUEUE_ITEM(newItem, data, hd, p->next);
					p->next=newItem;
					*dataPtr=newItem;
									
					return 1;					
			}
	}
	NEW_QUEUE_ITEM(newItem, data, hd, NULL);
	hd->tail->next=newItem;
	hd->tail = newItem;
	*dataPtr=newItem;

	return 1;
}

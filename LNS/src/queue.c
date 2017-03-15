#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"
#include "memory.h"
#include "queue.h"

/*
 * dequeue_from_queue: dequeues an item from queue
 *   hd: queue (this)
 *   data: dequeued item (optional)
 *
 * returns 1 if successful, 0 otherwise
 */
 
int dequeue_from_queue(struct queue_t *hd, void **data)
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
	FREE_SAFE(p);

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
unsigned long reset_queue(struct queue_t *hd)
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
	QUEUE_COUNT(hd)=0;

	return i;
}

/*
 * new_queue: create a queue (ctor)
 *   max: maximum number of nodes (optional)
 *   compar: comparison function for node data
 *
 * returns the queue created, NULL on error
 */
struct queue_t * new_queue(unsigned long max, 
	int (*compar)(const void *, const void *))
{
	struct queue_t *hd;
	
	MALLOC_ZERO(hd, sizeof(struct queue_t));
	hd->compar=compar;
	QUEUE_SET_MAX(hd, max);
	QUEUE_COUNT(hd)=0;
		
	return hd;
}

/*
 * delete_queue: destroys a queue
 *   hd: queue (this)
 */
void delete_queue(struct queue_t *hd)
{
	reset_queue(hd);
	FREE_SAFE(hd);
}

/* inserts unique data in the correct place of a sorted queue
 *  hd : the queue
 *  dataPtr : a pointer to the node of the queue that contains the data
 * return value:
 *  NEW_ITEM : if data is not in the queue
 *  DUPLICATE_ITEM: if data is already in the queue */

int enqueue_unique_to_sorted_queue(struct queue_t *hd,
	struct queue_node_t **dataPtr, void *data)
{
	struct queue_node_t *p;
	struct queue_node_t *newItem;
	
	if (QUEUE_IS_FULL(hd)) {
		fprintf(stderr, "Internal error: Trying to insert data in \
			a full queue\n");
		exit(1);
	}
	
	if(QUEUE_IS_EMPTY(hd)) {
		NEW_QUEUE_ITEM(newItem, data, hd, NULL);
		hd->head=hd->tail=newItem;
		*dataPtr=newItem;
							
		return NEW_ITEM;
	}

	p=hd->head;
	
	switch(hd->compar(p->data, data)){
		case LESS:
			break;
		case EQUAL:
			*dataPtr=p;
			return DUPLICATE_ITEM;
		case GREATER:
			NEW_QUEUE_ITEM(newItem, data, hd, p);
			hd->head=newItem;
			*dataPtr=newItem;
							
			return NEW_ITEM;					
	}
	
	QUEUE_FOREACH(p, hd) {
		if(p->next)
			switch(hd->compar(p->next->data, data))
			{
				case LESS:
					break;
				case EQUAL:
					*dataPtr=p->next;
					return DUPLICATE_ITEM;
				case GREATER:
					NEW_QUEUE_ITEM(newItem, data, hd, p->next);
					p->next=newItem;
					*dataPtr=newItem;
									
					return NEW_ITEM;					
			}
	}
	
	NEW_QUEUE_ITEM(newItem, data, hd, NULL);
	hd->tail->next=newItem;
	hd->tail = newItem;
	*dataPtr=newItem;

	return NEW_ITEM;
}

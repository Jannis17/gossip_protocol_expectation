/* queue.c */
//~ #include <stdio.h>
//~ #include <stdlib.h>
//~ #include <string.h>
#include "queue.h"

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
struct queue_t *
new_queue(const unsigned long max,
	int (*compar)(const void *, const void *),
	void (*print_data_fun)(FILE *fp, const void *))
{
	struct queue_t *hd;

	MALLOC_ZERO(hd, sizeof(struct queue_t));
	hd->compar=compar?compar:default_compar_fun;
	hd->print_data_fun=print_data_fun;
	QUEUE_SET_MAX(hd, max);

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
	struct queue_node_t *newNode;
	
	if(QUEUE_IS_EMPTY(hd)) {
		MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
		newNode->data=data;
		newNode->next=NULL;
		hd->head=hd->tail=newNode;
		*dataPtr=newNode;
							
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
			MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
			newNode->data=data;
			newNode->next=p;
			hd->head=newNode;
			*dataPtr=newNode;
							
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
					MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
					newNode->data=data;
					newNode->next=p->next;
					p->next=newNode;
					*dataPtr=newNode;
									
					return 1;					
			}
	}
	
	MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
	newNode->data=data;
	newNode->next=NULL;
	hd->tail->next=newNode;
	hd->tail = newNode;
	*dataPtr=newNode;

	return 1;
}

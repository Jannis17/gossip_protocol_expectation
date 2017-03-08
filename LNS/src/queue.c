/* queue.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


/*
 * search_queue: search data in queue
 *   hd: queue (this)
 *   found: data's queue node (optional)
 *   prev: data's previous queue node (optional)
 *   all: all queue nodes with this data (optional)
 *   where: data queue node's offset in queue (optional)
 *   data: data to lookup
 *
 * returns 1 if found, 0 otherwise
 */
int
search_queue(struct queue_t *hd,
	struct queue_node_t **found, struct queue_node_t **prev,
	struct queue_t **all, int *where,
	const void *data)
{
	struct queue_t *q;
	struct queue_node_t *p, *pp;
	int px;

	if(QUEUE_IS_EMPTY(hd)) {
		if(all)
			*all=NULL;

		return 0;
	}

	px=0;
	p=hd->head;
	if(!hd->compar(p->data, data)) {
		if(prev)
			*prev=NULL;
		if(where)
			*where=px;
		if(found)
			*found=p;

		if(!all)
			return 1;
		else
			goto sunexeia;
	}
	QUEUE_FOREACH(p, hd) {
		px++;
		if(p->next)
			if(!hd->compar(p->next->data, data)) {
				if(prev)
					*prev=p;
				if(where)
					*where=px;
				p=p->next;
				if(found)
					*found=p;

				if(!all)
					return 1;
				else
					goto sunexeia;
			}
	}

	return 0;

sunexeia:
	q=new_queue(hd->max, hd->compar, hd->print_data_fun);
	QUEUE_FOREACH_START(pp, p)
		if(pp->next)
			if(!hd->compar(data, pp->next->data))
				enqueue_to_queue(q, pp->next->data);
	*all=q;

	return 1;
}


/*
 * enqueue_to_queue: enqueues an item to queue
 *   hd: queue (this)
 *   data: item to enqueue
 *
 * returns 1
 */
int
enqueue_to_queue(struct queue_t *hd, void *data)
{
	struct queue_node_t *p;

	if(QUEUE_IS_FULL(hd))
		return 0;

	MALLOC_SAFE(p, sizeof(struct queue_node_t));

	p->data=data;
	p->next=NULL;
	if(!hd->head)
		hd->head=hd->tail=p;
	else {
		hd->tail->next=p;
		hd->tail=p;
	}

	hd->count++;

	return 1;
}


int enqueue_to_sorted_queue(struct queue_t *hd,
	struct queue_node_t **found, const void *data)
{
	struct queue_t *q;
	struct queue_node_t *p, *pp;
	struct queue_node_t *newNode;
	
	if(QUEUE_IS_EMPTY(hd))
		return 0;

	p=hd->head;
	
	switch(hd->compar(p->data, data){
		case -1:
			break;
		case 0:
			if(found)
				*found=p;
			return 1;
		case 1:
			MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
			newNode->data=data;
			newNode->next=hd->head->next;
			hd->tail->next=newNode;
			hd->tail=newNode;
			if(found)
				*found=p;
				
			return 1;					
	}
	
	QUEUE_FOREACH(p, hd) {
		px++;
		if(p->next)
			switch(hd->compar(p->next->data, data))
			{
				case -1:
					break;
				case 0:
					p=p->next;
					if(found)
						*found=p;
					return 1;
				case 1:
				MALLOC_SAFE(newNode, sizeof(struct queue_node_t));
				newNode->data=data;
				newNode->next=p->next;
				p->next=newNode;
				if(found)
					*found=p;
				
				return 1;					
			}
	}

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"

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

/*
 * search_in_sorted_queue: search data in queue
 *   hd: queue (this)
 *   found: data's queue node (optional)
 *   prev: data's previous queue node (optional)
 *   all: all queue nodes with this data (optional)
 *   where: data queue node's offset in queue (optional)
 *   data: data to lookup
 *
 * returns 1 if found, 0 otherwise
 */
int search_in_sorted_queue
	(struct queue_t *hd, 
	 struct queue_node_t *start, 
	 struct queue_node_t **prev,
	 struct queue_node_t **found, const void *data)
{
	struct queue_node_t *p;
	
				
	if(QUEUE_IS_EMPTY(hd))
			return 0;
		
	if (!start)
		start = hd->head;			
		
	switch(hd->compar(start->data, data)){
		case LESS:
			break;
		case EQUAL:
			if (prev) {
				if (start == hd->head)
					*prev=NULL;
				else
					*prev=start;
			}
			if (found)
				*found=start;
			return 1;
		case GREATER:
			if (prev)
				*prev=NULL;
			if (found)
				*found=NULL;
			return 0;					
	}
				
	for(p=start; p; p=p->next)
		if(p->next)
			switch(hd->compar(p->next->data, data))
			{
				case LESS:
					break;
				case EQUAL:
					if (prev)
						*prev=p;
					if (found)
						*found=p->next;
					return 1;
				case GREATER:
					if (prev)
						*prev=p;
					if (found)
						*found = NULL;									
					return 0;					
			}
			
	return 0;
}

/* inserts unique data in the correct place of a sorted queue
 *  hd : the queue
 *  found : a pointer to the node of the queue that contains the data
 *  return value:
 *  NEW_ITEM : if data is not in the queue
 *  DUPLICATE_ITEM: if data is already in the queue */
int enqueue_unique_to_sorted_queue
(struct queue_t *hd, struct queue_node_t *start,
 struct queue_node_t **found, void *data)
{
	struct queue_node_t *p;
	struct queue_node_t *new_item;
	
	if (QUEUE_IS_FULL(hd))
		INTERNAL_ERROR("Trying to insert data in a full queue");
		
	if(QUEUE_IS_EMPTY(hd)) {
		NEW_QUEUE_ITEM(new_item, data, hd, NULL);
		hd->head=hd->tail=new_item;
		if (found)
			*found=new_item;
							
		return NEW_ITEM;
	}	
	
	if (!start)
		start = hd->head;
		
	switch(hd->compar(start->data, data)){
		case LESS:
			break;
		case EQUAL:
			if (found)
				*found=start;
			return DUPLICATE_ITEM;
		case GREATER:
			NEW_QUEUE_ITEM(new_item, data, hd, start);
			if (start == hd-> head)
				hd->head=new_item;
			else
				start->next = new_item;
			if (found)
				*found=new_item;
					
			return NEW_ITEM;					
	}
			
	for(p=start; p; p=p->next)
		if(p->next)
			switch(hd->compar(p->next->data, data))
			{
				case LESS:
					break;
				case EQUAL:
					if (found)
						*found=p->next;
					return DUPLICATE_ITEM;
				case GREATER:
					NEW_QUEUE_ITEM(new_item, data, hd, p->next);
					p->next=new_item;
					if (found)
						*found=new_item;
									
					return NEW_ITEM;					
			}
					
	NEW_QUEUE_ITEM(new_item, data, hd, NULL);
	hd->tail->next=new_item;
	hd->tail = new_item;
	
	if (found)
		*found=new_item;

	return NEW_ITEM;
}

//~ int enqueue_unique_to_sorted_queue
//~ (struct queue_t *hd, struct queue_node_t *start,
 //~ struct queue_node_t **found, void *data)
//~ {
	//~ struct queue_node_t *p, *prev;
	//~ struct queue_node_t *new_item;
	
	//~ if (QUEUE_IS_FULL(hd))
		//~ INTERNAL_ERROR("Trying to insert data in a full queue");
		
	//~ if (!start)
		//~ start = hd->head;
		
	//~ for(p=prev=start; p; p=p->next) {
		//~ switch(hd->compar(p->data, data))
		//~ {
			//~ case LESS:
				//~ break;
			//~ case EQUAL:
				//~ if (found)
					//~ *found=p;
				//~ return DUPLICATE_ITEM;
			//~ case GREATER:
				//~ NEW_QUEUE_ITEM(new_item, data, hd, p);
				//~ prev->next=newItem;
				//~ if (found)
					//~ *found=newItem;
								
				//~ return NEW_ITEM;					
		//~ }
		//~ prev = p;		
	//~ }
						
	//~ NEW_QUEUE_ITEM(new_item, data, hd, NULL);
	//~ hd->tail->next=new_item;
	//~ hd->tail=new_item;
	//~ if (found)
		//~ *found=new_item;

	//~ return NEW_ITEM;
//~ }


/* enqueues a unique item to twin queues */
void enqueue_unique_to_twin_queues
(twin_queues twin_q, 
struct queue_node_t* fixed_name_start,
struct queue_node_t* can_start, 
void* item,
int prot)
{
  enqueue_unique_to_sorted_queue
	(twin_q.can_lab_queue, can_start, NULL, item);
	
  if (prot == ANY)	
	 enqueue_unique_to_sorted_queue
		(twin_q.fixed_name_queue, fixed_name_start, NULL, item);					
}

int enqueue_to_hash
(twin_queues hash[MAXN*MAXN], 
 struct queue_node_t* fixed_name_start,
 struct queue_node_t* can_start,
 pstate_t* s,
 struct queue_node_t** found, 
 int prot)
{
	int result;
	struct queue_t* fixed_name_queue = NULL, * can_queue = NULL;
	
	struct queue_node_t* fixed_name_prev;
	
	fixed_name_prev = NULL;
	
	switch (prot) {
		case ANY:
		case LNS:
			can_queue = hash[s->total_secrets -1].can_lab_queue;
			fixed_name_queue = 
				hash[s->total_secrets -1].fixed_name_queue;	
		break;
		case CO:
			can_queue = hash[s->total_calls].can_lab_queue;	
		break;
	}	
		
	if ( prot == ANY &&
		 search_in_sorted_queue (fixed_name_queue, fixed_name_start, 
			 &fixed_name_prev, found, s) )
		return DUPLICATE_ITEM;
	
	result=
		enqueue_unique_to_sorted_queue(can_queue, can_start, found, s);
	
	if (prot == ANY && result == NEW_ITEM)
		enqueue_unique_to_sorted_queue
			(fixed_name_queue, fixed_name_prev, found, s);
	
	return result; 					
}

/* searches for a state in twin queues
 * return value: 1 if found, 0 ow */
int 
search_in_twin_queues 
(twin_queues twin_q, 
 struct queue_node_t* fixed_name_start,
 struct queue_node_t* can_start,
 struct queue_node_t** fixed_name_prev,
 struct queue_node_t** can_prev,
 struct queue_node_t** found,  
 child_t* child, 
 int prot)
{
	struct queue_t* fixed_name_queue, *can_queue;
	
	fixed_name_queue = twin_q.fixed_name_queue;
	can_queue = twin_q.can_lab_queue;
	
	if ( prot == ANY &&
		 search_in_sorted_queue
		 (fixed_name_queue, fixed_name_start, fixed_name_prev, found, 
		  child) )
		return 1;
	
	return search_in_sorted_queue(can_queue, can_start, can_prev,found,
		child); 					
}



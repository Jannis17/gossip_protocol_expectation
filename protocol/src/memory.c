#include "main.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"
#include "graph.h"

/* searches for a state in twin queues
 * return value: 1 if found, 0 ow */
int search_in_twin_queues 
(twin_queues twin_q, struct queue_node_t** found, 
 child_t* child, int protocol_name)
{
	struct queue_t* fixed_name_queue, * can_queue;
	
	fixed_name_queue = twin_q.fixed_name_queue;
	can_queue = twin_q.can_lab_queue;
	
	if ( protocol_name == ANY &&
		 search_in_sorted_queue(fixed_name_queue, found, child))
		return 1;
	
	return search_in_sorted_queue(can_queue, found, child); 					
}

protocol_state_t* new_protocol_state 
(graph g[MAXN*MAXM], int agents, int protocol_name)
{
	protocol_state_t* s;
	
	MALLOC_SAFE(s, sizeof(protocol_state_t));
	
	copy_graph(s->fixed_name_secrets, g, agents);
	
	s->children.fixed_name_queue = 
		new_queue(MAXN*(MAXN-1), comp_fixed_name_children);
	s->children.can_lab_queue = 
		new_queue(MAXN*(MAXN-1), comp_can_children);
		
	if (protocol_name == ANY) {
		copy_graph(s->fixed_name_secrets_sorted, g, agents);
		qsort(s->fixed_name_secrets_sorted, 
			agents*MAXM, sizeof(graph), cmp_graph_nodes);
	}
			
	find_can_label(g, s->can_secrets, agents); 
			
	s->id = 0;
	s->agents = agents;
	s->edges = edges_of(g, agents);
							
	return s;		
}

/* creates a new child */
child_t *new_child
( graph secrets[MAXN*MAXM], struct queue_node_t* s, int calls_to_child)
{
	child_t* result;
		
	MALLOC_SAFE(result, sizeof(child_t));
	result->calls_to_child=calls_to_child;
	result->child_pos_in_hash=s;
	
	return result;
}

void destroy_protocol_state (protocol_state_t ** s)
{
	DELETE_QUEUE((*s)->children.can_lab_queue);
	DELETE_QUEUE((*s)->children.fixed_name_queue);
	FREE_SAFE(*s);
}

void destroy_twin_queues(twin_queues* twin_q)
{
	protocol_state_t *s;
	struct queue_node_t * p;
	
	QUEUE_FOREACH(p, twin_q->can_lab_queue) {
			s = (protocol_state_t *) (p->data);
			//~ printf("state = %d\n", s->id+1);			
			//~ print_graph(s->can_secrets, s->agents);
			destroy_protocol_state(&s);			
		}
		DELETE_QUEUE(twin_q->can_lab_queue);
}

void destroy_hash(int agents, twin_queues hash[MAXN*MAXN]) 
{
	int i;
	//~ printf("\nMarkov chain (%d agents)\n", agents);
		
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents) {
		//~ printf("%d secrets\n", i+1);
		destroy_twin_queues(&hash[i]);
	}	
}

#include "main.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"
#include "graph.h"

protocol_state_t* new_protocol_state 
(graph g[MAXN*MAXM], int agents, int protocol_name)
{
	protocol_state_t* s;
	
	MALLOC_SAFE(s, sizeof(protocol_state_t));
	
	copy_graph(s->fixed_name_secrets, g, agents);
	s->children.fixed_name_queue = 
		new_queue(MAXN*(MAXN-1), comp_fixed_name_children);
		
	if (protocol_name == ANY) {
		copy_graph(s->fixed_name_secrets_sorted, g, agents);
		qsort(s->fixed_name_secrets_sorted, 
			agents*MAXM, 
			sizeof(graph), cmp_graph_nodes);
	}
			
	find_can_label(g, s->can_secrets, agents); 
			
	s->children.can_lab_queue = 
		new_queue(MAXN*(MAXN-1), comp_can_children);
		
	s->id = 0;
	s->agents = agents;
	s->edges = edges_of(g, agents);
							
	return s;		
}

/* creates a new child */
child_t *new_child
( graph secrets[MAXN*MAXM], protocol_state_t *s, int calls_to_child)
{
	child_t* result;
		
	MALLOC_SAFE(result, sizeof(child_t));
	result->calls_to_child=calls_to_child;
	result->childs_state_ptr=s;
	
	return result;
}

void destroy_protocol_state (protocol_state_t ** s)
{
	DELETE_QUEUE((*s)->children.can_lab_queue);
	DELETE_QUEUE((*s)->children.fixed_name_queue);
	FREE_SAFE(*s);
}


void destroy_hash(int agents, twin_queues hash[MAXN*MAXN]) 
{
	int i;
	//~ printf("\nMarkov chain (%d agents)\n", agents);
		
	protocol_state_t *s;
	
	struct queue_node_t * p;
	
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents) {
		//~ printf("%d secrets\n", i+1);
		QUEUE_FOREACH(p, hash[i].can_lab_queue) {
			s = (protocol_state_t *) (p->data);
			
			//~ printf("state = %d\n", s->id+1);			
			//~ print_graph(s->can_secrets, s->agents);
			
			DELETE_QUEUE(s->children.fixed_name_queue);
			DELETE_QUEUE(s->children.can_lab_queue);
			FREE_SAFE(s);
		}
		DELETE_QUEUE(hash[i].can_lab_queue);
	}	
}

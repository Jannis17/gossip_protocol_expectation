#include "main.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"
#include "graph.h"


void init_hash
(twin_queues hash[MAXN*MAXN], int agents, int protocol_name)
{
	int i;
		
	FOR_ALL_EDGES(i, agents){
		hash[i].can_lab_queue = new_queue(MAXSTATES, comp_can_secrets);
		hash[i].fixed_name_queue = new_queue(MAXSTATES, comp_fixed_name_secrets);
	}
}

protocol_state_t* new_protocol_state 
(graph g[MAXN*MAXM], int agents, int protocol_name)
{
	protocol_state_t* s;
	
	MALLOC_SAFE(s, sizeof(protocol_state_t));
	
	copy_graph(s->fixed_name_secrets, g, agents);
	
	//~ print_graph(g, agents);
	
	if (protocol_name == ANY) {
		copy_graph(s->fixed_name_secrets_sorted, g, agents);
		qsort(s->fixed_name_secrets_sorted, agents*MAXM, 
			sizeof(graph), comp_nodes);
	}
	
	//~ print_graph(g, agents);
			
	find_can_label(g, s->can_secrets, agents); 
	
	//~ print_graph(s->can_secrets, agents);
			
	s->children = new_queue(MAXN*(MAXN-1), comp_children);
	s->id = 0;
	s->agents = agents;
	s->edges = edges_of(g, agents);
							
	return s;		
}

/* creates a new child */
child_t *new_child
( graph secrets[MAXN*MAXM], int agents, 
  int protocol_name, int calls_to_child)
{
	protocol_state_t* childs_state = 
		new_protocol_state(secrets, agents, protocol_name);
	
	child_t* result;
		
	MALLOC_SAFE(result, sizeof(child_t));
	result->calls_to_child=calls_to_child;
	result->childs_state_ptr=childs_state;
	
	return result;
}

void destroy_child(child_t *child)
{
	DELETE_QUEUE(child->childs_state_ptr->children);
	FREE_SAFE(child->childs_state_ptr);
	FREE_SAFE(child);
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
			
			DELETE_QUEUE(s->children);
			FREE_SAFE(s);
		}
		DELETE_QUEUE(hash[i].can_lab_queue);
	}	
}

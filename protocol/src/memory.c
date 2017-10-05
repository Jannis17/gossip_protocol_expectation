#include "main.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"
#include "graph.h"

pstate_t* new_pstate 
(graph g[MAXN*MAXM], int agents, int prot)
{
	pstate_t* s;
	
	MALLOC_SAFE(s, sizeof(pstate_t));
	
	copy_graph(s->fixed_name_secrets, g, agents);
	
	s->children.fixed_name_queue = 
		new_queue(MAXN*(MAXN-1), cmp_fixed_name_children);
	s->children.can_lab_queue = 
		new_queue(MAXN*(MAXN-1), cmp_can_children);
		
	if (prot == ANY) {
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
( graph secrets[MAXN*MAXM], pstate_t* childs_state,
  int calls_to_child)
{
	child_t* result;
	int i,j;
		
	MALLOC_SAFE(result, sizeof(child_t));
	result->calls_to_child=calls_to_child;
	result->childs_state=childs_state;
	
	for (i=0; i< childs_state -> agents; i++)
		for (j=0; j< childs_state -> agents; j++)
			result->calls[i][j]=0;
	
	return result;
}

void destroy_protocol_state (pstate_t ** s)
{
	if (*s) {
		DELETE_QUEUE((*s)->children.can_lab_queue);
		DELETE_QUEUE((*s)->children.fixed_name_queue);
		FREE_SAFE(*s);
	}
}

void destroy_twin_queues(twin_queues* twin_q)
{
	pstate_t *s;
	struct queue_node_t * p;
	
	QUEUE_FOREACH(p, twin_q->can_lab_queue) {
			s = (pstate_t *) (p->data);
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

void malloc_safe_2D_float(float ***p, int n) 
{
	int i;
	
	MALLOC_SAFE(*p, n * sizeof(float*) );
		
	for(i=0; i < n; i++)
		MALLOC_SAFE((*p)[i], n * sizeof(float) );
}


void free_safe_2D_float(float ***p, int n)
{
	int i;
	
	for(i=0; i < n; i++)
		FREE_SAFE((*p)[i]);
	
	FREE_SAFE(*p);	
}

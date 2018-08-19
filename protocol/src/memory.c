#include <math.h>
#include "main.h"
#include "memory.h"
#include "queue.h"
#include "state.h"
#include "compar.h"
#include "graph.h"

//copy_token
void copy_tokens(int from[MAXN], int to[MAXN], int n)
{
		int i;
		
		for(i=0; i<n; i++)
			to[i]=from[i];
}

int count_tokens(int token[MAXN], int n)
{
		int i, result;
		
		for (i=result=0;i<n;i++)
			result+=token[i];
		
		return result;	
}

/* creates a new protocol state */
pstate_t* 
new_pstate(graph secrets[MAXN*MAXM], int calls[MAXN][MAXN],
int token[MAXN], int total_calls, pars_t pars)
{
	pstate_t* s;
	
	MALLOC_SAFE(s, sizeof(pstate_t));
	
	graph w_token[MAXN*MAXM];
	graph wo_token[MAXN*MAXM];
	
	//printf("total tokes = %d\n", total_tokens);
	
	int i, j, total_tokens;
	
	if (pars.prot == TOK || pars.prot==SPI)
		copy_tokens(token,s->token, pars.n);
	
	copy_graph(s->fixed_name_secrets, secrets, pars.n, pars.m);
	can_label_secrets(secrets, s->can_secrets, s-> token, 
		pars.prot, pars.n, pars.m); 
	
	if ( (pars.prot == ANY && !pars.o_ANY) || pars.prot == SPI 
		 || pars.prot == TOK)
		s->children.fixed_name_queue = 
			new_queue(MAXN*(MAXN-1), cmp_fixed_name_children);
	else
		s->children.fixed_name_queue = NULL;
		
	if (pars.prot == CO)
		s->children.can_lab_queue =
			new_queue(MAXN*(MAXN-1), cmp_can_children_calls);
	else
		s->children.can_lab_queue =
			new_queue(MAXN*(MAXN-1), cmp_can_children_secrets);
	
	if ( (pars.prot == ANY && !pars.o_ANY) ) {
		copy_graph(s->fixed_name_secrets_sorted, secrets, 
			pars.n, pars.m);
		qsort(s->fixed_name_secrets_sorted, 
			pars.n*pars.m, sizeof(graph), cmp_graph_nodes);
	}
	
					
	if (pars.prot == SPI || pars.prot == TOK) {
		for(i=j=0;i<pars.n;i++)
			if (token[i])
				w_token[j++]=secrets[i];
		
		total_tokens=j;		
	
		qsort(w_token, total_tokens*pars.m, sizeof(graph), 
			cmp_graph_nodes);
		
		if (j<pars.n) {
			for(i=0;i<pars.n;i++)
				if (!token[i])
					wo_token[j++]=secrets[i];
			
			qsort(wo_token, 
				(pars.n-total_tokens)*pars.m, sizeof(graph), 
					cmp_graph_nodes);
		}
			
		for(i=0;i<total_tokens; i++)
			s->fixed_name_secrets_sorted[i]=w_token[i];
			
		for(;i<pars.n; i++)
			s->fixed_name_secrets_sorted[i]=wo_token[i-total_tokens];			
	}
	
	
	s->nl = pars.n * ceil(log2( (pars.n * (pars.n-1))/2 + 1 ));
	s->ml = SETWORDSNEEDED(s->nl);
	s->id=0;
	s->n=pars.n;
	s->m=pars.m;
	s->total_secrets=edges_of(secrets, pars.n, pars.m);
	s->total_calls=total_calls;
	s->prot=pars.prot;
	
	s->is_absorption = (s->total_secrets == pars.n*pars.n)?1:0;
	copy_calls_graph(s->fixed_name_calls, calls, pars.n);
	can_label_calls(calls, s->can_calls,pars.n, s->nl, s-> ml);
			
	return s;		
}

/* creates a new child */
child_t *new_child (pstate_t* childs_state, int calls_to_child)
{
	child_t* result;
	int i,j;
		
	MALLOC_SAFE(result, sizeof(child_t));
	result->calls_to_child=calls_to_child;
	result->childs_state=childs_state;
	
	for (i=0; i<childs_state->n; i++)
		for (j=0; j<childs_state->n; j++)
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
	//~ int i;
	
	if (!QUEUE_IS_EMPTY(twin_q->can_lab_queue)) {
		QUEUE_FOREACH(p, twin_q->can_lab_queue) {
				s = (pstate_t *) (p->data);
				
				//print fixed names graph
				//~ print_graph(s->fixed_name_secrets,s->n, s->m);
				
				//~ printf("tokens=");
				
				//~ for (i=0; i < s-> n; i++)
					//~ printf("%d, ", s->token[i]);
				
				//~ printf("\n=======================\n");
				
				destroy_protocol_state(&s);			
			}
		DELETE_QUEUE(twin_q->can_lab_queue);
		DELETE_QUEUE(twin_q->fixed_name_queue);
	}
	
	twin_q->can_lab_queue = twin_q->fixed_name_queue = NULL;
}

void destroy_hash(int n, twin_queues hash[MAXN*MAXN]) 
{
	int i;
	
	FOR_ALL_EDGES(i, n)
		destroy_twin_queues(&hash[i]);
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

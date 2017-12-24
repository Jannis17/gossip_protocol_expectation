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
	int token[MAXN], int total_calls, int n, int m, int prot)
	{
		pstate_t* s;
		
		MALLOC_SAFE(s, sizeof(pstate_t));
		
		graph w_token[MAXN*MAXM];
		graph wo_token[MAXN*MAXM];
		
		int total_tokens;
		
		//printf("total tokes = %d\n", total_tokens);
		
		int i, j;
		
		if (prot == TOK || prot==SPI)
			copy_tokens(token,s->token, n);
		else
			for(i=0;i<n;i++)
				token[i]=1;
		
		copy_graph(s->fixed_name_secrets, secrets, n, m);
		can_label_secrets(secrets, s->can_secrets, s-> token, prot, n, m); 
		
		if (prot == ANY || prot == SPI || prot == TOK)
			s->children.fixed_name_queue = 
				new_queue(MAXN*(MAXN-1), cmp_fixed_name_children);
		else
			s->children.fixed_name_queue = NULL;
			
		if (prot == CO)
			s->children.can_lab_queue =
				new_queue(MAXN*(MAXN-1), cmp_can_children_calls);
		else
			s->children.can_lab_queue =
				new_queue(MAXN*(MAXN-1), cmp_can_children_secrets);
		
		if (prot == ANY) {
			copy_graph(s->fixed_name_secrets_sorted, secrets, n, m);
			qsort(s->fixed_name_secrets_sorted, 
				n*m, sizeof(graph), cmp_graph_nodes);
		}
						
		if (prot == SPI || prot == TOK) {
			for(i=j=0;i<n;i++)
				if (token[i])
					w_token[j++]=secrets[i];
			
			total_tokens=j;		
		
			qsort(w_token, total_tokens*m, sizeof(graph), cmp_graph_nodes);
			
			if (j<n) {
				for(i=0;i<n;i++)
					if (!token[i])
						wo_token[j++]=secrets[i];
				
				qsort(wo_token, 
					(n-total_tokens)*m, sizeof(graph), cmp_graph_nodes);
			}
				
			for(i=0;i<total_tokens; i++)
				s->fixed_name_secrets_sorted[i]=w_token[i];
				
			for(;i<n; i++)
				s->fixed_name_secrets_sorted[i]=wo_token[i];			
		}
		
		s->nl = n * ceil(log2( (n * (n-1))/2 + 1 ));
		s->ml = SETWORDSNEEDED(s->nl);
		s->id=0;
		s->n=n;
		s->m=m;
		s->total_secrets=edges_of(secrets, n, m);
		s->total_calls=total_calls;
		s->prot=prot;
		
		s->is_absorption = (s->total_secrets == n*n)?1:0;
		
		copy_calls_graph(s->fixed_name_calls, calls, n);
		can_label_calls(calls, s->can_calls,n, s->nl, s-> ml);
				
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
		
		if (!QUEUE_IS_EMPTY(twin_q->can_lab_queue)) {
			QUEUE_FOREACH(p, twin_q->can_lab_queue) {
					s = (pstate_t *) (p->data);
					
					//print fixed names graph
					print_graph(s->can_secrets,s->n, s->m);
					
					printf("=======================\n");
					
					destroy_protocol_state(&s);			
				}
			DELETE_QUEUE(twin_q->can_lab_queue);
			DELETE_QUEUE(twin_q->fixed_name_queue);
		}
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

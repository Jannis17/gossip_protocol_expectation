#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "queue.h"
#include "compar.h"
#include "test.h"
#include "../../nauty26r7/nauty.h"

void generate_children
(pstate_t *parent, int n, int m, twin_queues hash[MAXN*MAXN], int prot) 
{
	int i, j, calls_to_child;
	graph temp_secrets[MAXN*MAXM];
	graph temp_calls[MAXN*MAXM];
	child_t* found_child;
	struct queue_node_t* found_child_node;
	struct queue_node_t* child_pos_in_par_list;
	struct queue_node_t* childs_can_prev = NULL;
	struct queue_node_t* childs_fixed_name_prev = NULL;
	child_t * potential_child;
	pstate_t* childs_state;
					
	for (i=0; i<n; i++)
	 for (j=i+1; j<n; j++)
	  if ( (calls_to_child = no_poss_calls(parent,i,j,prot, n)) > 0 )
		{
		  copy_graph(temp_secrets, parent->fixed_name_secrets, n, m);
		  update_secrets(temp_secrets, i ,j, n);
		  		  
		  childs_state = new_pstate(temp_secrets, temp_calls, n, m, prot);
		  potential_child = 
			new_child(temp_secrets, childs_state, calls_to_child);
		  
		  potential_child->calls[i][j] = 
			can_call( parent->fixed_name_secrets, i, j, n);
				
		  potential_child->calls[j][i] = 
			can_call(parent->fixed_name_secrets, j, i, n);
		  		  
		  if (search_in_twin_queues(parent->children, NULL, NULL,
			   &childs_fixed_name_prev, &childs_can_prev,
			   &child_pos_in_par_list, potential_child, prot))
		  {
			 FREE_SAFE(potential_child); 
			 destroy_protocol_state(&childs_state);
			 found_child = (child_t *) child_pos_in_par_list->data;
			 found_child->calls_to_child += calls_to_child;
			 
			 found_child->calls[i][j] = 
				can_call(parent->fixed_name_secrets, i, j, n);
				
			 found_child->calls[j][i] = 
				can_call(parent->fixed_name_secrets, j, i, n);
		  } 
		  else 
		  { if ( enqueue_to_hash (hash, NULL, NULL, childs_state, 
				 &found_child_node, prot) == DUPLICATE_ITEM ) 
			{
			  destroy_protocol_state(&childs_state);
			  potential_child->childs_state = 
				found_child_node->data;				
			}
			 enqueue_unique_to_twin_queues
				( parent->children, NULL, NULL,	
				  potential_child, prot );
		   }		  
		 }		  
}	

void build_the_markov_chain
(twin_queues hash[MAXN*MAXN], int n, int m,
 int prot, int calc_exp, int *no_states)
{
	int i;
	
	struct queue_node_t * p;
	
	//~ printf("=========================================\n");
	//~ printf("Agents = %d\n", n);
	
	//~ clock_t start, end;
	
	*no_states = 0;
	
	FOR_ALL_EDGES(i, n) {
		//~ start = clock();
		//~ printf("%d secrets:", i+1);
		QUEUE_FOREACH(p, hash[i].can_lab_queue) 
			generate_children(p->data, n, m, hash, prot);
		//~ end = clock();
		//~ printf("%lu states in %f seconds\n", 
				//~ QUEUE_COUNT(hash[i].can_lab_queue),
				//~ ( (float) end - start )/CLOCKS_PER_SEC );
		/* count the states */	
		if ( prot == ANY &&
			 QUEUE_COUNT(hash[i].can_lab_queue) != 
			 QUEUE_COUNT(hash[i].fixed_name_queue) ) {
			printf("canonical = %lu	\n", QUEUE_COUNT(hash[i].can_lab_queue));
			printf("fixed name = %lu\n", QUEUE_COUNT(hash[i].fixed_name_queue));
		   INTERNAL_ERROR("Queues do not have\
		   the same number of elements\n");			 
		  }
		*no_states += QUEUE_COUNT(hash[i].can_lab_queue);
					
		if (!calc_exp)
			destroy_twin_queues(&hash[i]);		
	}
	//~ printf("\n");
}

float get_prob
( pstate_t ** trans_matrix, 
  int from, int to, int prot, int rand_ag) 
{
	pstate_t* s = trans_matrix[from];
	pstate_t* t;
	struct queue_node_t *p;
	child_t* child;	
	float prob = 0;
	int i,j, calls, av_agents;
	
	QUEUE_FOREACH(p, s->children.can_lab_queue) {
		child = (child_t*) (p-> data);
		t = (pstate_t*) child->childs_state;
		
		if (t->id == to) {
			if (rand_ag) {
				for (av_agents = i=0; i<s->n;i++) {
					calls = s->n - POPCOUNT(s->fixed_name_secrets[i]);
					if (calls)
						av_agents++;
				}		
								
				for (i=0; i<s->n;i++) {
					calls = s->n - 
							POPCOUNT(s->fixed_name_secrets[i]);
					if (calls)
						for (j=0; j<s->n;j++) 
							prob = prob +
									( (float) child->calls[i][j]) / (calls * av_agents);
				}							
			}
			else {	
				prob = child->calls_to_child;			
			
				switch (prot) {
					case (ANY):
						prob = prob / ( (s->n) * (s->n - 1) );
						break;
					case (LNS):
						prob = prob / ((s->n) * (s->n) - s->total_secrets);
						break;
					case (CO):
						break;
					case (TOK):
						break;
					case (SPI):
						break;
					default:
						INTERNAL_ERROR("Unknown protocol name!");
				}
			}									 
		}
	}		
		
	return prob;	
}

void init_markov_chain
(twin_queues hash[MAXN*MAXN], int n, int m, int prot)
{
	int i;
	graph init_secrets [MAXN*MAXM];
	graph init_calls [MAXN*MAXM];
	pstate_t *s;
		
	FOR_ALL_EDGES(i, n){
		hash[i].can_lab_queue = 
			new_queue(MAXSTATES, cmp_can_secrets);
		hash[i].fixed_name_queue = 
			new_queue(MAXSTATES, cmp_fixed_name_secrets);
	}
	
	diagonal(init_secrets, n, m);
	diagonal(init_calls, n, m);
	
	s =	new_pstate(init_secrets, init_calls, n, m, prot);
	enqueue_to_hash(hash, NULL, NULL, s, NULL, prot);
}

float exact(int n, int m, int* no_states, int prot, int calc_exp, int rand_ag)
{		
	twin_queues hash[MAXN*MAXN];
	pstate_t *s;
	int i, j, label;
	pstate_t** trans_matrix;
	struct queue_node_t * p;
	float* expect_vec;
	float result = 0;
	//~ clock_t start, end;	
	
	/* create the lists in the markov chain and
	 * add the initial state				    */
	init_markov_chain(hash, n, m, prot);
			
	/* build the markov chain */
	build_the_markov_chain(hash, n, m, prot, calc_exp, no_states);
	
	if (calc_exp)
	{		
		MALLOC_SAFE( trans_matrix,
				(* no_states) * sizeof(pstate_t *) );
		 	
		/* label the states */
		label = 0;
		
		FOR_ALL_EDGES(i, n)
			QUEUE_FOREACH(p, hash[i].can_lab_queue) {
				s = (pstate_t *) (p->data);
				trans_matrix[label] = s;
				s->id = label++;
			}
	
		/* compute the expectation for each state */	
		MALLOC_SAFE(expect_vec, *no_states * sizeof(float));
		expect_vec[*no_states-1]= 0;
							
		for(i = (* no_states)-2; i>=0; i--)
		{
			expect_vec[i] = 1;
                                        
			for(j=i+1; j<(* no_states); j++)
				expect_vec[i] += 
					get_prob(trans_matrix, i, j, prot, rand_ag) * 
						expect_vec[j];
		
			if (prot == ANY)			       
				expect_vec[i] = 
					expect_vec[i] / 
					(1-get_prob(trans_matrix, i, i, prot, rand_ag));
		}
		
		result = expect_vec[0];
		
		//~ print_expect_vec_and_trans_matrix(*no_states, 
			//~ expect_vec, trans_matrix, n, prot, rand_ag);
		
		//~ start = clock();
		//~ print_probs_to_absorption (*no_states, trans_matrix,
			//~ n, prot, rand_ag, (n*(n-1))/2,
			//~ hash);		
		//~ end = clock();
		//~ printf("Elapsed time = %f seconds\n", 
			//~ ( (float) end - start )/CLOCKS_PER_SEC );
			
		FREE_SAFE(expect_vec);
		FREE_SAFE(trans_matrix);
		destroy_hash(n, hash);
	} 
				
	return result;	
}

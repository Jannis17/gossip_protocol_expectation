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
(protocol_state_t* parent, int agents, 
 twin_queues hash[MAXN*MAXN], int protocol_name) 
{
	int i, j, calls_to_child;
	graph temp[MAXN*MAXM];
	child_t* found_child;
	struct queue_node_t* found_child_node;
	struct queue_node_t* child_pos_in_par_list;
	struct queue_node_t* childs_can_prev = NULL;
	struct queue_node_t* childs_fixed_name_prev = NULL;
	child_t * potential_child;
	protocol_state_t* childs_state;
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    SWITCH_PROT_NAME(protocol_name, 
	    	calls_to_child = 
				poss_calls(parent->fixed_name_secrets, i, j),
	    	calls_to_child = 1);	    	
	    if ( calls_to_child > 0 )
		{
		  copy_graph(temp, parent->fixed_name_secrets, agents);
		  make_call(temp, i ,j);
		  		  
		  childs_state = 
			new_protocol_state(temp, agents, protocol_name);
		  potential_child = 
			new_child(temp, childs_state, calls_to_child);
		  
		  potential_child->calls[i][j] = 
			can_call( parent->fixed_name_secrets, i, j);
				
		  potential_child->calls[j][i] = 
			can_call(parent->fixed_name_secrets, j, i);
		  		  
		  if (search_in_twin_queues(parent->children, 
									NULL,
									NULL,
		                            &childs_fixed_name_prev,
									&childs_can_prev,
									&child_pos_in_par_list, 
		  							potential_child, protocol_name))
		  {
			 FREE_SAFE(potential_child); 
			 destroy_protocol_state(&childs_state);
			 found_child = (child_t *) child_pos_in_par_list->data;
			 found_child->calls_to_child += calls_to_child;
			 
			 found_child->calls[i][j] = 
				can_call(parent->fixed_name_secrets, i, j);
				
			 found_child->calls[j][i] = 
				can_call(parent->fixed_name_secrets, j, i);
		  } 
		  else 
		  { if ( enqueue_to_hash
				   (hash, 
					NULL,
					NULL,
					childs_state, 
				    &found_child_node, 
				    protocol_name)
					  == DUPLICATE_ITEM ) 
			{
			  destroy_protocol_state(&childs_state);
			  potential_child->childs_state = 
				found_child_node->data;				
			}
			  enqueue_unique_to_twin_queues
				( parent->children, 
				  NULL,
				  NULL,	
				  potential_child, protocol_name );
		   }		  
		 }		  
	   }
}	

void build_the_markov_chain
(twin_queues hash[MAXN*MAXN], int agents, 
 int protocol_name, int calc_exp, int *no_states)
{
	int i;
	
	struct queue_node_t * p;
	
	//~ printf("=========================================\n");
	//~ printf("Agents = %d\n", agents);
	
	//~ clock_t start, end;
	
	*no_states = 0;
	
	FOR_ALL_EDGES(i, agents) {
		//~ start = clock();
		//~ printf("%d secrets:", i+1);
		QUEUE_FOREACH(p, hash[i].can_lab_queue) 
			generate_children(p->data, agents, hash, protocol_name);
		//~ end = clock();
		//~ printf("%lu states in %f seconds\n", 
				//~ QUEUE_COUNT(hash[i].can_lab_queue),
				//~ ( (float) end - start )/CLOCKS_PER_SEC );
		/* count the states */	
		if ( protocol_name == ANY &&
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
( protocol_state_t ** trans_matrix, 
  int from, int to, int protocol_name, int rand_ag) 
{
	protocol_state_t* s = trans_matrix[from];
	protocol_state_t* t;
	struct queue_node_t *p;
	child_t* child;	
	float prob = 0;
	int i,j, calls, av_agents;
	
	QUEUE_FOREACH(p, s->children.can_lab_queue) {
		child = (child_t*) (p-> data);
		t = (protocol_state_t*) child->childs_state;
		
		if (t->id == to) {
			if (rand_ag) {
				for (av_agents = i=0; i<s->agents;i++) {
					calls = s->agents - POPCOUNT(s->fixed_name_secrets[i]);
					if (calls)
						av_agents++;
				}		
								
				for (i=0; i<s->agents;i++) {
					calls = s->agents - 
							POPCOUNT(s->fixed_name_secrets[i]);
					if (calls)
						for (j=0; j<s->agents;j++) 
							prob = prob +
									( (float) child->calls[i][j]) / (calls * av_agents);
				}							
			}
			else {	
				prob = child->calls_to_child;
			
				SWITCH_PROT_NAME(protocol_name, 
					prob = 
						prob / ((s->agents) * (s->agents) - s->edges),
					prob = 
						(2 * prob) / 
						( (s->agents) * (s->agents - 1) ) );
			}									 
		}
	}		
		
	return prob;	
}

void init_markov_chain
(twin_queues hash[MAXN*MAXN], int agents, int protocol_name)
{
	int i;
	graph init_secrets [MAXN*MAXM];
	protocol_state_t *s;
		
	FOR_ALL_EDGES(i, agents){
		hash[i].can_lab_queue = 
			new_queue(MAXSTATES, cmp_can_secrets);
		hash[i].fixed_name_queue = 
			new_queue(MAXSTATES, cmp_fixed_name_secrets);
	}
	
	init_secrets_graph(init_secrets, agents);
	s =	new_protocol_state(init_secrets, agents, protocol_name);
	enqueue_to_hash(hash, NULL, NULL, s, NULL, protocol_name);
}

float find_expectation
(int agents, int* no_states, int protocol_name, int calc_exp, 
 int rand_ag)
{		
	twin_queues hash[MAXN*MAXN];
	protocol_state_t *s;
	int i, j, label;
	protocol_state_t** trans_matrix;
	struct queue_node_t * p;
	float* expect_vec;
	float result = 0;
	//~ clock_t start, end;	
	
	/* create the lists in the markov chain and
	 * add the initial state				    */
	init_markov_chain(hash, agents, protocol_name);
			
	/* build the markov chain */
	build_the_markov_chain(hash, agents, 
		protocol_name, calc_exp, no_states);
	
	if (calc_exp)
	{		
		MALLOC_SAFE( trans_matrix,
				(* no_states) * sizeof(protocol_state_t *) );
		 	
		/* label the states */
		label = 0;
		
		FOR_ALL_EDGES(i, agents)
			QUEUE_FOREACH(p, hash[i].can_lab_queue) {
				s = (protocol_state_t *) (p->data);
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
					get_prob(trans_matrix, i, j, protocol_name, rand_ag) * 
						expect_vec[j];
		
			if (protocol_name == ANY)			       
				expect_vec[i] = 
					expect_vec[i] / 
					(1-get_prob(trans_matrix, i, i, protocol_name, rand_ag));
		}
		
		result = expect_vec[0];
		
		//~ print_expect_vec_and_trans_matrix(*no_states, 
			//~ expect_vec, trans_matrix, agents, protocol_name, rand_ag);
		
		//~ start = clock();
		//~ print_probs_to_absorption (*no_states, trans_matrix,
			//~ agents, protocol_name, rand_ag, (agents*(agents-1))/2,
			//~ hash);		
		//~ end = clock();
		//~ printf("Elapsed time = %f seconds\n", 
			//~ ( (float) end - start )/CLOCKS_PER_SEC );
			
		FREE_SAFE(expect_vec);
		FREE_SAFE(trans_matrix);
		destroy_hash(agents, hash);
	} 
				
	return result;	
}

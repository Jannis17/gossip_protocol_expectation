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
#include "../../nauty26r7/nauty.h"

int enqueue_to_twin_queues
(twin_queues hash[MAXN*MAXN], protocol_state_t* s,
 struct queue_node_t** found, int protocol_name)
{
	struct queue_t* fixed_name_queue = 
		hash[s-> edges -1].fixed_name_queue;
	struct queue_t* can_queue = 
		hash[s-> edges -1].can_lab_queue;
	
	if (search_in_sorted_queue(fixed_name_queue, found, s))
		return DUPLICATE_ITEM;
	
	int result=enqueue_unique_to_sorted_queue(can_queue, found, s);
	
	if (result == NEW_ITEM)
		enqueue_unique_to_sorted_queue(fixed_name_queue, found, s);
	
	return result; 					
}

void add_new_child_to_parent
(protocol_state_t* parent, protocol_state_t* childs_state, int calls)
{
	child_t* new_child;
	
	MALLOC_SAFE(new_child, sizeof(child_t));
	new_child->childs_state_ptr = childs_state;
					
	if( enqueue_unique_to_sorted_queue
		(parent->children, NULL, new_child)
		== DUPLICATE_ITEM ) {
		//INTERNAL_ERROR("Found duplicate child where I shouldn't!");
	} else
		new_child->calls_to_child = calls;
}

void generate_children
(protocol_state_t* parent, int agents, 
 twin_queues hash[MAXN*MAXN], int protocol_name) 
{
	int i, j, calls_to_child;
	graph temp[MAXN*MAXM];
	child_t* found_child;
	struct queue_node_t* queue_node_of_found_child;
	child_t * potential_child;
					
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
		  
		  potential_child = 
			new_child(temp, agents, protocol_name, calls_to_child);
		  		  
		  if ( search_in_sorted_queue( parent->children, 
				 					   &queue_node_of_found_child, 
									   potential_child) )
		  {
			 destroy_child(potential_child); 
			 found_child = (child_t *) queue_node_of_found_child->data;
			 (found_child->calls_to_child) += calls_to_child;
		  }
		  else {		  			
			if ( enqueue_to_twin_queues
				  (hash, potential_child->childs_state_ptr, 
				   &queue_node_of_found_child, protocol_name) 
					== DUPLICATE_ITEM )
				 destroy_child(potential_child);
 		  
			add_new_child_to_parent(parent, 
				queue_node_of_found_child->data, 
				calls_to_child);
		  }		  
	    }
	  }	
}

void build_the_markov_chain
(twin_queues hash[MAXN*MAXN], int agents, int protocol_name)
{
	int i;
	
	struct queue_node_t * p;
	
	printf("Agents = %d\n", agents);
	
	clock_t start, end;
	
	FOR_ALL_EDGES(i, agents) {
		start = clock();
		QUEUE_FOREACH(p, hash[i].can_lab_queue)
			generate_children(p->data, agents, hash, protocol_name);
		end = clock();
		printf("%d secrets in %f seconds\n", i+1 , 
				( (float) end - start )/CLOCKS_PER_SEC);	
	}
}

float get_prob
( protocol_state_t ** trans_matrix, 
  int from, int to, int protocol_name ) 
{
	protocol_state_t * s = trans_matrix[from];
	
	struct queue_node_t *p;
	
	child_t* child;
	
	float enumer, denom;
	
	QUEUE_FOREACH(p, s->children) {
		child = (child_t*) (p-> data);
		if (child->childs_state_ptr->id == to) {
			enumer = child->calls_to_child;
			
			SWITCH_PROT_NAME(protocol_name, 
				denom = (s->agents) * (s->agents) - s->edges,
				denom = ((float) (s->agents) * (s->agents - 1) ) / 2);
				//~ 1 );
							
			return enumer / denom; 
		}
	}		
	
	return 0;	
}


float find_expectation (int agents, int* no_states, int protocol_name)
{		
	twin_queues hash[MAXN*MAXN];
	graph init_secrets [MAXN*MAXM];
	protocol_state_t *s;
	int i, j, label;
	protocol_state_t** trans_matrix;
	struct queue_node_t * p;
	float* expect_vec;
	
	/* add the initial state to the markov chain */	
	init_hash(hash, agents, protocol_name);
	init_secrets_graph(init_secrets, agents);
	s =	new_protocol_state(init_secrets, agents, protocol_name);
	enqueue_to_twin_queues(hash, s ,NULL, protocol_name);
	
	/* build the markov chain */
	build_the_markov_chain(hash, agents, protocol_name);
		
	/* count the states */	
	*no_states = 0;
	
	FOR_ALL_EDGES(i, agents) {
		if ( QUEUE_COUNT(hash[i].can_lab_queue) != 
			 QUEUE_COUNT(hash[i].fixed_name_queue) )
			INTERNAL_ERROR("Queues do not have the same number of elements\n");			 
		*no_states += QUEUE_COUNT(hash[i].can_lab_queue);
	}	
	
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
							
    for(i=(* no_states)-2; i>=0; i--)
    {
        expect_vec[i] = 1;
                                        
        for(j=i+1; j<(* no_states); j++)
            expect_vec[i] += 
				get_prob(trans_matrix, i, j, protocol_name) * 
					expect_vec[j];
					       
        SWITCH_PROT_NAME(protocol_name, 
			do {} while(0), 
			expect_vec[i] = 
				expect_vec[i] / 
				(1-get_prob(trans_matrix, i, i, protocol_name)));
    }

	float result = expect_vec[0];
	
	//~ for (i=0; i < *no_states; i++)
		//~ printf("expectation(%d) = %f\n", i, expect_vec[i]);
	
	//~ printf("Transition Matrix (%d agents)\n", agents);
	
	//~ for (i=0; i < *no_states; i++) {
		//~ for (j=0; j < *no_states; j++)
			//~ printf("%.0f ", get_prob(trans_matrix, i, j, protocol_name));
		//~ printf("\n");
	//~ }
	
	FREE_SAFE(expect_vec);
	FREE_SAFE(trans_matrix);
	destroy_hash(agents, hash);
				
	return result;	
}

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "queue.h"
#include "../../nauty26r7/nauty.h"

/* needed for qsort*/
int comp_nodes (const void *p, const void *q) {
    graph x = *(const graph *)p;
    graph y = *(const graph *)q;

    /* Avoid return x - y, which can cause undefined behaviour
       because of signed integer overflow. */
    if (x < y)
        return 1;  // Return -1 if you want ascending, 1 if you want descending order. 
    else if (x > y)
        return -1;   // Return 1 if you want ascending, -1 if you want descending order. 

    return 0;
}

/* compares the secrets of the args lexicographically */
int comp_protocol_states(const void* item1, const void* item2)
{
	protocol_state_t* state1, *state2;
			
	state1 = (protocol_state_t *) item1;
	state2 = (protocol_state_t *) item2;
	
	return comp_graphs(state1->can_secrets, 
			state2->can_secrets, 
			state1->agents);
}

/* compares the secrets of the args lexicographically */
int comp_children(const void* item1, const void* item2)
{
	child_t* child1, *child2;
	
	child1 = (child_t *) item1;
	child2 = (child_t *) item2;
	
	return comp_graphs(child1->childs_state_ptr->can_secrets, 
			child2->childs_state_ptr->can_secrets, 
			child1->childs_state_ptr->agents);
}

/* returns a pointer to a protocol_state_t with g in canonical form */
protocol_state_t* new_protocol_state
(graph g[MAXN*MAXM], int agents, int protocol_name)
{
	protocol_state_t* s;
	
	MALLOC_SAFE(s, sizeof(protocol_state_t));
	
	copy_graph(s->init_secrets, g, agents);
	
	//~ print_graph(g, agents);
	
	if (protocol_name == ANY) {
		copy_graph(s->init_secrets_sorted, g, agents);
		qsort(s->init_secrets_sorted, agents*MAXM, 
			sizeof(graph), comp_nodes);
	}
	
	//~ print_graph(g, agents);
			
	find_can_label(g, s->can_secrets, agents); 
	
	//~ print_graph(s->can_secrets, agents);
			
	s->children = new_queue(MAXN*(MAXN-1), comp_children);
	s->id = 0;
	s->agents = agents;
							
	return s;		
}

void init_hash
(twin_queues hash[MAXN*MAXN], int agents, int protocol_name)
{
	int i;
	
	graph init_secrets [MAXN*MAXM];
	
	/* create the queues */		
	FOR_ALL_EDGES(i, agents)
		hash[i].can_lab_queue = new_queue(MAXSTATES, comp_protocol_states);
	
	/* we add the first state into the hash */				
	init_secrets_graph(init_secrets, agents);
	
	protocol_state_t* init_state = 
		new_protocol_state(init_secrets, agents, protocol_name);
	
	struct queue_t* init_queue = 
		hash[edges_of(init_secrets, agents)-1].can_lab_queue;
		
	enqueue_unique_to_sorted_queue(init_queue, NULL, init_state);
}

void add_new_child_to_parent
(protocol_state_t* parent, protocol_state_t* childs_state, int calls)
{
	child_t* new_child;
	
	MALLOC_SAFE(new_child, sizeof(child_t));
	new_child->childs_state_ptr = childs_state;
					
	if( enqueue_unique_to_sorted_queue
		(parent->children, NULL, new_child)
		== DUPLICATE_ITEM ) 
	{
		printf("Internal Error: Find duplicate\
		child where I shouldn't!\n");
		exit(1);
	} else
		new_child->calls_to_child = calls;
}

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

void generate_children
( protocol_state_t* parent, int agents, twin_queues hash[MAXN*MAXN],
  int protocol_name ) 
{
	int i, j, calls_to_child;
		
	graph temp[MAXN*MAXM];
			
	child_t* foundChild;
	
	struct queue_node_t* childsQueueNode;
	
	struct queue_node_t* childsStructPtr;
		
	struct queue_t* childsList;
	
	child_t * potential_child;
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    SWITCH_PROT_NAME(protocol_name, 
	    	calls_to_child = poss_calls(parent->init_secrets, i, j),
	    	calls_to_child = 1);
	    	
	    if ( calls_to_child > 0 )
		{
		  copy_graph(temp, parent->init_secrets, agents);
		  make_call(temp, i ,j); 
		  
		  potential_child = 
			new_child(temp, agents, protocol_name, calls_to_child);
		  		  
		  if (search_in_sorted_queue( parent->children, 
									  &childsStructPtr, 
									  potential_child) )
		  {
			 FREE_SAFE(potential_child); 
			 foundChild = (child_t *) childsStructPtr->data;
			 
			 (foundChild->calls_to_child) += calls_to_child;
		  }
		  else {
		  	childsList = hash[edges_of(temp, agents)-1].can_lab_queue;
		
			if ( enqueue_unique_to_sorted_queue(childsList, 
					&childsQueueNode, potential_child->childs_state_ptr) 
					== DUPLICATE_ITEM )
				 destroy_child(potential_child);
 		  
			add_new_child_to_parent(parent, childsQueueNode->data, 
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
	
	//~ printf("Agents = %d\n", agents);
	
	//~ clock_t start, end;
	
	FOR_ALL_EDGES(i, agents) {
		//~ start = clock();
		QUEUE_FOREACH(p, hash[i].can_lab_queue)
			generate_children(p->data, agents, hash, protocol_name);
		//~ end = clock();
		//~ printf("%d secrets in %f seconds\n", i+1 , 
				//~ ( (float) end - start )/CLOCKS_PER_SEC);	
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
				denom = (s->agents) * (s->agents) - 
							edges_of(s->can_secrets, s->agents),
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
	
	init_hash(hash, agents, protocol_name);
	
	build_the_markov_chain(hash, agents, protocol_name);
	
	int i, j;
	
	*no_states = 0;
		
	/* count the states */	
	FOR_ALL_EDGES(i, agents)
		*no_states += QUEUE_COUNT(hash[i].can_lab_queue);
	
	protocol_state_t** trans_matrix;
	
	MALLOC_SAFE( trans_matrix, 
				 (* no_states) * sizeof(protocol_state_t *) );
		 	
	struct queue_node_t * p;
	
	protocol_state_t *s;
	
	/* label the states */	
	int label = 0;
	
	FOR_ALL_EDGES(i, agents)
		QUEUE_FOREACH(p, hash[i].can_lab_queue) {
			s = (protocol_state_t *) (p->data);
			trans_matrix[label] = s;
			s->id = label++;
		}

	float* expect_vec;
		
	MALLOC_SAFE(expect_vec, *no_states * sizeof(float));
	
	expect_vec[*no_states-1]= 0;
							
    /* this loop is for backward substitution*/
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
	
	//~ printf("\nMarkov chain (%d agents)\n", agents);
		
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
		
	return result;	
}

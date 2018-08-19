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
#include "../../nauty/nauty.h"
				
void
generate_children
(pstate_t *parent, pars_t pars, twin_queues hash[MAXN*MAXN], 
 twin_queues ordered_hash[MAXN*MAXN]) 
{
	if (parent->is_absorption)
		return;
	
	int i, j, calls_to_child, temp_prot;
	graph temp_secrets[MAXN*MAXM];
	int temp_calls[MAXN][MAXN];
	int temp_token[MAXN];
	child_t* found_child;
	struct queue_node_t* found_child_node;
	struct queue_node_t* child_pos_in_par_list;
	struct queue_node_t* childs_can_prev = NULL;
	struct queue_node_t* childs_fixed_name_prev = NULL;
	child_t * potential_child;
	pstate_t* childs_state;
	int exists_in_hash;
	pstate_t* state_for_ordered_hash;
						
	for (i=0; i<pars.n; i++)
	 for (j= (pars.prot == SPI || pars.prot == TOK)? 0: i+1; j<pars.n; j++)
	  if ( (i!=j) && 
		(calls_to_child = no_poss_calls(parent,i,j,pars)) > 0 )
	  {
		//create the new secrets graph  
		copy_graph(temp_secrets, parent->fixed_name_secrets, pars.n, pars.m);
		update_secrets(temp_secrets, i ,j, pars.n, pars.m);
		
		//create the new calls graph  
		copy_calls_graph(temp_calls,parent-> fixed_name_calls,pars.n);
		temp_calls[i][j]=temp_calls[j][i]=parent->total_calls+1;
		
		//copy the tokens
		copy_tokens(parent->token, temp_token, pars.n);
		
		if (pars.prot==TOK) {
			temp_token[i]=0;
			temp_token[j]=1;
		}
		
		if (pars.prot==SPI) {
			temp_token[i]=1;
			temp_token[j]=0;
		}

		//create a new child state 		  
		childs_state = new_pstate(temp_secrets,temp_calls, temp_token,
			parent->total_calls+1, pars);
				
		//add the child to the parent  
		potential_child = new_child(childs_state, calls_to_child);
		  
		//needed only for rand over agents
		potential_child->calls[i][j] = 
			can_call( parent->fixed_name_secrets, i, j, pars.n, pars.m);
		potential_child->calls[j][i] = 
			can_call(parent->fixed_name_secrets, j, i, pars.n, pars.m);
		  		  
		if(search_in_twin_queues(parent->children, NULL, NULL,
			&childs_fixed_name_prev, &childs_can_prev,
			&child_pos_in_par_list, potential_child, pars))
		{
		  FREE_SAFE(potential_child); 
		  destroy_protocol_state(&childs_state);
		  found_child = (child_t *) child_pos_in_par_list->data;
		  found_child->calls_to_child += calls_to_child;
			 
		  //needed only for rand over agents
		  found_child->calls[i][j] = 
			can_call(parent->fixed_name_secrets, i, j, pars.n, pars.m);
		  found_child->calls[j][i] = 
			can_call(parent->fixed_name_secrets, j, i, pars.n, pars.m);
		} 
		else 
		{ 
		  exists_in_hash = enqueue_to_hash
			(hash, NULL, NULL, childs_state, &found_child_node, pars);
		  if ( exists_in_hash == DUPLICATE_ITEM ) 
		  {
			destroy_protocol_state(&childs_state);
			potential_child->childs_state = found_child_node->data;
		  }
		  enqueue_unique_to_twin_queues
		   ( parent->children, NULL, NULL, potential_child, pars);
		  
		  /* if we are in CO, TOK or SPI we want to count 
		   * the ordered tuples too */		  
		  if ( (pars.prot == CO || pars.prot == TOK 
				|| pars.prot == SPI) && exists_in_hash == NEW_ITEM )
		  {
			  temp_prot = pars.prot;
			  pars.prot =LNS;
			  state_for_ordered_hash= 
			  	new_pstate(temp_secrets,temp_calls,temp_token,
			  		parent->total_calls+1, pars);
			  enqueue_to_hash
				(ordered_hash, NULL, NULL, state_for_ordered_hash, 
					NULL, pars);
			  pars.prot = temp_prot;						
		  }
		        
		}		  
	  }		  
}	

void 
build_the_markov_chain
(twin_queues hash[MAXN*MAXN], int* no_states, pars_t pars, 
 twin_queues ordered_hash[MAXN*MAXN])
{
	int i;
	struct queue_node_t * p;
	*no_states = 0;
	
	//~ printf("%d agents:\n", pars.n);
		
	FOR_ALL_EDGES(i, pars.n) {
		//~ printf("\nEdges: %d, ", i);		
		QUEUE_FOREACH(p, hash[i].can_lab_queue)
			generate_children
				(p->data, pars, hash, ordered_hash);

		//~ if ( pars.prot == ANY && pars.o_ANY &&
			 //~ QUEUE_COUNT(hash[i].can_lab_queue) != 
			 //~ QUEUE_COUNT(hash[i].fixed_name_queue) ) 
		//~ {
			//~ printf("canonical = %lu	\n", 
				//~ QUEUE_COUNT(hash[i].can_lab_queue));
			//~ printf("fixed name = %lu\n", 
				//~ QUEUE_COUNT(hash[i].fixed_name_queue));
		   //~ INTERNAL_ERROR("Queues do not have the same number of elements\n");			 
		//~ }
		
		/* count the states */	
		*no_states += QUEUE_COUNT(hash[i].can_lab_queue);
		//~ printf("states: %lu", QUEUE_COUNT(hash[i].can_lab_queue));
		if (!pars.calc_exp)
			destroy_twin_queues(&hash[i]);		
	}
	//~ printf("\n");
}

int
count_ordered_tuples
(twin_queues ordered_hash[MAXN*MAXN], int n)
{
	int result = 0;
	int i;
			
	FOR_ALL_EDGES(i, n) {
		result += QUEUE_COUNT(ordered_hash[i].can_lab_queue);
		//~ destroy_twin_queues(&ordered_hash[i]);
	}
	
	return result;	
}

float
get_prob
(pstate_t ** trans_matrix, int from, int to, int prot) 
{
	pstate_t* s = trans_matrix[from], * t;
	struct queue_node_t *p;
	child_t* child;	
	float prob = 0;
	
	QUEUE_FOREACH(p, s->children.can_lab_queue) {
		child = (child_t*) (p-> data);
		t = (pstate_t*) child->childs_state;
		
		if (t->id == to) {
			prob = child->calls_to_child;			
			
			switch (prot) {
				case (ANY):
					prob /= ((s->n) * (s->n - 1));
					break;
				case (LNS):
					prob /= ((s->n) * (s->n) - s->total_secrets);
					break;
				case (CO):
					prob /= 
						((s->n) * (s->n -1 ) - 2 * s-> total_calls);
					break;
				case (TOK):
				case (SPI):
					prob /= 
						(count_tokens(s ->token, s->n) * (s->n -1));
					break;
			}									 
		}
	}		
		
	return prob;	
}

void
init_markov_chain
(twin_queues hash[MAXN*MAXN], pars_t pars)
{
	int i;
	graph init_secrets [MAXN*MAXM];
	int init_calls [MAXN][MAXN];
	int init_token[MAXN];
	pstate_t *s;
		
	FOR_ALL_EDGES(i, pars.n){
		switch (pars.prot) {
			case (LNS):
				hash[i].can_lab_queue=
					new_queue(MAXSTATES, cmp_can_secrets);
				hash[i].fixed_name_queue=NULL;
			break;
			case (ANY):
			case (TOK):
			case (SPI):
				hash[i].can_lab_queue=
					new_queue(MAXSTATES, cmp_can_secrets);
				hash[i].fixed_name_queue=
					new_queue(MAXSTATES, cmp_fixed_name_secrets);
			break;
			case (CO):
				hash[i].can_lab_queue=
					new_queue(MAXSTATES, cmp_can_calls);
				hash[i].fixed_name_queue=NULL;
			break;	
		}
	}
	
	diagonal(init_secrets, pars.n, pars.m);
	init_calls_graph(init_calls, pars.n);
	
	for(i=0;i<pars.n;i++)
		init_token[i]=1;
	
	s=new_pstate(init_secrets, init_calls, init_token, 0, pars);
	enqueue_to_hash(hash, NULL, NULL, s, NULL, pars);
}

float 
exact_expectation
(int* no_states, int * no_ordered_tuples, pars_t pars)
{		
	twin_queues hash[MAXN*MAXN];
	pstate_t *s;
	int i, j, label,  temp_prot;
	pstate_t** trans_matrix = NULL;
	struct queue_node_t * p;
	float* expect_vec = NULL;
	int* is_absorption = NULL;
	float result = 0;	
	//this hash will store only ordered tuples, regardless the protocol
	twin_queues ordered_hash[MAXN*MAXN];
	
	init_markov_chain(hash, pars);
				
	/* if we are in CO, TOK, SPI we want to count the ordered tuples too*/
	temp_prot = pars.prot;
	pars.prot = LNS;
	init_markov_chain(ordered_hash, pars);
	pars.prot = temp_prot;
	
	/* build the markov chain */
	build_the_markov_chain(hash, no_states, pars, ordered_hash);
		
	//compute the expectation	
	if (pars.calc_exp)
	{		
		MALLOC_SAFE(trans_matrix,(*no_states)*sizeof(pstate_t *) );
		 	
		/* label the states */
		label = 0;
		
		MALLOC_SAFE(is_absorption, *no_states * sizeof(int));
		
		FOR_ALL_EDGES(i, pars.n)
			QUEUE_FOREACH(p, hash[i].can_lab_queue) {
				s = (pstate_t *) (p->data);
				trans_matrix[label] = s;
				s->id = label++;
				is_absorption[s->id] = s->is_absorption;
			}
		
		MALLOC_SAFE(expect_vec, *no_states * sizeof(float));
								
		/* compute the expectations, backwards */					
		for(i = (* no_states)-1; i>=0; i--)
			if (is_absorption[i])
				expect_vec[i] = 0;
			else {
				expect_vec[i] = 1;
                                        
				for(j=i+1; j<(* no_states); j++)
					expect_vec[i] += 
						get_prob(trans_matrix, i, j, pars.prot) * 
							expect_vec[j];
		
				if (pars.prot == ANY || pars.prot == SPI || pars.prot == TOK)			       
					expect_vec[i] /= 
						(1-get_prob(trans_matrix, i, i, pars.prot));
			}
		
		result = expect_vec[0];
	}
	
	//if we are in CO, TOK or SPI we count the ordered tuples
	switch (pars.prot) {
		case CO:
		case TOK: 
		case SPI:
			*no_ordered_tuples = 
				count_ordered_tuples(ordered_hash, pars.n);
			break;
		default:
			break;
	}
			
	FREE_SAFE(is_absorption);
	FREE_SAFE(expect_vec);
	FREE_SAFE(trans_matrix);
	
	if (pars.calc_exp)
		destroy_hash(pars.n, hash);
	
	//~ printf("Oredered hash starts\n");
		 
	destroy_hash(pars.n, ordered_hash);
				
	return result;	
}

#ifndef H_STATE_H_
#define H_STATE_H_

#include "../../nauty/nauty.h"

#define MAXSTATES 100000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= 0; i < (AGENTS) * (AGENTS); (VAR)++)

typedef struct twin_queues_tag {
	struct queue_t* fixed_name_queue;
	struct queue_t* can_lab_queue;
} twin_queues;

typedef struct pstate_tag {
	graph can_secrets[MAXN*MAXM];
	graph fixed_name_secrets[MAXN*MAXM];
	graph fixed_name_secrets_sorted[MAXN*MAXM];
	int fixed_name_calls[MAXN][MAXN];
	graph can_calls[MAXN*MAXM];
	int id;
	//~ int total_tokens;
	int token[MAXN];
	int nl; //number of agents/bits in the layered graph
	int ml; //number of setwords in the layered graph
	int n; //number of agents/bits
	int m; //number of setwords
	int prot;
	int total_secrets;
	int total_calls;
	int is_absorption;
	twin_queues children;
} pstate_t;

typedef struct child_tag {
	int calls_to_child;
	int calls[MAXN*MAXM][MAXN*MAXM];
	pstate_t* childs_state;
} child_t;

float 
exact_expectation
(int* no_states, int * no_ordered_tuples, pars_t pars);

float 
get_prob
(pstate_t ** trans_matrix, int from, int to, int prot, int rand_ag);


#endif /* H_STATE_H_ */

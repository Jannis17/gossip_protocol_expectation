#ifndef H_STATE_H_
#define H_STATE_H_

#define MAXSTATES 100000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

typedef struct twin_queues_tag {
	struct queue_t* fixed_name_queue;
	struct queue_t* can_lab_queue;
} twin_queues;

typedef struct pstate_tag {
	graph can_secrets[MAXN*MAXM];
	graph fixed_name_secrets[MAXN*MAXM];
	graph fixed_name_secrets_sorted[MAXN*MAXM];
	graph can_calls[MAXN*MAXM];
	int id;
	int agents;
	int total_secrets;
	twin_queues children;
} pstate_t;

typedef struct child_tag {
	int calls_to_child;
	int calls[MAXN*MAXM][MAXN*MAXM];
	pstate_t* childs_state;
} child_t;

float find_expectation
(int agents, int* no_states, int prot, int calc_exp, int rand_ag);

float get_prob
(pstate_t ** trans_matrix, int from, int to, int prot, int rand_ag);


#endif /* H_STATE_H_ */

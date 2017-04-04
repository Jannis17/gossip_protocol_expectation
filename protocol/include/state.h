#ifndef H_STATE_H_
#define H_STATE_H_

#define MAXSTATES 100000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

float find_expectation (int agents, int* no_states, int protocol_name);

typedef struct twin_queues_tag {
	struct queue_t* fixed_name_queue;
	struct queue_t* can_lab_queue;
} twin_queues;

/* State of the protocol. The graph secrets contains the
 * distribution of secrets */
typedef struct protocol_state_tag {
	graph can_secrets[MAXN*MAXM];
	graph fixed_name_secrets[MAXN*MAXM];
	graph fixed_name_secrets_sorted[MAXN*MAXM];
	int id;
	int agents;
	int edges;
	twin_queues children;
} protocol_state_t;

/* child of a given state */
typedef struct child_tag {
	int calls_to_child;
	protocol_state_t* childs_state_ptr;
} child_t;

#endif /* H_STATE_H_ */

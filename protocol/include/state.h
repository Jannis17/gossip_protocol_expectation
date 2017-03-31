#ifndef H_STATE_H_
#define H_STATE_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

#define LESS -1
#define EQUAL 0
#define GREATER 1

#define MAXSTATES 10000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

float find_expectation (int agents, int* no_states, int protocol_name);

/* State of the protocol. The graph secrets contains the
 * distribution of secrets */
typedef struct protocol_state_tag {
	graph can_secrets[MAXN*MAXM];
	graph init_secrets[MAXN*MAXM];
	graph init_secrets_sorted[MAXN*MAXM];
	int id;
	int agents;
	struct queue_t* children;
} protocol_state_t;

/* child of a given state */
typedef struct child_tag {
	int calls_to_child;
	protocol_state_t* childs_state_ptr;
} child_t;

typedef struct hash_tag {
	struct queue_t* initial_hash;
	struct queue_t* can_hash;
} hash_t;

#endif /* H_STATE_H_ */

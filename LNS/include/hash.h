#ifndef H_COMPARE_H_
#define H_COMPARE_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

#define LESS -1
#define EQUAL 0
#define GREATER 1

#define MAXSTATES 10000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

typedef struct LNSstate_tag {
	graph secrets[MAXN*MAXM];
	int id;
	int agents;
	struct queue_t* children;
} LNSstate_t;

typedef struct child_tag {
	int callsToChild;
	struct queue_node_t* childPtr;
} child_t;

#endif /*H_COMPAR_H_*/

#include "queue.h"
#include "graph.h"
#include "state.h"

/* compares the secrets of the args lexicographically */
int compStates(const void* item1, const void* item2)
{
	LNSstate_t* state1, *state2;
	
	state1 = (LNSstate_t *) item1;
	state2 = (LNSstate_t *) item2;
	
	return compGraphs(state1->secrets, state2->secrets, state1->agents);
}

/* compares the ids of the args lexicographically */
int compStateIds(const void* item1, const void* item2)
{
	LNSstate_t* state1, *state2;
	
	state1 = (LNSstate_t *) item1;
	state2 = (LNSstate_t *) item2;
	
	if (state1->id < state2->id)
		return LESS;
	
	if (state1->id > state2->id)
		return GREATER;
	
	return EQUAL;
}

/* compares the secrets of the args lexicographically */
int compChildren(const void* item1, const void* item2)
{
	child_t* child1, *child2;
	
	child1 = (child_t *) item1;
	child2 = (child_t *) item2;
	
	LNSstate_t* state1, * state2;
	
	state1 = (LNSstate_t *) child1->childPtr->data;
	state2 = (LNSstate_t *) child2->childPtr->data;
	
	return compGraphs(state1->secrets, state2->secrets, state1->agents);
}


/* compares g1 and g2 lexicographically
 * return value: 
 * LESS iff g1 < g2
 * EQUAL iff g1 == g2 
 * GREATER iff g1 > g2 */
int compGraphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n) {
	size_t k;
	
	for (k = 0; k < MAXM*(size_t)n; ++k) {
		 if (g1[k] < g2[k])
			return LESS;
		 if (g1[k] > g2[k])
			return GREATER;
	}
	
	return EQUAL;
}

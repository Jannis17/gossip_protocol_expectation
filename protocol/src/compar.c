#include "main.h"
#include "state.h"
#include "graph.h"
#include "compar.h"

/* compares g1 and g2 lexicographically
 * return value: 
 * LESS iff g1 < g2
 * EQUAL iff g1 == g2 
 * GREATER iff g1 > g2 */
int cmp_graphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n) 
{
	size_t k;
	
	for (k = 0; k < MAXM*(size_t)n; ++k) {
		 if (g1[k] < g2[k])
			return LESS;
		 if (g1[k] > g2[k])
			return GREATER;
	}
	
	return EQUAL;
}

/* needed for qsort*/
int cmp_graph_nodes (const void *p, const void *q) {
    graph x = *(const graph *)p;
    graph y = *(const graph *)q;

    /* Avoid return x - y, which can cause undefined behaviour
       because of signed integer overflow. */
    if (x < y)
        return -1;  // Return -1 for ascending, 1 for descending order 
    else if (x > y)
        return 1;   // Return 1 for ascending, -1 for descending order 

    return 0;
}

/* compares the canonical secrets of the args */
int cmp_can_secrets(const void* item1, const void* item2)
{
	pstate_t* state1, *state2;
			
	state1 = (pstate_t *) item1;
	state2 = (pstate_t *) item2;
	
	return cmp_graphs(state1->can_secrets, state2->can_secrets, 
			state1->agents);
}

/* compares the fixed name secrets (as unordered tuple ) of the args */
int cmp_fixed_name_secrets(const void* item1, const void* item2)
{
	pstate_t* state1, *state2;
			
	state1 = (pstate_t *) item1;
	state2 = (pstate_t *) item2;
	
	return cmp_graphs(state1->fixed_name_secrets_sorted, 
			state2->fixed_name_secrets_sorted, 
			state1->agents);
}

int cmp_can_children(const void* item1, const void* item2)
{
	child_t* child1, *child2;
	
	child1 = (child_t *) item1;
	child2 = (child_t *) item2;
	
	return cmp_graphs(child1->childs_state->can_secrets, 
			child2->childs_state->can_secrets,
			child1->childs_state->agents);
}

int cmp_fixed_name_children(const void* item1, const void* item2)
{
	child_t* child1, *child2;
	
	child1 = (child_t *) item1;
	child2 = (child_t *) item2;
	
	return cmp_graphs(child1->childs_state->fixed_name_secrets_sorted, 
			child2->childs_state->fixed_name_secrets_sorted,
			child1->childs_state->agents);
}

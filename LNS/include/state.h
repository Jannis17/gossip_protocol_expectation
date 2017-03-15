#ifndef H_STATE_H_
#define H_STATE_H_

#include "../../nauty26r7/nauty.h"

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

/* maximum nuber of isomorphic states */
#define MAXSTATES 10000000

/* totalStates[i] = number of different states for i agents */
int no_states[MAXN];

float findExpectation (int);

#endif /* H_STATE_H_ */

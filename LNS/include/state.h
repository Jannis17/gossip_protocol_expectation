#ifndef H_STATE_H_
#define H_STATE_H_

#include "../../nauty26r7/nauty.h"

/* maximum nuber of isomorphic states */
#define MAXSTATES 10000

/* totalStates[i] = number of different states for i agents */
int totalStates[MAXN];

float findExpectation (int);

#endif /* H_STATE_H_ */

#ifndef H_TEST_H_
#define H_TEST_H_

#include "../../nauty/nauty.h"
#include "state.h"

void
graph_test(int n, int m);

void
print_trans_matrix
(float**tm, int n);

void 
print_expect_vec_and_trans_matrix
(int no_states, float* expect_vec, pstate_t** trans_matrix, int n,
 int prot);

void 
multiply_matrices
(float** a, float** b, float** c, int n);

void
print_probs_to_absorption
(int no_states, pstate_t** trans_matrix, int agents, int prot,
 int max_calls, twin_queues hash[MAXN*MAXN]);

void counterexample(int n);

#endif /* H_TEST_H_ */

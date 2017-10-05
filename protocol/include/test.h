#ifndef H_TEST_H_
#define H_TEST_H_

#include "../../nauty26r7/nauty.h"
#include "state.h"

void print_graph(graph g[MAXN*MAXM], int n);

void graph_test(int n);

void print_expect_vec_and_trans_matrix
(int no_states, float* expect_vec, pstate_t** trans_matrix,
 int agents, int prot, int rand_ag);

void multiply_matrices(float** a, float** b, float** c, int n);

void print_probs_to_absorption 
(int no_states, pstate_t** trans_matrix,
 int agents, int prot, int rand_ag, int max_calls,
 twin_queues hash[MAXN*MAXN]);

#endif /* H_TEST_H_ */

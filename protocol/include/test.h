#ifndef H_TEST_H_
#define H_TEST_H_

#include "../../nauty26r7/nauty.h"
#include "state.h"

void print_graph(graph g[MAXN*MAXM], int n);

void graph_test(int n);

void print_expect_vec_and_trans_matrix
(int no_states, float* expect_vec, protocol_state_t** trans_matrix,
 int agents,
 int protocol_name, int rand_ag);

#endif /* H_TEST_H_ */

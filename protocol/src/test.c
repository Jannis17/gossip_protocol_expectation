#include "graph.h"
#include "test.h"
#include "state.h"
#include "memory.h"

/* prints the contexts of g in hex form */
void print_graph(graph g[MAXN*MAXM], int n)
{
	size_t i;
	
	for (i=0; i < MAXM*(size_t) n ; i++){
		printf(SETWORD_FORMAT, g[i]);
		printf("\n");
	}
	
	printf("\n");
}

void print_trans_matrix(float**tm, int n)
{
	int i,j;
	
	for(i=0;i<n;i++) {
		for(j=0;j<n;j++)
			printf("%f ", tm[i][j]);
			printf("\n");
	}
}

void print_expect_vec_and_trans_matrix
(int no_states, float* expect_vec, protocol_state_t** trans_matrix,
 int agents, int protocol_name, int rand_ag)
{
	int i,j;
	
	for (i=0; i < no_states; i++)
		printf("expectation(%d) = %f\n", i, expect_vec[i]);
	printf("\n");
			
	printf("Transition Matrix (%d agents)\n", agents);
	
	for (i=0; i < no_states; i++) {
		for (j=0; j < no_states; j++)
			printf("%f ", get_prob(trans_matrix, i, j, protocol_name, 
				rand_ag));	
		printf("\n");
	}
}

void graph_test(int n)
{
	graph g1[MAXN*MAXM];
	graph g2[MAXN*MAXM];
	graph g3[MAXN*MAXM];
	graph g4[MAXN*MAXM];
	
	int i = 2 , j = 0;
		
	init_secrets_graph(g1, n);
	init_secrets_graph(g2, n);
		
	make_call(g1, 0, 1);
	//~ make_call(g1, 2, 3);
	//~ make_call(g1, 0, 2);
	//~ make_call(g1, 1, 3);	
	print_graph(g1, n);
	
	find_can_label(g1, g3, n);
	
	print_graph(g3, n);
	
	make_call(g2, 3, 0);
	//~ make_call(g2, 1, 3);
	//~ make_call(g2, 2, 1);
	//~ make_call(g2, 0, 3);
		
	print_graph(g2, n);
	
	find_can_label(g2, g4, n);
	
	print_graph(g4, n);
			
	printf ("edges of g1 = %d \n", edges_of(g1,n));
	
	printf ("edges of g2 = %d \n", edges_of(g2,n));
	
	printf ("poss Calls = %d \n", poss_calls(g1,i,j) );
}

void multiply_matrices(float **c, float** a, float**b, int n)
{
	int i,j,k;
	
	for(i=0;i<n;i++)
		for(j=i;j<n;j++) {
			c[i][j] = 0;
			for(k=0;k<n;k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	
}

void copy_matrix(float **to, float** from, int n)
{
	int i,j;
	
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			to[i][j]=from[i][j];
}
/* prints the probabilities to absorption*/
void print_probs_to_absorption 
(int no_states, protocol_state_t** trans_matrix,
 int agents, int protocol_name, int rand_ag, int max_calls, 
 twin_queues hash[MAXN*MAXN])
{
	int i,j;	
	float **tm1, **tm2, **tm3;
	
	malloc_safe_2D_float(&tm1, no_states); 
	
						
	for(i=0;i<no_states;i++)
		for(j=0;j<no_states;j++)
			tm1[i][j] = 
				get_prob(trans_matrix,i,j,protocol_name, rand_ag);
	
	FREE_SAFE(trans_matrix);
	destroy_hash(agents, hash);
	
	malloc_safe_2D_float(&tm2, no_states);
	malloc_safe_2D_float(&tm3, no_states);
	
	for(i=0;i<no_states;i++)
		for(j=0;j<no_states;j++)
			tm2[i][j] = tm1[i][j];
	
	tm1[no_states-1][no_states-1]=tm2[no_states-1][no_states-1]=1;
	
	printf("Probabilities to reach absorption after:\n");
		
	for(i=2; i <= max_calls; i++) {
		multiply_matrices(tm3, tm2,tm1, no_states);
		printf("%d calls = %f\n", i, tm3[0][no_states-1]);
		//~ print_trans_matrix(tm3, no_states);
		copy_matrix(tm2, tm3, no_states);
	}
		
	free_safe_2D_float(&tm1, no_states);
	free_safe_2D_float(&tm2, no_states);
	free_safe_2D_float(&tm3, no_states);
}

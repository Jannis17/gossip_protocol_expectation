#include <math.h>
#include "main.h"
#include "graph.h"
#include "test.h"
#include "state.h"
#include "memory.h"
#include "compar.h"

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
(int no_states, float* expect_vec, pstate_t** trans_matrix,
 int agents, int prot, int rand_ag)
{
	int i,j;
	
	for (i=0; i < no_states; i++)
		printf("expectation(%d) = %f\n", i, expect_vec[i]);
	printf("\n");
			
	printf("Transition Matrix (%d agents)\n", agents);
	
	for (i=0; i < no_states; i++) {
		for (j=0; j < no_states; j++)
			printf("%f ", get_prob(trans_matrix, i, j, prot, 
				rand_ag));	
		printf("\n");
	}
}


void graph_test(int n, int m)
{
	//~ graph g1[MAXN*MAXM];
	//~ graph g2[MAXN*MAXM];
	//~ graph g3[MAXN*MAXM];
	//~ graph g4[MAXN*MAXM];
		
	//~ diagonal(g1, n);
	//~ diagonal(g2, n);
		
	//~ update_secrets(g1, 0, 1);
	//~ update_secrets(g1, 2, 3);
	//~ update_secrets(g1, 0, 2);
	//~ update_secrets(g1, 1, 3);	
	//~ print_graph(g1, n, m);
	
	//~ find_can_label(g1, g3, n);
	
	//~ print_graph(g3, n, m);
	
	//~ update_secrets(g2, 3, 0);
	//~ update_secrets(g2, 1, 3);
	//~ update_secrets(g2, 2, 1);
	//~ update_secrets(g2, 0, 3);
		
	//~ print_graph(g2, n);
	
	//~ find_can_label(g2, g4, n);
	
	//~ print_graph(g4, n);
			
	//~ printf ("edges of g1 = %d \n", edges_of(g1,n));
	
	//~ printf ("edges of g2 = %d \n", edges_of(g2,n));
	
	
	int c1[MAXN][MAXN], c2[MAXN][MAXN]; 	
	graph can_calls1[MAXM*MAXN];
	
	graph can_calls2[MAXM*MAXN];
		 
	 printf("m=%d, n=%d, MAXN=%d , MAXM=%d\n", m, n, MAXN, MAXM);
		
	 /* The following optional call verifies that we are linking
	 to compatible versions of the nauty routines. */
	 nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
	
	init_calls_graph(c1,n);
	init_calls_graph(c2,n);
	
	c1[0][1] = c1[1][0] = 1;
	c1[2][3] = c1[3][2] = 2;
	c1[0][2] = c1[2][0] = 3;
	c1[3][1] = c1[1][3] = 4;
	
	c2[3][0] = c2[0][3] = 1;
	c2[2][1] = c2[1][2] = 2;
	c2[3][2] = c2[2][3] = 3;
	c2[0][1] = c2[1][0] = 4;
	
	printf("Initial c1\n");
	print_calls_graph(c1,n);
	printf("Initial c2\n");
	print_calls_graph(c2,n);
	
	can_label_calls(c1,can_calls1,n);
	printf("afsafasfa\n");
	can_label_calls(c2,can_calls2,n);
	
		
	printf("%d\n", cmp_graphs(can_calls1,can_calls2,MAXN));
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
(int no_states, pstate_t** trans_matrix,
 int agents, int prot, int rand_ag, int max_calls, 
 twin_queues hash[MAXN*MAXN])
{
	int i,j;	
	float **tm1, **tm2, **tm3, prev;
	
	malloc_safe_2D_float(&tm1, no_states); 
	
						
	for(i=0;i<no_states;i++)
		for(j=0;j<no_states;j++)
			tm1[i][j] = 
				get_prob(trans_matrix,i,j,prot, rand_ag);
	
	FREE_SAFE(trans_matrix);
	destroy_hash(agents, hash);
	
	malloc_safe_2D_float(&tm2, no_states);
	malloc_safe_2D_float(&tm3, no_states);
	
	for(i=0;i<no_states;i++)
		for(j=0;j<no_states;j++)
			tm2[i][j] = tm1[i][j];
	
	tm1[no_states-1][no_states-1]=tm2[no_states-1][no_states-1]=1;
	
	printf("Probabilities to reach absorption after:\n");
	
	prev = 0;	
	for(i= 2; i <= max_calls; i++) {
		multiply_matrices(tm3, tm2,tm1, no_states);
		if (i >= 2 * agents -4)
			printf("%d calls = %f\n", i, tm3[0][no_states-1]-prev);
		prev = tm3[0][no_states-1];
		//~ print_trans_matrix(tm3, no_states);
		copy_matrix(tm2, tm3, no_states);
	}
		
	free_safe_2D_float(&tm1, no_states);
	free_safe_2D_float(&tm2, no_states);
	free_safe_2D_float(&tm3, no_states);
}

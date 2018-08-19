#include <math.h>
#include "main.h"
#include "graph.h"
#include "test.h"
#include "state.h"
#include "memory.h"
#include "compar.h"

typedef struct call_pair_tag {
	int a;
	int b;
} call_pair_t;


void print_trans_matrix(float**tm, int n)
{
	int i,j;
	
	for(i=0;i<n;i++) {
		for(j=0;j<n;j++)
			printf("%f ", tm[i][j]);
			printf("\n");
	}
}

void 
print_expect_vec_and_trans_matrix
(int no_states, float* expect_vec, pstate_t** trans_matrix,
 int agents, int prot)
{
	int i,j;
	
	for (i=0; i < no_states; i++)
		printf("expectation(%d) = %f\n", i, expect_vec[i]);
	printf("\n");
			
	printf("Transition Matrix (%d agents)\n", agents);
	
	for (i=0; i < no_states; i++) {
		printf("%d: ", i);
		for (j=0; j < no_states; j++)
			printf("%.1f ", get_prob(trans_matrix, i, j, prot));	
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
	
	int c1[MAXN][MAXN], c2[MAXN][MAXN]; 	
	graph can_calls1[MAXM*MAXN];
	
	graph can_calls2[MAXM*MAXN];
		 
	 printf("m=%d, n=%d, MAXN=%d , MAXM=%d\n", m, n, MAXN, MAXM);
		
	 /* The following optional call verifies that we are linking
	 to compatible versions of the nauty routines. */
	 nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
	
	init_calls_graph(c1,n);
	init_calls_graph(c2,n);

	c1[0][1]=c1[1][0]=1;
	c1[3][2]=c1[2][3]=2;
	c1[2][0]=c1[0][2]=3;
	c1[1][3]=c1[3][1]=4;
	
	/*
	0 1 3 0 
	1 0 0 4 
	3 0 0 2 
	0 4 2 0 */

	
	c2[0][1]=c2[1][0]=1;
	c2[0][2]=c2[2][0]=2;
	c2[1][3]=c2[3][1]=3;
	c2[2][3]=c2[3][2]=4;
	
	/*
0 1 2 0 
1 0 0 3 
2 0 0 4 
0 3 4 0 	*/
		
	printf("Initial c1\n");
	print_calls_graph(c1,n);
	printf("Initial c2\n");
	print_calls_graph(c2,n);
	
	int nl = n * ceil(log2( ( n * (n-1) ) /2 +1 ));
	int ml = SETWORDSNEEDED(nl);
	
	can_label_calls(c1,can_calls1,n, nl, ml);
	can_label_calls(c2,can_calls2,n, nl, ml);
			
	printf("%d\n", cmp_graphs(can_calls1,can_calls2,nl, ml));
	
	//~ printf("Canonical c1\n");
	//~ print_graph(can_calls1,nl,ml);
	//~ printf("Canonical c2\n");
	//~ print_graph(can_calls2,nl,ml);

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
 int agents, int prot, int max_calls, 
 twin_queues hash[MAXN*MAXN])
{
	int i,j;	
	float **tm1, **tm2, **tm3, prev;
	
	malloc_safe_2D_float(&tm1, no_states); 
	
						
	for(i=0;i<no_states;i++)
		for(j=0;j<no_states;j++)
			tm1[i][j] = 
				get_prob(trans_matrix,i,j,prot);
	
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

void copy_1D_array(int *to, int *from, int n)
{
	int i;
	
	for(i=0;i<n;i++)
		to[i]=from[i];
}

void iter_calls
(graph ANY_secrets [MAXN*MAXM],
 graph secrets [MAXN*MAXM], int token[MAXN], int avail_call[MAXN],
 call_pair_t call_seq[11],
 int n, int m)
{
	int i;
	graph secrets_tmp[MAXN*MAXM]; 
	int token_tmp[MAXN];
	int avail_call_tmp[MAXN];
	
	size_t k;
	
	for (k = 0; k < m* (size_t) n && (secrets[k] == ANY_secrets[k]); k++);
	
	if (k == m* (size_t) n) {
		printf("ANY reachable!\n");
		return;
	}	
		
	for(i=0;i<11;i++) 
		if(avail_call[i] && token[call_seq[i].a]) {
			//printf("%d calls %d ", call_seq[i].a,call_seq[i].b);
			copy_graph(secrets_tmp,secrets, n, m);
			update_secrets(secrets_tmp,call_seq[i].a,call_seq[i].b,n,m);
			copy_1D_array(token_tmp,token,n);
			token_tmp[call_seq[i].b]=0;
			copy_1D_array(avail_call_tmp, avail_call, n);
			avail_call_tmp[i]= 0;
			iter_calls
				(ANY_secrets, secrets_tmp, token_tmp, avail_call_tmp,
				call_seq, n, m);
		}
}

void counterexample(int n) 
{
	call_pair_t call_seq[11];
	
	/* store the call sequence 15.20.43.23.45.09.61.87.67.89.03 */
	
	call_seq[0].a = 1;
	call_seq[0].b = 5;
	
	call_seq[1].a = 2;
	call_seq[1].b = 0;
	
	call_seq[2].a = 4;
	call_seq[2].b = 3;
	
	call_seq[3].a = 2;
	call_seq[3].b = 3;
	
	call_seq[4].a = 4;
	call_seq[4].b = 5;
	
	call_seq[5].a = 0;
	call_seq[5].b = 9;
	
	call_seq[6].a = 6;
	call_seq[6].b = 1;

	call_seq[7].a = 8;
	call_seq[7].b = 7;
	
	call_seq[8].a = 6;
	call_seq[8].b = 7;

	call_seq[9].a = 8;
	call_seq[9].b = 9;
	
	call_seq[10].a = 0;
	call_seq[10].b = 3;
		
	/* calculate the number of words needed to hold n bits */
	int m = SETWORDSNEEDED(n);
		 			
	/* verify that we are linking to compatible versions of the
	 * nauty routines. */
	nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

	int token[MAXN];
	graph secrets [MAXN*MAXM];
	diagonal(secrets, n, m);
	
	int i;
	
	//everybody has a token in the beginning
	for(i=0;i<n;i++)
		token[i]=1;
		
	int avail_call[11];	
	
	for(i=0;i<11;i++)
		avail_call[i]=1;
		
	//make the first 3 calls
	//~ update_secrets(secrets,1,5,n,m);
	//~ update_secrets(secrets,2,0,n,m);
	//~ update_secrets(secrets,4,3,n,m);
	//~ token[5]=token[0]=token[3]=0;
	//~ avail_call[0]=avail_call[1]=avail_call[2]=0;
	
	//create the ANY reachable state
	graph ANY_secrets [MAXN*MAXM];
	diagonal(ANY_secrets, n, m);
	for(i=0; i<11;i++)
		update_secrets(ANY_secrets, call_seq[i].a, call_seq[i].b,n,m);
	
	call_seq[0].a = 5;
	call_seq[0].b = 1;
	
	call_seq[1].a = 2;
	call_seq[1].b = 0;
	
	call_seq[2].a = 3;
	call_seq[2].b = 4;
	
	call_seq[3].a = 3;
	call_seq[3].b = 2;
	
	call_seq[4].a = 5;
	call_seq[4].b = 4;
	
	call_seq[5].a = 9;
	call_seq[5].b = 0;
	
	call_seq[6].a = 6;
	call_seq[6].b = 1;

	call_seq[7].a = 8;
	call_seq[7].b = 7;
	
	call_seq[8].a = 6;
	call_seq[8].b = 7;

	call_seq[9].a = 8;
	call_seq[9].b = 9;
	
	call_seq[10].a = 3;
	call_seq[10].b = 0;
	
		
	iter_calls(ANY_secrets, secrets, token, avail_call, call_seq, n, m);
}

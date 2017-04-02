#include "main.h"
#include "graph.h"
#include "state.h"
#include "compar.h"

/* counts the directed edges (including loops) in g */
int edges_of (graph g[MAXN*MAXM], int n) 
{
	size_t i;
	
	int edges = 0;
		
	for (i=0; i < MAXM*(size_t) n; i++)
		edges = edges + (POPCOUNT(g[i]));
	
	return edges;
}

/* adds only self loops and NO OTHER edge in g */
void init_secrets_graph (graph g[MAXN*MAXM], int n) 
{			
	size_t i;
	
	EMPTYGRAPH(g, MAXM, MAXN);
	
	for (i=0; i < MAXM*(size_t) n; i++)
		ADDONEARC(g,i,i, MAXM);
}

/* copies graph "from" to graph "to" */
void copy_graph (graph to[MAXN*MAXM], graph from[MAXN*MAXM], int n) 
{
	size_t i;
	
	for (i=0; i < MAXM*(size_t) n;i++)
		to[i] = from[i];
}

/* g2 will be equal to the canonical labeling of g1
 * n : size of g1 and g2 */
void find_can_label 
	(graph from[MAXN*MAXM], graph to[MAXN*MAXM], int n)
{
	int lab[MAXN], ptn[MAXN], orbits[MAXN];
	
	/* this is a function for vertex invariants. If it is set to NULL
	 * we loose some optimizations. I am not sure if there are any
	 * other side-effects */
	void* adjacencies = NULL;
	
	DEFAULTOPTIONS_DIGRAPH(options);
	statsblk stats;
			
	/* select option for canonical labelling */
    options.getcanon = TRUE;
    
    /* create the cannonicaly labeled graph */        		
	densenauty(from,lab,ptn,orbits,&options,&stats, MAXM, n, to);
}

/* makes the call from i to j in g */
void make_call(graph g[MAXN*MAXM], int i, int j) 
{	
	size_t k;
	
	for (k=0; k < MAXM;k++) {
		g[i + k] = g[i + k] | g[j + k];
		g[j + k] = g[i + k];
	}
}

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

/* returns the number of possible BIDIRECTIONAL calls between i and j
 * in g */
int poss_calls (graph g[MAXN * MAXM], int i, int j)
{
	int possCalls = 0;
	
	if ( !ISELEMENT(GRAPHROW(g,i,MAXM), j) )
		possCalls++;
	
	if ( !ISELEMENT(GRAPHROW(g,j,MAXM), i) )
		possCalls++;
	
	return possCalls;
}

int comp_perm_graphs(graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n)
{
	size_t k, l;
	
	int paired[MAXN*MAXM];		
	
	for (k = 0; k < MAXM*(size_t)n; ++k)
		paired[k]=0;
		
	for (k = 0; k < MAXM*(size_t)n; ++k) {
		for (l = 0; l < MAXM*(size_t)n; ++l)
			if ( (g1[k] == g2[l]) && !paired[l] ) {
				paired[k] = paired [l] = 1;
				break;				
			};
		
		if (!paired[k])
			return comp_graphs(g1, g2, n);
	}
			
	return EQUAL;
}

/* will be used for tests */
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
	
	printf ("equal = %d \n", comp_perm_graphs(g1, g2, n));
		
	printf ("edges of g1 = %d \n", edges_of(g1,n));
	
	printf ("edges of g2 = %d \n", edges_of(g2,n));
	
	printf ("poss Calls = %d \n", poss_calls(g1,i,j) );
}

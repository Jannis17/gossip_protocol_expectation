#include "main.h"
#include "graph.h"

/* counts the directed edges (including loops) in g */
int edgesOf (graph g[MAXN*MAXM], int n) 
{
	size_t i;
	
	int edges = 0;
		
	for (i=0; i < MAXM*(size_t) n; i++)
		edges = edges + (POPCOUNT(g[i]));
	
	return edges;
}

/* adds only self loops and NO OTHER edge in g */
void addOnlySelfLoops (graph g[MAXN*MAXM], int n) 
{			
	size_t i;
	
	EMPTYGRAPH(g, MAXM, MAXN);
	
	for (i=0; i < MAXM*(size_t) n; i++)
		ADDONEARC(g,i,i, MAXM);
}

/* copies graph "from" to graph "to" */
void copyGraph (graph to[MAXN*MAXM], graph from[MAXN*MAXM], int n) 
{
	size_t i;
	
	for (i=0; i < MAXM*(size_t) n;i++)
		to[i] = from[i];
}

/* compares g1 and g2 lexicographically
 * return value: 
 * -1 iff g1 < g2
 * 0 iff g1 < g2 
 * 1 iff g1 > g2 */
int compGraphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n) {
	size_t k;
	
	for (k = 0; k < MAXM*(size_t)n; ++k) {
		 if (g1[k] < g2[k])
			return -1;
		 if (g1[k] > g2[k])
			return 1;
	}
	
	return 0;
}

/* g2 will be equal to the canonical labeling of g1 */
void findCanonicalLabeling 
	(graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n)
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
	densenauty(g1,lab,ptn,orbits,&options,&stats, MAXM, n, g2);
}

/* makes the call from i to j in g */
void makeCall(graph g[MAXN*MAXM], int i, int j) 
{	
	size_t k;
	
	for (k=0; k < MAXM;k++) {
		g[i + k] = g[i + k] | g[j + k];
		g[j + k] = g[i + k];
	}
}

/* prints the contexts of g in hex form */
void printGraph(graph g[MAXN*MAXM], int n)
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
int possibleCalls (graph g[MAXN * MAXM], int i, int j)
{
	int possCalls = 0;
	
	if ( !ISELEMENT(GRAPHROW(g,i,MAXM), j) )
		possCalls++;
	
	if ( !ISELEMENT(GRAPHROW(g,j,MAXM), i) )
		possCalls++;
	
	return possCalls;
}


/* will be used for tests */
void graphTest(int n)
{
	graph g1[MAXN*MAXM];
	graph g2[MAXN*MAXM];
	graph g3[MAXN*MAXM];
	graph g4[MAXN*MAXM];
	
	int i = 2 , j = 0;
		
	addOnlySelfLoops(g1, n);
	addOnlySelfLoops(g2, n);
		
	makeCall(g1, 0, 1);
	makeCall(g1, 2, 3);
	//~ makeCall(g1, 0, 2);
	//~ makeCall(g1, 1, 3);	
	printGraph(g1, n);
	
	findCanonicalLabeling(g1, g3, n);
	
	printGraph(g3, n);
	
	makeCall(g2, 2, 0);
	makeCall(g2, 1, 3);
	makeCall(g2, 2, 1);
	makeCall(g2, 0, 3);
		
	printGraph(g2, n);
	
	findCanonicalLabeling(g2, g4, n);
	
	printGraph(g4, n);
	
	printf ("equal = %d \n", compGraphs(g4, g3, n));
		
	printf ("edges of g1 = %d \n", edgesOf(g1,n));
	
	printf ("edges of g2 = %d \n", edgesOf(g2,n));
	
	printf ("poss Calls = %d \n", possibleCalls(g1,i,j) );
}

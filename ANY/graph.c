#include "mainHeader.h"
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

/* returns 1 iff g1 and g2 are isomorphic */
int areIsomorphic (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n)
{
	/* this two graphs will store the cannonicaly labeled graphs */
	graph cg1[MAXN*MAXM];
	graph cg2[MAXN*MAXM];
	
	int lab1[MAXN], lab2[MAXN], ptn[MAXN], orbits[MAXN];
	
	/* this is a function for vertex invariants. If it is set to NULL
	 * we loose some optimizations. I am not sure if there other
	 * side-effects */
	void* adjacencies = NULL;
	
	DEFAULTOPTIONS_DIGRAPH(options);
	statsblk stats;
		
	size_t k;
		
	/* select option for canonical labelling */
    options.getcanon = TRUE;
    
    /* create the cannonicaly labeled graphs */        		
	densenauty(g1,lab1,ptn,orbits,&options,&stats, MAXM,n,cg1);
    densenauty(g2,lab2,ptn,orbits,&options,&stats, MAXM,n,cg2);
	
	/* g1 is iso to g2 iff cg1 is exactly the same as cg2 */
	for (k = 0; k < MAXM*(size_t)n; ++k)
		 if (cg1[k] != cg2[k])
			return 0;
			
	return 1;
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
		printf("%1lx", g[i]);
	}
	
	printf("\n");
}

int availCalls(graph g[MAXN * MAXM], int n)
{
	return n * n - edgesOf(g, n);
}

/* returns the number of possible BIDIRECTIONAL calls between i and j
 * in g. The size of g is n */
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
	
	int i = 0 , j = 1;
		
	addOnlySelfLoops(g1, n);
	addOnlySelfLoops(g2, n);
		
	makeCall(g1, 0, 1);
	makeCall(g1, 2, 3);
	makeCall(g1, 0, 2);
	makeCall(g1, 1, 3);	
	
	printGraph(g1, n);
	
	makeCall(g2, 2, 0);
	makeCall(g2, 2, 1);
	makeCall(g2, 2, 3);
	//~ makeCall(g2, 0, 1);
	//~ makeCall(g2, 0, 2);
		
	printGraph(g2, n);
	
	printf ("iso = %d \n", areIsomorphic(g1, g2, n));
	
	printf ("edges of g1 = %d \n", edgesOf(g1,n));
	
	printf ("poss Calls = %d \n", possibleCalls(g1,i,j) );
}

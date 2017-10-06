#include "main.h"
#include "graph.h"
#include "state.h"
#include "compar.h"
#include "memory.h"

/* counts the directed edges (including self loops) in g */
int edges_of (graph g[MAXN*MAXM], int n) 
{
	size_t i;
	
	int edges = 0;
		
	for (i=0; i < MAXM*(size_t) n; i++)
		edges = edges + (POPCOUNT(g[i]));
	
	return edges;
}

/* adds only self loops and NO OTHER edge in g */
void diagonal (graph g[MAXN*MAXM], int n) 
{			
	size_t i;
	
	EMPTYGRAPH(g, MAXM, MAXN);
	
	for (i=0; i < MAXM*(size_t) n; i++)
		ADDONEARC(g,i,i, MAXM);
}

/* sets the main diagonal equal to 0 and the rest equal to 1*/
void init_avail_calls_graph (graph g[MAXN*MAXM], int n) 
{			
	size_t i, j;
	
	EMPTYGRAPH(g, MAXM, MAXN);
		
	for (i=0; i < MAXM*(size_t) n; i++)
		for (j=0; j < MAXM*(size_t) n; j++)
			if (i!=j)
				ADDONEARC(g,i,j, MAXM);
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
void find_can_label (graph from[MAXN*MAXM], graph to[MAXN*MAXM], int n)
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

int can_call(graph g[MAXN * MAXM], int i, int j) 
{	
	return !ISELEMENT(GRAPHROW(g,i,MAXM), j);
}

/* returns the number of possible bidirectional calls between
 * i and j according to prot */
int no_poss_calls(pstate_t * pstate, int i, int j, int prot)
{
	int poss_calls = 0;
	
	switch (prot) {
		case (ANY):
			poss_calls = 2;
			break;
		case (LNS):
	       	if ( !ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,i,MAXM), j) )
				poss_calls++;
			if ( !ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,j,MAXM), i) )
				poss_calls++;
			break;
		case (CO):
			break;
		case (TOK):
			break;
		case (SPI):
			break;
		default:
			INTERNAL_ERROR("Unknown protocol name!");
		}
	
	return poss_calls;	
}

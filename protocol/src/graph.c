#include <math.h>
#include "main.h"
#include "graph.h"
#include "state.h"
#include "compar.h"
#include "memory.h"

/* prints the contexts of g in hex form */
void print_graph(graph g[MAXN*MAXM], int n, int m)
{
	size_t i;
	
	for (i=0; i < m*(size_t) n ; i++){
		printf(SETWORD_FORMAT, g[i]);
		printf("\n");
	}
}

void print_calls_graph(int calls[MAXN][MAXN], int n)
{
	int i,j;
	
	for (i=0; i < n ; i++) {
		for (j=0; j < n ; j++)
			printf("%d ", calls[i][j]);
		printf("\n");	
	}	
}


/* counts the directed edges (including self loops) in g */
int edges_of(graph g[MAXN*MAXM], int n, int m) 
{
	size_t i;
	
	int edges = 0;
			
	for (i=0; i < m*(size_t) n; i++)
		edges = edges + (POPCOUNT(g[i]));
	
	return edges;
}

/* adds only self loops and NO OTHER edge in g */
void diagonal(graph g[MAXN*MAXM], int n, int m) 
{			
	size_t i;
	
	EMPTYGRAPH(g,m,n);
	
	for(i=0; i < m*(size_t) n; i++)
		ADDONEARC(g,i,i, m);
}

void init_calls_graph(int calls[MAXN][MAXN], int n)
{
	int i,j;
	
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			calls[i][j]=0;
}

/* sets the main diagonal equal to 0 and the rest equal to 1*/
void init_avail_calls_graph(graph g[MAXN*MAXM], int n, int m) 
{			
	size_t i, j;
	
	EMPTYGRAPH(g, m, n);
		
	for (i=0; i < m*(size_t) n; i++)
		for (j=0; j < m*(size_t) n; j++)
			if (i!=j)
				ADDONEARC(g,i,j, m);
}

/* copies graph "from" to graph "to" */
void copy_graph(graph to[MAXN*MAXM], graph from[MAXN*MAXM], int n, int m) 
{
	size_t i;
	
	for (i=0; i < m*(size_t) n;i++)
		to[i] = from[i];
}

void copy_calls_graph(int to[MAXN][MAXN],int from[MAXN][MAXN],int n)
{
	size_t i, j;
	
	for (i=0; i < (size_t) n; i++)
		for (j=0; j < (size_t) n; j++)
			to[i][j] = from[i][j];
}

/* g2 will be equal to the canonical labeling of g1
 * n : size of g1 and g2 */
void find_can_label(graph from[MAXN*MAXM], graph to[MAXN*MAXM], int n)
{
	int lab[MAXN], ptn[MAXN], orbits[MAXN];
	
	int m = SETWORDSNEEDED(n);
		
	/* this is a function for vertex invariants. If it is set to NULL
	 * we loose some optimizations. I am not sure if there are any
	 * other side-effects */
	void* adjacencies = NULL;
	
	DEFAULTOPTIONS_DIGRAPH(options);
	statsblk stats;
			
	/* select option for canonical labelling */
    options.getcanon = TRUE;
    
    /* create the cannonicaly labeled graph */        		
	densenauty(from,lab,ptn,orbits,&options,&stats, m, n, to);
}


/* finds the canonical labeling of a calls graph*/
void can_label_calls(int init_calls[MAXN][MAXN],
graph can_calls[MAXN*MAXM], int n)
{
	int lab[MAXN], ptn[MAXN], orbits[MAXN], c;
	
	size_t i,j,k;
	
	int max_layers = ceil(log2( ( (double)n * (n-1) )/2 +1 ));
		
	int n_layered = max_layers*n;
	
	/* this is the layered graph that originates from the original
	 * graph */
	graph init_calls_layered[MAXN*MAXM];
	
	/* this is a function for vertex invariants. If it is set to NULL
	 * we loose some optimizations. I am not sure if there are any
	 * other side-effects */
	void* adjacencies = NULL;
			
	DEFAULTOPTIONS_DIGRAPH(options);
	statsblk stats;
	
	/* the following means that we will deal with coloured graphs */
	options.defaultptn = FALSE;		
	/* select option for canonical labelling */
    options.getcanon = TRUE;
        
    for(i=0;i<MAXN;i++) {
		lab[i]=i;
		ptn[i]= (i % n == n-1)? 0: 1;		
	}
        
	int m_layered = SETWORDSNEEDED(n_layered);
	
	EMPTYGRAPH(init_calls_layered, m_layered, n_layered);
    
    for(i=0; i < max_layers-1; i++) 
		for(j=0; j < n; j++) {
			ADDONEARC(init_calls_layered,j,j+n, m_layered);
			ADDONEARC(init_calls_layered,j+n,j, m_layered);
		}
    
    for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if ((c = init_calls[i][j]) > 0) {
				for (k = 0; k < max_layers;k++) {
					if (c % 2 == 1)
						ADDONEARC(init_calls_layered,k*n+i,k*n+j,m_layered);
					c>>=1;
				}
			}
	
    /* create the cannonicaly labeled graph */        		
	densenauty(init_calls_layered,lab,ptn,orbits,&options,&stats,m_layered, 
		n_layered, can_calls);
}			


void update_secrets(graph g[MAXN*MAXM], int i, int j, int n) 
{	
	size_t k;
	
	int m = SETWORDSNEEDED(n);
	
	for (k=0; k < m;k++) {
		g[i + k] = g[i + k] | g[j + k];
		g[j + k] = g[i + k];
	}
}


int can_call(graph g[MAXN * MAXM], int i, int j, int n) 
{	
	int m = SETWORDSNEEDED(n);
	
	return !ISELEMENT(GRAPHROW(g,i,m), j);
}

/* returns the number of possible bidirectional calls between
 * i and j according to prot */
int no_poss_calls(pstate_t * pstate, int i, int j, int prot, int n)
{
	int poss_calls = 0;
	
	int m = SETWORDSNEEDED(n);
		
	switch (prot) {
		case (ANY):
			poss_calls = 2;
			break;
		case (LNS):
			poss_calls=
	       	!ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,i,m),j) + 
	       	!ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,j,m),i);
			break;
		case (CO):
			poss_calls= 2 * !pstate->fixed_name_calls[i][j];
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




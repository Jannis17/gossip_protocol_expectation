#include "main.h"
#include "graph.h"
#include "state.h"
#include "compar.h"
#include "memory.h"

/* prints the contexts of g in hex form */
void print_graph(graph g[MAXN*MAXM], int n)
{
	size_t i;
	
	for (i=0; i < MAXM*(size_t) n ; i++){
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
	
	for(i=0; i < MAXM*(size_t) n; i++)
		ADDONEARC(g,i,i, MAXM);
}

void init_calls_graph(int calls[MAXN][MAXN], int n)
{
	int i,j;
	
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			calls[i][j]=0;
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


/* finds the canonical labeling of a calls graph*/
void can_label_calls(int init_calls[MAXN][MAXN],
int can_calls[MAXN][MAXN], int n)
{
	int i, j, curr_call, can_caller, can_callee, next_unmatched=0;
	
	int match_of[MAXN];
	
	for(i=0;i<n;i++) {
		match_of[i]=-1;
		for(j=0;j<n;j++)
			can_calls[i][j]=0;
	}
	
	for(i=0;i<n;i++)
		for(j=i+1;j<n;j++)	
			if( ( curr_call=init_calls[i][j] )>0) {
				if(match_of[i]>=0)
					can_caller=match_of[i];
				else{
					can_caller=next_unmatched;
					match_of[i]=next_unmatched++;
				}
				
				if(match_of[j]>=0)
					can_callee=match_of[j];
				else{
					can_callee=next_unmatched;
					match_of[j]=next_unmatched++;
				}
				
				//~ printf("next_unmatched = %d, can_caller =%d , can_callee =%d \n",  next_unmatched,
					//~ can_caller, can_callee);
				
				can_calls[can_caller][can_callee]=	
					can_calls[can_callee][can_caller]=curr_call;
			}	
}			


void update_secrets(graph g[MAXN*MAXM], int i, int j) 
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
			poss_calls=
	       	!ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,i,MAXM),j) + 
	       	!ISELEMENT(GRAPHROW(pstate->fixed_name_secrets,j,MAXM),i);
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




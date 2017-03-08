#include "mainHeader.h"
#include "graph.h"
#include "memory.h"
#include "prob.h"
#include "state.h"
#include "queue.h"

/* state structure */
typedef struct state_tag {
	graph secrets[MAXN*MAXM];
	int edges;
	int id;
	int agents;
	struct queue_t children;
} LNSstate;

/* hash table that contains all the possible states
 * each dimension corresponds to the total number of secrets, i.e.
 * the total number of edges in the secrets graph */
struct queue_t* lnsHash[MAXN*MAXN];

/* compares the secrets of s1 and s2 lexicographically
 * return value: 
 * -1 iff s1 -> secrets < s2 -> secrets
 * 0 iff s1 -> secrets = s2 -> secrets
 * 1 iff s1 -> secrets > s2 -> secrets */
int compStates(LNSstate s1, LNSstate s2)
{
	size_t k;
	
	for (k = 0; k < MAXM*(size_t)n; ++k) {
		 if ( (s1 -> secrets)[k] < (s2 -> secrets)[k] )
			return -1;
		 if ( (s1 -> secrets)[k] > (s2 -> secrets)[k] )
			return 1;
	}
	
	return 0;
}

/* gets a graph g and returns an LNSstate with the graph in
 * canonical form */
LNSstate* newLNSstate(graph g[MAXN*MAXM], int agents)
{
	LNSstate* s = (LNSstate*) malloc(sizeof(LNSstate));
	exitIfNULL(s);
		
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
	densenauty(g,lab,ptn,orbits,&options,&stats, MAXM, agents, 
				s->secrets);
    	
	s->id = 0;
	s->agents=agents;
	
	s->children=new_queue(agents * (agents-1) ,NULL, NULL);
		
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
struct queue_node_t* addToHash (graph secrets[MAXN*MAXM], int agents) 
{
	LNSstate* child = newLNSstate(secrets, agents);
	
	struct queue_t* childsList = hashedStates[edgesOf(secrets)];
	
	struct queue_node_t* childPtr;
	
	if ( !enqueue_unique_to_sorted_queue(childsList, &childPtr, child) )
	{
		DELETE_QUEUE(child->children);
		free(child);
	}	
		
	return childPtr;
}



void genChildren(int agents, state* parent) 
{
	int i, j, possCalls;
		
	graph newSecrets[MAXN*MAXM];
	
	struct queue_node_t* childPtr;
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    possCalls = possibleCalls(parent->secrets, i, j);
	    if (possCalls)
		{
		  copyGraph(newSecrets, parent->secrets, agents);
		  
		  makeCall(newSecrets, i ,j);
		  		  
		  childPtr = addToHash(newSecrets, agents);
		  
		  addChildToParent(parent, childPtr, possCalls);		  
	    }
	  }	
}

void initHash(int agents)
{
	int i;
	
	for(i=agents-1; i < agents * agents; i++)
		hashedStates[i] = new_queue(MAXSTATES, compStates, NULL);
}


void destroyHash(int agents)
{
	int i;
	
	for(i=agents-1; i < agents * agents; i++)
		DELETE_QUEUE(hashedStates[i]);
}

void initTrMat()
{
	
}

float findExpectation (int agents)
{
	int i;
	
	graph initSecrets[MAXN*MAXM];
				
	initHash();
	
	initTrMat();
					
	addOnlySelfLoops(initSecrets, agents);
	
	makeCall(initSecrets, 0, 1);					
			
	state* statePtr;
		
		
	return 0.0;	
}

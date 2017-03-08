#include "mainHeader.h"
#include "graph.h"
#include "memory.h"
#include "prob.h"
#include "state.h"
#include "queue.h"

typedef struct queue_t LNSstateList;

/* state structure */
typedef struct state_tag {
	graph secrets[MAXN*MAXM];
	int edges;
	int id;
	int agents;
	LNSstateList children;
} LNSstate;

/* hash table that contains all the possible states
 * each dimension corresponds to the total number of secrets, i.e.
 * the total number of edges in the secrets graph */
LNSstateList lnsHash[MAXN*MAXN];

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
	s->edges = edgesOf(s->secrets,agents);
	s->agents=agents;
	
	new_queue(agents * (agents-1) ,NULL, NULL);
		
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
LNSstate* addToHash (graph secrets[MAXN*MAXM], int agents) 
{
	LNSstate* child = newLNSstate (secrets, agents);
	
	LNSstateList childsList = hashedStates[child->edges];
	
	
				
	totalStates[agents] = totalStates[agents]+1;
	
	return ;
}



void genChildren(int agents, state* parent) 
{
	int i, j, possCalls;
		
	graph newSecrets[MAXN*MAXM];
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    possCalls = possibleCalls(parent->secrets, i, j);
	    if (possCalls)
		{
		  copyGraph(newSecrets, parent->secrets, agents);
		  
		  makeCall(newSecrets, i ,j);
		  		  
		  child = addToHash(newSecrets, agents);
		  
		  addChildToParent(parent, child, possCalls);		  
	    }
	  }	
}


/* frees all the states in the sList */
//~ void freeStates (stateList sList) 
//~ {
	//~ state* currState = sList->first;
	//~ state* prevState;
	
	//~ while (currState) 
	//~ {
		//~ prevState=currState;
		//~ currState=currState->next;
		//~ free(prevState);
	//~ }
	
	//~ sList->first = sList->last = NULL;
//~ }

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
		delete_queue(hashedStates[i]);
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
				
	totalStates[agents] = 0;
				
	addOnlySelfLoops(initSecrets, agents);
	
	makeCall(initSecrets, 0, 1);					
		
		
	state* statePtr;
		
		
		
	
	return calcExpectation(trMat, totalStates[agents]-1) + 1.0;	
}

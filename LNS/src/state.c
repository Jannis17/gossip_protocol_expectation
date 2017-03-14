#include "main.h"
#include "graph.h"
#include "memory.h"
#include "gauss.h"
#include "state.h"
#include "queue.h"

int agents;

/* state structure */
typedef struct LNSstate_tag {
	graph secrets[MAXN*MAXM];
	int id;
	int no_of_children;
	int callsToChild[MAXN*(MAXN-1)];
	struct LNSstate_tag* children[MAXN*(MAXN-1)];
} LNSstate;

/* hash[i] contains all the states with i+1 secrets */
struct queue_t* hash[MAXN*MAXN];


/* compares the secrets of s1 and s2 lexicographically
 * return value: 
 * -1 iff s1 -> secrets < s2 -> secrets
 * 0 iff s1 -> secrets = s2 -> secrets
 * 1 iff s1 -> secrets > s2 -> secrets */
int compStates(const void* item1, const void* item2)
{
	LNSstate* state1, *state2;
	
	state1 = (LNSstate *) item1;
	state2 = (LNSstate *) item2;
	
	return compGraphs(state1->secrets, state2->secrets, agents);
}

/* gets a graph g and returns an LNSstate with the graph in
 * canonical form */
LNSstate* newLNSstate(graph g[MAXN*MAXM])
{
	LNSstate* s;
	
	MALLOC_SAFE(s, sizeof(LNSstate));
		
	findCanonicalLabeling(g, s->secrets, agents);    	
	s->id = 0;
			
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
LNSstate* addToHash(graph secrets[MAXN*MAXM]) 
{
	LNSstate* state = newLNSstate(secrets);
	
	struct queue_t* statesList = hash[edgesOf(secrets, agents)];
	
	struct queue_node_t* statePtr;
	
	if (!enqueue_unique_to_sorted_queue(statesList, &statePtr, state))
		FREE_SAFE(state);
		
	return statePtr->data;
}

void addChildToParent(LNSstate* parent, LNSstate* child, int calls)
{
	int i;
	
	for (i=0; i < (parent -> no_of_children) ; i++)
		if ((parent -> children)[i] == child) {
			(parent -> callsToChild)[i] = 
				(parent -> callsToChild)[i] + calls;
			return;
		}
	
	(parent -> children)[i] = child;
	
	(parent -> callsToChild)[i] = calls;
	
	(parent -> no_of_children)++;
}

void genChildren(LNSstate* parent) 
{
	int i, j, callsToChild;
		
	graph childsSecrets[MAXN*MAXM];
	
	LNSstate* childPtr;
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    callsToChild = possibleCalls(parent->secrets, i, j);
	    if (callsToChild)
		{
		  copyGraph(childsSecrets, parent->secrets, agents);
		  makeCall(childsSecrets, i ,j);  
		  childPtr = addToHash(childsSecrets);		  
		  addChildToParent(parent, childPtr, callsToChild);		  
	    }
	  }	
}

void initHash()
{
	int i;
	
	graph initial[MAXN*MAXM];
	
	/* create the queues */		
	FOR_ALL_EDGES(i, agents)
		hash[i] = new_queue(MAXSTATES, compStates);
	
	/* we add the first state into the hash */				
	addOnlySelfLoops(initial, agents);
	addToHash(initial);
}

void build_the_markov_chain()
{
	int i;
	
	struct queue_node_t * p;
	
	FOR_ALL_EDGES(i, agents)
		QUEUE_FOREACH(p, hash[i])
			genChildren(p->data);
}


float findExpectation (int theAgents)
{
	agents = theAgents;
	
	initHash();
	
	build_the_markov_chain();
	
	int i;
	
	/* count the states */	
	FOR_ALL_EDGES(i, agents)
		no_states[agents] = no_states[agents] + QUEUE_COUNT(hash[i]);
	
	/* label the states */	
	
	/* create the upper triangular transition matrix */
	
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents)
		DELETE_QUEUE(hash[i]);	
	
	/* compute the gaussian elimination */	
	//return gaussElimination();
	
	return 0.0;	
}

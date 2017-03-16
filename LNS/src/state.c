#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "gauss.h"
#include "state.h"
#include "queue.h"
#include "compar.h"

int agents;

/* hash[i] contains all the states with i+1 secrets */
struct queue_t* hash[MAXN*MAXN];

/* returns an LNSstate_t with g in canonical form */
LNSstate_t* newLNSstate(graph g[MAXN*MAXM])
{
	LNSstate_t* s;
	
	MALLOC_SAFE(s, sizeof(LNSstate_t));
		
	findCanonicalLabeling(g, s->secrets, agents); 
		
	s->children = new_queue(MAXN*(MAXN-1), compChildren);
	s->id = 0;
	s->agents=agents;
					
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
struct queue_node_t* addToHash(graph secrets[MAXN*MAXM]) 
{
	LNSstate_t* state = newLNSstate(secrets);
	
	struct queue_t* statesList = hash[edgesOf(secrets, agents)-1];
	
	struct queue_node_t* statePtr;
	
	if ( enqueue_unique_to_sorted_queue(statesList, &statePtr, state) ==
		 DUPLICATE_ITEM )
		 FREE_SAFE(state);
		
	return statePtr;
}

void addChildToParent
	(LNSstate_t* parent, struct queue_node_t* child, int calls)
{
	child_t* newChild;
	
	MALLOC_SAFE(newChild, sizeof(child_t));
	
	newChild->childPtr = child;
	
	struct queue_node_t* p;
	
	child_t * oldChild;
				
	if( enqueue_unique_to_sorted_queue(parent->children, &p, newChild)
		== DUPLICATE_ITEM) {
		FREE_SAFE(newChild);
		oldChild = (child_t *) p->data;
		(oldChild->callsToChild)+=calls;
	} else
		newChild->callsToChild = calls;
}

void genChildren(LNSstate_t* parent) 
{
	int i, j, callsToChild;
		
	graph childsSecrets[MAXN*MAXM];
	
	struct queue_node_t* childPtr;
					
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
	
	printf("Agents = %d\n", agents);
	
	clock_t start, end;
	
	FOR_ALL_EDGES(i, agents) {
		start = clock();
		QUEUE_FOREACH(p, hash[i])
			genChildren(p->data);
		end = clock();
		printf("%d secrets in %f seconds\n", i+1 , 
				( (float) end - start )/CLOCKS_PER_SEC);	
	}
}


float findExpectation (int theAgents)
{
	agents = theAgents;
	
	initHash();
	
	build_the_markov_chain();
	
	int i;
	
	no_states[agents]=0;
	
	/* count the states */	
	FOR_ALL_EDGES(i, agents)
		no_states[agents] = no_states[agents] + QUEUE_COUNT(hash[i]);
	
	struct queue_node_t * p;
	
	LNSstate_t *s;
	
	/* label the states */	
	//~ int label = 0;
	//~ FOR_ALL_EDGES(i, agents)
		//~ QUEUE_FOREACH(p, hash[i]) {
			//~ s = (LNSstate_t *) (p->data);
			//~ s->id = label++;
		//~ }

	
	/* create the upper triangular transition matrix */
	//~ struct queue_t** transMatrix;
	
	//~ MALLOC_SAFE(transMatrix, no_states[agents] * sizeof(struct queue_t *));
	
	//~ for(i=0;i<no_states[agents];i++)
		//~ transMatrix[i]=new_queue(MAXN*(MAXN-1), );
		
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents) {
		QUEUE_FOREACH(p, hash[i]) {
			s = (LNSstate_t *) (p->data);
			printGraph(s->secrets, agents);
			DELETE_QUEUE(s->children);
		}
		DELETE_QUEUE(hash[i]);
	}	
	
	/* compute the gaussian elimination */	
	//return gaussElimination();
	
	return 0.0;	
}

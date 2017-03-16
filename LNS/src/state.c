#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "gauss.h"
#include "state.h"
#include "queue.h"
#include "hash.h"

/* compares the ids of the args lexicographically */
int compStateIds(const void* item1, const void* item2)
{
	LNSstate_t* state1, *state2;
	
	state1 = (LNSstate_t *) item1;
	state2 = (LNSstate_t *) item2;
	
	if (state1->id < state2->id)
		return LESS;
	
	if (state1->id > state2->id)
		return GREATER;
	
	return EQUAL;
}

/* compares the secrets of the args lexicographically */
int compChildren(const void* item1, const void* item2)
{
	child_t* child1, *child2;
	
	child1 = (child_t *) item1;
	child2 = (child_t *) item2;
	
	LNSstate_t* state1, * state2;
	
	state1 = (LNSstate_t *) child1->childPtr->data;
	state2 = (LNSstate_t *) child2->childPtr->data;
	
	return compGraphs(state1->secrets, state2->secrets, state1->agents);
}

/* compares the secrets of the args lexicographically */
int compStates(const void* item1, const void* item2)
{
	LNSstate_t* state1, *state2;
			
	state1 = (LNSstate_t *) item1;
	state2 = (LNSstate_t *) item2;
	
	//~ if (state1->agents != state2->agents) {
		//~ printf("Internal error: Trying to compare graphs
				//~ of different size\n");
		//~ exit(0);
	//~ }
	
	//~ if (state1->agents < 0) {
		//~ printf("Negative agents (%dd)!\n", state1->agents);
		//~ printGraph(state1->secrets,4);
		//~ exit(1);
	//~ }
		
	//~ printf("Compared graphs with %d agents\n", state2->agents);
	
	return compGraphs(state1->secrets, state2->secrets, state1->agents);
}


/* returns an LNSstate_t with g in canonical form */
LNSstate_t* newLNSstate(graph g[MAXN*MAXM], int agents)
{
	LNSstate_t* s;
	
	if (agents < 0) {
		printf("Negative agents (%dd)!\n", agents);
		exit(1);
	}
	
	MALLOC_SAFE(s, sizeof(LNSstate_t));
		
	findCanonicalLabeling(g, s->secrets, agents); 
		
	s->children = new_queue(MAXN*(MAXN-1), compChildren);
	s->id = 0;
	s->agents=agents;
						
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
void addToHash(graph secrets[MAXN*MAXM], int agents, 
	struct queue_node_t** found, struct queue_t* hash[MAXN*MAXN]) 
{
	if (agents < 0) {
		printf("Negative agents (%dd)!\n", agents);
		exit(1);
	}

	LNSstate_t* state = newLNSstate(secrets, agents);
	
	struct queue_t* statesList = hash[edgesOf(secrets, agents)-1];
		
	if ( enqueue_unique_to_sorted_queue(statesList, found, state) 
		 == DUPLICATE_ITEM )
		FREE_SAFE(state);
		
	//~ LNSstate_t* s;
	
	//~ if (found) {
		//~ s = (LNSstate_t *) ((*found) -> data); 
	
		//~ printf("hans\n");
		
		//~ if (s -> agents < 0) {
			//~ printf("Negative agents (%d) inserted to list !\n", agents);
			//~ exit(1);
		//~ }
	//~ }

}

void initHash(struct queue_t* hash[MAXN*MAXN], int agents)
{
	int i;
	
	graph initial[MAXN*MAXM];
	
	/* create the queues */		
	FOR_ALL_EDGES(i, agents)
		hash[i] = new_queue(MAXSTATES, compStates);
	
	/* we add the first state into the hash */				
	addOnlySelfLoops(initial, agents);
		
	addToHash(initial, agents, NULL, hash);
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

void genChildren(LNSstate_t* parent, int agents, struct queue_t* hash[MAXN*MAXN]) 
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
		  addToHash(childsSecrets, agents, &childPtr, hash);		  
		  addChildToParent(parent, childPtr, callsToChild);		  
	    }
	  }	
}

void build_the_markov_chain(struct queue_t* hash[MAXN*MAXN], int agents)
{
	int i;
	
	struct queue_node_t * p;
	
	printf("Agents = %d\n", agents);
	
	clock_t start, end;
	
	FOR_ALL_EDGES(i, agents) {
		start = clock();
		QUEUE_FOREACH(p, hash[i])
			genChildren(p->data, agents, hash);
		end = clock();
		printf("%d secrets in %f seconds\n", i+1 , 
				( (float) end - start )/CLOCKS_PER_SEC);	
	}
}


float findExpectation (int agents, int* no_states)
{		
	struct queue_t* hash[MAXN*MAXN];
	
	initHash(hash, agents);
	
	build_the_markov_chain(hash, agents);
	
	int i;
	
	*no_states = 0;
	
	/* count the states */	
	FOR_ALL_EDGES(i, agents)
		*no_states += QUEUE_COUNT(hash[i]);
		 	
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
			
			//~ printf("Agents = %d!\n", s->agents);
			//~ printGraph(s->secrets, agents);
			DELETE_QUEUE(s->children);
		}
		DELETE_QUEUE(hash[i]);
	}	
	
	/* compute the gaussian elimination */	
	//return gaussElimination();
	
	return 0.0;	
}


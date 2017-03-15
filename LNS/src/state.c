#include "main.h"
#include "graph.h"
#include "memory.h"
#include "gauss.h"
#include "state.h"
#include "queue.h"
#include <time.h>

int agents;

/* state structure */
typedef struct LNSstate_tag {
	graph secrets[MAXN*MAXM];
	int id;
	int children_no;
	int callsToChild[MAXN*(MAXN-1)];
	struct LNSstate_tag* children[MAXN*(MAXN-1)];
} LNSstate;

/* hash[i] contains all the states with i+1 secrets */
struct queue_t* hash[MAXN*MAXN];


/* compares the secrets of s1 and s2 lexicographically
 * return value: 
 * LESS iff s1 -> secrets < s2 -> secrets
 * EQUAL iff s1 -> secrets == s2 -> secrets
 * GREATER iff s1 -> secrets > s2 -> secrets */
int compStates(const void* item1, const void* item2)
{
	LNSstate* state1, *state2;
	
	state1 = (LNSstate *) item1;
	state2 = (LNSstate *) item2;
	
	return compGraphs(state1->secrets, state2->secrets, agents);
}

int compStateIds(const void* item1, const void* item2)
{
	LNSstate* state1, *state2;
	
	state1 = (LNSstate *) item1;
	state2 = (LNSstate *) item2;
	
	if (state1->id < state2->id)
		return LESS;
	
	if (state1->id > state2->id)
		return GREATER;
	
	return EQUAL;
}

/* returns an LNSstate with g in canonical form */
LNSstate* newLNSstate(graph g[MAXN*MAXM])
{
	LNSstate* s;
	
	MALLOC_SAFE(s, sizeof(LNSstate));
		
	findCanonicalLabeling(g, s->secrets, agents); 
	s->children_no = 0;   	
	s->id = 0;
	
	int i;
	
	for(i=0; i < MAXN*(MAXN-1); i++) {
		s->callsToChild[i] = 0;
		s->children[i]=NULL;
	}
					
	return s;		
}

/* adds state to the list sList 
 * returns a pointer to the state just added */
LNSstate* addToHash(graph secrets[MAXN*MAXM]) 
{
	LNSstate* state = newLNSstate(secrets);
	
	struct queue_t* statesList = hash[edgesOf(secrets, agents)-1];
	
	struct queue_node_t* statePtr;
	
	if ( enqueue_unique_to_sorted_queue(statesList, &statePtr, state) ==
		 DUPLICATE_ITEM )
		 FREE_SAFE(state);
		
	return statePtr->data;
}

void addChildToParent(LNSstate* parent, LNSstate* child, int calls)
{
	int i;
	
	for (i=0; i < (parent -> children_no) ; i++)
		if (parent -> children[i] == child) {
			parent -> callsToChild[i] = 
				parent -> callsToChild[i] + calls;
			return;
		}
	
	parent -> children[i] = child;
	
	parent -> callsToChild[i] = calls;
	
	(parent -> children_no)++;
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
	
	LNSstate *s;
	
	/* label the states */	
	int label = 0;
	FOR_ALL_EDGES(i, agents)
		QUEUE_FOREACH(p, hash[i]) {
			s = (LNSstate *) (p->data);
			s->id = label++;
		}

	
	/* create the upper triangular transition matrix */
	struct queue_t** transMatrix;
	
	MALLOC_SAFE(transMatrix, no_states[agents] * sizeof(struct queue_t *));
	
	//~ for(i=0;i<no_states[agents];i++)
		//~ transMatrix[i]=new_queue(MAXN*(MAXN-1), );
		
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents)
		DELETE_QUEUE(hash[i]);	
	
	/* compute the gaussian elimination */	
	//return gaussElimination();
	
	return 0.0;	
}

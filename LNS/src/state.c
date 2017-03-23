#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "gauss.h"
#include "state.h"
#include "queue.h"

typedef struct LNSstate_tag {
	graph secrets[MAXN*MAXM];
	int id;
	int agents;
	struct queue_t* children;
} LNSstate_t;

typedef struct child_tag {
	int callsToChild;
	LNSstate_t* childPtr;
} child_t;


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
	
	return compGraphs(child1->childPtr->secrets, 
			child2->childPtr->secrets, child1->childPtr->agents);
}

/* compares the secrets of the args lexicographically */
int compStates(const void* item1, const void* item2)
{
	LNSstate_t* state1, *state2;
			
	state1 = (LNSstate_t *) item1;
	state2 = (LNSstate_t *) item2;
	
	return compGraphs(state1->secrets, state2->secrets, state1->agents);
}

/* returns an LNSstate_t with g in canonical form */
LNSstate_t* newLNSstate(graph g[MAXN*MAXM], int agents)
{
	LNSstate_t* s;
	
	MALLOC_SAFE(s, sizeof(LNSstate_t));
		
	findCanonicalLabeling(g, s->secrets, agents); 
		
	s->children = new_queue(MAXN*(MAXN-1), compChildren);
	s->id = 0;
	s->agents=agents;
						
	return s;		
}

void initHash(struct queue_t* hash[MAXN*MAXN], int agents)
{
	int i;
	
	graph initG [MAXN*MAXM];
	
	/* create the queues */		
	FOR_ALL_EDGES(i, agents)
		hash[i] = new_queue(MAXSTATES, compStates);
	
	/* we add the first state into the hash */				
	addOnlySelfLoops(initG, agents);
	
	LNSstate_t* initState = newLNSstate(initG, agents);
	
	struct queue_t* initQueue = hash[edgesOf(initG, agents)-1];
		
	enqueue_unique_to_sorted_queue(initQueue, NULL, initState);
}

void addChildToParent
	(LNSstate_t* parent, LNSstate_t* child, int calls)
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

void genChildren
	(LNSstate_t* parent, int agents, struct queue_t* hash[MAXN*MAXN]) 
{
	int i, j, callsToChild;
		
	graph temp[MAXN*MAXM];
	
	child_t* childsStruct;
	
	child_t* foundChild;
	
	struct queue_node_t* childsQueueNode;
	
	struct queue_node_t* childsStructPtr;
	
	LNSstate_t* childsState;
	
	struct queue_t* childsList;
					
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    callsToChild = possibleCalls(parent->secrets, i, j);
	    if (callsToChild)
		{
		  copyGraph(temp, parent->secrets, agents);
		  makeCall(temp, i ,j); 
		  childsState = newLNSstate(temp, agents);
		  MALLOC_SAFE(childsStruct, sizeof(child_t));
		  childsStruct->callsToChild=callsToChild;
		  childsStruct->childPtr=childsState;
		  		  
		  if (search_in_sorted_queue( parent->children, &childsStructPtr, 
									   childsStruct ) )
		  {
			 FREE_SAFE(childsStruct); 
			 foundChild = (child_t *) childsStructPtr->data;
			 
			 (foundChild->callsToChild) += callsToChild;
		  }
		  else {
		  	childsList = hash[edgesOf(temp, agents)-1];
		
			if ( enqueue_unique_to_sorted_queue(childsList, &childsQueueNode, childsState) 
				== DUPLICATE_ITEM )
			 FREE_SAFE(childsState);
 		  
			addChildToParent(parent, childsQueueNode->data, callsToChild);
		  }
		  
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

float getProb(struct queue_t* hash[MAXN*MAXN], LNSstate_t ** transMatrix, 
	int from, int to) 
{
	LNSstate_t * s = transMatrix[from];
	
	struct queue_node_t *p;
	
	child_t* child;
	
	float prob;
	
	QUEUE_FOREACH(p, s->children)
		child = (child_t*) (p-> data);
		if (child->ChildPtr->id == to) {
			prob = 
		}
			
		
	return 0.0;	
}

float findExpectation (int agents, int* no_states)
{		
	struct queue_t* hash[MAXN*MAXN];
	
	initHash(hash, agents);
	
	build_the_markov_chain(hash, agents);
	
	int i, j;
	
	*no_states = 0;
	
	/* count the states */	
	FOR_ALL_EDGES(i, agents)
		*no_states += QUEUE_COUNT(hash[i]);
	
	LNSstate_t ** transMatrix;
	
	MALLOC_SAFE(transMatrix, (* no_states) * sizeof(LNSstate_t *));
		 	
	struct queue_node_t * p;
	
	LNSstate_t *s;
	
	/* label the states */	
	int label = 0;
	FOR_ALL_EDGES(i, agents)
		QUEUE_FOREACH(p, hash[i]) {
			s = (LNSstate_t *) (p->data);
			transMatrix[label]=s;
			s->id = label++;
		}

	float* expectVec;
		
	MALLOC_SAFE(expectVec, *no_states * sizeof(float));
	
	expectVec[*no_states-1]= 0;
					
    /* this loop is for backward substitution*/
    for(i= (* no_states)-2; i>=0; i--)
    {
        expectVec[i] = 0;
                        
        for(j=i+1; j< (* no_states); j++)
            expectVec[i] = 
            1 +	getProb(hash, transMatrix, i, j) * expectationVec[j];                        
    }

	float result = expectVec[0];
	
	FREE_SAFE(expectVec);
	
	FREE_SAFE(transMatrix);
		
	/* destroy the hash */		
	FOR_ALL_EDGES(i, agents) {
		QUEUE_FOREACH(p, hash[i]) {
			s = (LNSstate_t *) (p->data);
			DELETE_QUEUE(s->children);
		}
		DELETE_QUEUE(hash[i]);
	}	
		
	return result;	
}

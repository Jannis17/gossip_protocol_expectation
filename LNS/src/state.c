#include "mainHeader.h"
#include "graph.h"
#include "memory.h"
#include "prob.h"
#include "state.h"

/* state structure
 * next = pointer to the next state 
 * id = unique identifier */
typedef struct state_tag {
	graph secrets[MAXN*MAXM];
	int id;
	struct state_tag* next;
} state;

/* list of states structure
 * first = pointer to the first state
 * last = pointer to the last state */
typedef struct stateList_tag {
	state* first;
	state* last;
}* stateList;

/* hash table that contains all the possible states
	 * each dimension corresponds to the total number of secrets */
stateList hashedStates[MAXN*MAXN];

/* the transition probability matrix */
float trMat[MAXSTATES][MAXSTATES+1];

int absorbStateID = -1;

/* adds a new state to the list sList 
 * return value = the id of the state of the just added state */
int addNewStateInList 
	(graph secrets[MAXN*MAXM], stateList sList, int agents) 
{
	state* newState = (state *) malloc(sizeof(state));
	exitIfNULL(newState);
		
	copyGraph(newState->secrets, secrets, agents);
	newState->id = totalStates[agents];
	newState->next = NULL;
		
	if (sList->first == NULL)
		sList->first = sList->last = newState;
	else 
	{	
		sList->last->next = newState;			
		sList->last = newState;	
	}
	
	totalStates[agents] = totalStates[agents]+1;
	
	//~ if (totalStates[agents] > MAXSTATES) 
	//~ {
		//~ printf("Too many states ...\n");
		//~ printf("The current limit is: %d\n", MAXSTATES);
		//~ exit(1);
	//~ }
	
	return newState->id;
}

/* if the given state appears in the sList the function returns
 * the id of the state, otherwise -1 */
int findStateInList 
	(graph secr[MAXN*MAXM], int agents, stateList sList) 
{
	state* statePtr = sList->first;
	
	while (statePtr) 
	{
		if ( areIsomorphic (secr, statePtr->secrets, agents) )
			return statePtr-> id; /* state found */
		statePtr = statePtr->next;
	}
	
	return -1;/* state not found */
}

/* frees all the states in the sList */
void freeStates (stateList sList) 
{
	state* currState = sList->first;
	state* prevState;
	
	while (currState) 
	{
		prevState=currState;
		currState=currState->next;
		free(prevState);
	}
	
	sList->first = sList->last = NULL;
}

void genChildren(int agents, state* currState) 
{
	int i, j, newStateID, possCalls, avCalls, absorbState = 1;
	
	float transProb;
	
	graph newSecrets[MAXN*MAXM];
	
	stateList newSList;
				
	for (i=0; i<agents; i++)
	  for (j=i+1; j<agents; j++) 
	  {
	    possCalls = possibleCalls(currState->secrets, i, j);
	    if ( possCalls > 0 )
		{
		  absorbState = 0;	
		  copyGraph(newSecrets, currState->secrets, agents);
		  makeCall(newSecrets, i ,j);
		  
		  newSList = hashedStates[edgesOf(newSecrets, agents)];
		  
		  newStateID = findStateInList(newSecrets, agents, newSList);
		  
		  if (newStateID == -1)
		    newStateID = addNewStateInList(newSecrets, newSList, agents);
		  
		  avCalls = availCalls(currState->secrets, agents);
		  
		  transProb = ((float) possCalls) /  ( (float) avCalls);
		  trMat[currState->id][newStateID] =
			  trMat[currState->id][newStateID] + transProb;
	    }
	  };
	
	if (absorbState) {
		absorbStateID = currState->id;
		trMat[currState->id][currState->id] = 1.0;
	}
}

void initHash()
{
	int i;
	
	for(i=0; i < MAXN * MAXN; i++)
	{
		hashedStates[i] = (stateList) malloc(sizeof(stateList *));
		exitIfNULL(hashedStates[i]);
		hashedStates[i]->first=hashedStates[i]->last=NULL;
	}
}

void initTrMat()
{
	int i,j;
	
	for (i = 0; i < MAXSTATES; i ++)
		for (j = 0; j < MAXSTATES; j ++)
			trMat[i][j] = 0.0;
}

void removeAbsorbState(int agents)
{
	int i, j;
	
	/* remove the column that corresponds to the absorption state */
	for(j=absorbStateID; j<totalStates[agents]; j++)
		for(i=0; i<totalStates[agents]; i++)
			trMat[i][j]=trMat[i][j+1];
	
	/* remove the row that corresponds to the absorption state */
	for(i=absorbStateID; i<totalStates[agents]-1; i++)
		for(j=0; j<=totalStates[agents]; j++)
			trMat[i][j]=trMat[i+1][j];
}

float findExpectation (int agents)
{
	int i;
	
	graph initSecrets[MAXN*MAXM];
	
	stateList initSList;
			
	initHash();
	
	initTrMat();
				
	/* set the state counter to 0 */
	totalStates[agents] = 0;
				
	/* create the initial state */
	addOnlySelfLoops(initSecrets, agents);
	
	/* we choose an arbitrary first call
	 * this choice does not affect the expectated time */
	makeCall(initSecrets, 0, 1);					
		
	initSList = hashedStates[edgesOf(initSecrets, agents)];
	
	/* we add the initial state in the hash */
	addNewStateInList(initSecrets, initSList, agents);
	
	state* statePtr;
		
	for(i=0; i <= agents* agents; i++)
	{
		statePtr = hashedStates[i]->first;
		while (statePtr) 
		{
			genChildren(agents, statePtr);
			statePtr = statePtr->next;
		}
		freeStates(hashedStates[i]);
	}
	
	for(i=0; i < totalStates[agents]; i++)
		trMat[i][totalStates[agents]] = 1.0;
	
	removeAbsorbState(agents);
	
	return calcExpectation(trMat, totalStates[agents]-1) + 1.0;	
}

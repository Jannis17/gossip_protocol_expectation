/* transition probability element structure */
struct transProbEl_tag {
	   stDT toStateID;
	   float prob;
	   struct transProbEl_tag* next;
};

typedef struct transProbEl_tag transProbEl;

/* list of transition probability elements data type */
struct transProbList_tag {
	transProbEl* first;
	transProbEl* last;
};

typedef struct transProbList_tag* transProbList;


void addElToTrMat(int from, int to, float prob) 
{
	transProbEl* trProbPtr = trMat[from]->first;
	
	while (trProbPtr) 
	{
		if (trProbPtr->toStateID == to ) {
			trProbPtr->prob = trProbPtr->prob + prob;
			return;
		}
		statePtr = statePtr->next;
	}
	
	transProbEl* newTransProbEl = 
		(transProbEl *) malloc(sizeof(transProbEl));
	
	exitIfNULL(newTransProbEl);
	
	newTransProbEl->toStateID=to;
	newTransProbEl->prob=prob;
	newTransProbEl->next = NULL;
			
	if (trMat[from]->first == NULL) 
		trMat[from]->first = trMat[from]->last = newTransProbEl;
	else 
	{	
		trMat[from]->last->next = newTransProbEl;								
		trMat[from]->last = newTransProbEl;	
	}
}

float getTrMatEl(int from, int to)
{
	transProbEl* trProbPtr = trMat[from]->first;
	
	while (trProbPtr) 
	{
		if (trProbPtr->toStateID == to )
			return trProbPtr->prob;
		statePtr = statePtr->next;
	}
	
	return 0.0;
}

void setTrMatElIfnotZero(int from, int to, float prob)
{
	transProbEl* trProbPtr = trMat[from]->first;
	
	while (trProbPtr) 
	{
		if (trProbPtr->toStateID == to )
		{
			trProbPtr->prob = prob;
			return;
		}
		statePtr = statePtr->next;
	}
}

void setTrMatEl(int from, int to, float prob)
{
	transProbEl* trProbPtr = trMat[from]->first;
	
	while (trProbPtr) 
	{
		if (trProbPtr->toStateID == to )
		{
			trProbPtr->prob = prob;
			return;
		}
		statePtr = statePtr->next;
	}
	
	transProbEl* newTransProbEl = 
		(transProbEl *) malloc(sizeof(transProbEl));
	
	exitIfNULL(newTransProbEl);
	
	newTransProbEl->toStateID=to;
	newTransProbEl->prob=prob;
	newTransProbEl->next = NULL;
			
	if (trMat[from]->first == NULL) 
		trMat[from]->first = trMat[from]->last = newTransProbEl;
	else 
	{	
		trMat[from]->last->next = newTransProbEl;								
		trMat[from]->last = newTransProbEl;	
	}

}


/* returns 1 iff g1 and g2 are isomorphic */
int areIsomorphic (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n)
{
	/* this two graphs will store the cannonicaly labeled graphs */
	graph cg1[MAXN*MAXM];
	graph cg2[MAXN*MAXM];
	
	int lab1[MAXN], lab2[MAXN], ptn[MAXN], orbits[MAXN];
	
	/* this is a function for vertex invariants. If it is set to NULL
	 * we loose some optimizations. I am not sure if there are other
	 * side-effects */
	void* adjacencies = NULL;
	
	DEFAULTOPTIONS_DIGRAPH(options);
	statsblk stats;
			
	/* select option for canonical labelling */
    options.getcanon = TRUE;
    
    /* create the cannonicaly labeled graphs */        		
	densenauty(g1,lab1,ptn,orbits,&options,&stats, MAXM,n,cg1);
    densenauty(g2,lab2,ptn,orbits,&options,&stats, MAXM,n,cg2);
	
	/* g1 is iso to g2 iff cg1 is exactly the same as cg2 */
		
	return (compGraphs(cg1, cg2, n)==0)?1:0;
}


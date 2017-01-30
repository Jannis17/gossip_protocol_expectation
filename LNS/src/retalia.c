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

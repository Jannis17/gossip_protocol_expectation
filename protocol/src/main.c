#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"

void printResults(int agentsMin, int agentsMax, int no_states[MAXN],
	float expectation[MAXN], float elpsTime[MAXN], int protocol_name) {
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int agents;
	char *prot_name;
		
	SWITCH_PROT_NAME(protocol_name, prot_name = "LNS", prot_name = "ANY");
		
	/* create the name of the file with timestamp */
	sprintf(filename, 
	"../../results/%s-%d-%d-%d-%dh-%dm-%ds-Agents_%d_to_%d.txt",
	prot_name,
	localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
	localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
	agentsMin, agentsMax);
	
	FILE * fp = fopen (filename, "w+");
    
    EXIT_IF_ERROR_OPENING_FILE(fp, filename);
	
	/* print expectation time, number of states, elapsed time etc. */
	fprintf(fp,"==========================================\n");
	fprintf(fp,"||        The %s protocol              ||\n", 
				prot_name);
	fprintf(fp,"==========================================\n");
	fprintf(fp,"   Timestamp = %d-%d-%d-%dh-%dm-%ds   \n",
		localTime.tm_year + 1900, localTime.tm_mon + 1, 
		localTime.tm_mday,
		localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
	fprintf(fp,"==========================================\n");
	
	for (agents=agentsMin; agents<=agentsMax; agents++) {
		fprintf(fp,"%d agents:\n", agents);
		fprintf(fp,"Number of states = %d\n", no_states[agents]);
		fprintf(fp,"Expected length = %f\n", expectation[agents]);
		fprintf(fp,"Time: %f s\n", elpsTime[agents]);
		fprintf(fp,"==========================================\n");
	}
	
	fclose(fp);
}

int main (int argc, char * argv[]){
	if ( argc !=4 ) 
	{
		printf("Usage: %s [protocol_name] ", argv[0]);
		printf("[agents_min] [agents_max]\n\n");
				
		return 1;
	}
		
	int agentsMin = atoi(argv[2]), agentsMax = atoi(argv[3]);

	if ( agentsMin > agentsMax ) 
	{
		printf("Usage: %s [protocol_name]\n", argv[0]);
		printf("Agents_min should be at most agents_max");
				
		return 1;
	}
	
	if ( agentsMax > MAXN ) 
	{
		printf("Usage: %s [protocol_name]\n", argv[0]);
		printf("Agents_max should be at most %d\n", MAXN);
		return 1;
	}

	int protocol_name;
	
	if ( strcmp(argv[1], "LNS") == 0)
		protocol_name = LNS;
	else if ( strcmp(argv[1], "ANY") == 0 )
			protocol_name = ANY;
		else 	{
			printf("Usage: %s [protocol_name]\n", argv[0]);
			printf("protocol_name should be either ANY or LNS\n\n");
		
			return 1;
		}
	
	/* The following optional call verifies that we are linking
	 * to compatible versions of the nauty routines. */
	nauty_check(WORDSIZE,MAXM,MAXN,NAUTYVERSIONID);

	/* expectation[i] = expected execution length for i agents */
	float expectation[MAXN];

	/* totalStates[i] = number of different states for i agents */
	int no_states[MAXN];
		
	clock_t start, end;
	int agents;
	float elpsTime[MAXN];
	
	//~ printf("%d\n", protocol_name);
													
	for (agents=agentsMin; agents<=agentsMax; agents++) {
		start = clock();		
		expectation[agents] = 
			find_expectation(agents, &no_states[agents], protocol_name);								
		end = clock();
		elpsTime[agents] = ( (float) end - start )/CLOCKS_PER_SEC;	
	}
			
	printResults(agentsMin, agentsMax, no_states, expectation, 
		elpsTime, protocol_name);
	
	//~ graph_test(agentsMin);
	
	//~ printf("MAXM = %d\n", SETWORDSNEEDED(MAXN));
	
	//~ printf("MAXM = %d\n", MAXM);
	
	return 0;
}

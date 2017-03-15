#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "gauss.h"

/* expectation[i] = expected execution length for i agents */
float expectation[MAXN];

int agentsMin; 

int agentsMax; 

float elpsTime[MAXN];

/* generates a file with timestamp and prints results */
void printResults() {
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int agents;
	
	/* create the name of the file with timestamp */
	sprintf(filename, 
	"../../results/LNS-%d-%d-%d-%dh-%dm-%ds-Agents_%d_to_%d.txt",
	localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
	localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
	agentsMin, agentsMax);
	
	FILE * fp = fopen (filename, "w+");
    
    EXIT_IF_ERROR_OPENING_FILE(fp, filename);
	
	/* print expectation time, number of states, elapsed time etc. */
	fprintf(fp,"==========================================\n");
	fprintf(fp,"||        The LNS protocol              ||\n");
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
	if ( ( argc !=3 ) || 
		 ( atoi(argv[1]) > atoi(argv[2]) ) || 
		 ( atoi(argv[2]) > MAXN ) ) 
	{
		printf ("Usage: %s [agentsMin] [agentsMax]\n", argv[0]);
		printf ("agentsMin should be at most agentsMax\n");
		printf ("and agentsMax should be at most %d\n", MAXN);
		return 1;
	}
	
	agentsMin = atoi(argv[1]);
	
	agentsMax = atoi(argv[2]);
	
	/* The following optional call verifies that we are linking
	 * to compatible versions of the nauty routines. */
	nauty_check(WORDSIZE,MAXM,MAXN,NAUTYVERSIONID);
	
	clock_t start, end;
	int agents;
													
	for (agents=agentsMin; agents<=agentsMax; agents++) {
		start = clock();		
		expectation[agents] = findExpectation(agents);								
		end = clock();
		elpsTime[agents] = ( (float) end - start )/CLOCKS_PER_SEC;	
	}
			
	printResults(agentsMin, agentsMax, elpsTime);
	
	//~ graphTest(agentsMin);
	
	//~ printf("MAXM = %d\n", SETWORDSNEEDED(MAXN));
	
	//~ printf("MAXM = %d\n", MAXM);
	
	return 0;
}

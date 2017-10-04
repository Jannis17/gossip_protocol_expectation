#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "simulations.h"

void print_results
(int agents_min, int agents_max, int no_states[MAXN],
 float expectation[MAXN], float elps_time[MAXN], int protocol_name,
 int calc_exp, int sim) 
{
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int agents;
	char *prot_name;
		
	SWITCH_PROT_NAME(protocol_name, prot_name = "LNS", prot_name = "ANY", prot_name = "CO" );
		
	/* create the name of the file with timestamp */
	sprintf(filename, 
	"../../results/%s-%d-%d-%d-%dh-%dm-%ds-Agents_%d_to_%d.txt",
	prot_name,
	localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
	localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
	agents_min, agents_max);
	
	FILE * fp = fopen (filename, "w+");
    
    EXIT_IF_ERROR_OPENING_FILE(fp, filename);
	
	if (sim) {
		fprintf(fp,"agents, expectation\n");
		for (agents=agents_min; agents<=agents_max; agents++)
			fprintf(fp,"%d, %f\n", agents, expectation[agents]);
	}
	else {
		fprintf(fp,"==========================================\n");
		fprintf(fp,"||        The %s protocol              ||\n", 
					prot_name);
		fprintf(fp,"==========================================\n");
		fprintf(fp,"   Timestamp = %d-%d-%d-%dh-%dm-%ds   \n",
			localTime.tm_year + 1900, localTime.tm_mon + 1, 
			localTime.tm_mday,
			localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
		fprintf(fp,"==========================================\n");
		
		for (agents=agents_min; agents<=agents_max; agents++) {
			fprintf(fp,"%d agents:\n", agents);
			fprintf(fp,"Number of states = %d\n", no_states[agents]);
			if (calc_exp)
				fprintf(fp,"Expected length = %f\n", 
					expectation[agents]);
			fprintf(fp,"Elapsed Time = %f s\n", elps_time[agents]);
			fprintf(fp,"==========================================\n");
		}
	}
	fclose(fp);
}

void print_usage_and_exit(int argc, char * argv[])
{
	printf("Usage: %s [name] [min] [max] s ra ne \n\n", argv[0]);
	printf("name: ANY or LNS\n");
	printf("min:  minimum number of agents\n");
	printf("max:  maximum number of agents (%d >= agents_max >= agents_min) \n", MAXN);
	printf("ne:  if it is present, the program will not calculate the expectation. If it is used with the s option, it has no effect.\n");
	printf("s: if it is present, the program calculates simulations. If it is absent, the program caluclates exact values\n");
	printf("ra: if it is present, we have randomization over agents. If it is absent, we have randomization over calls\n");
	printf("\nThe results will be generated in a file with timestamp in the folder gossip_protocol_expectation/results.\n");
			
	exit(1);
}

int main (int argc, char * argv[]){
	int agents_min, agents_max, protocol_name;
	int calc_exp = 1;
	int rand_ag = 0;
	int sim =0;
	
	switch(argc) {
		case 7:
			if (strcmp(argv[6], "ne") == 0)
				calc_exp = 0;
			else print_usage_and_exit(argc, argv);
		case 6:
			if (strcmp(argv[5], "ra") == 0)
				rand_ag = 1;
			else print_usage_and_exit(argc, argv);
		case 5:
			if (strcmp(argv[4], "s") == 0)
				sim = 1;
			else print_usage_and_exit(argc, argv);
		case 4:
			agents_min = atoi(argv[2]);
			agents_max = atoi(argv[3]);
			if ( strcmp(argv[1], "LNS") == 0)
				protocol_name = LNS;
			else if ( strcmp(argv[1], "ANY") == 0 )
				protocol_name = ANY;
			else if ( strcmp(argv[1], "CO") == 0 )
				protocol_name = CO;
			else print_usage_and_exit(argc, argv);
		break;	
		default:
			print_usage_and_exit(argc, argv);
	}

	if ( agents_min > agents_max ) 
		print_usage_and_exit(argc, argv);			
	
	if ( agents_max > MAXN ) 
		print_usage_and_exit(argc, argv);			
	
			
	/* The following optional call verifies that we are linking
	 * to compatible versions of the nauty routines. */
	nauty_check(WORDSIZE,MAXM,MAXN,NAUTYVERSIONID);

	/* expectation[i] = expected execution length for i agents */
	float expectation[MAXN];

	/* totalStates[i] = number of different states for i agents */
	int no_states[MAXN];
		
	clock_t start, end;
	int agents;
	float elps_time[MAXN];
	
	//~ printf("%d\n", protocol_name);
	
	//~ printf("MAXM = %d\n", MAXM);
		
	if (sim) {
		srand(time(NULL));
		for (agents=agents_min; agents<= agents_max; agents++)
			expectation[agents] = simulate(agents, protocol_name);
	}
	else												
		for (agents=agents_min; agents<=agents_max; agents++) {
			start = clock();		
			expectation[agents] = 
				find_expectation(agents, &no_states[agents], 
				protocol_name, calc_exp, rand_ag);								
			end = clock();
			elps_time[agents] = ( (float) end - start )/CLOCKS_PER_SEC;	
		}
			
	
	print_results(agents_min, agents_max, no_states, expectation, 
		elps_time, protocol_name, calc_exp, sim);
	
	//~ graph_test(agents_min);
	
	//~ printf("MAXM = %d\n", SETWORDSNEEDED(MAXN));
	
	
	//~ printf("size of graph = %lu\n", sizeof(graph));
	
	return 0;
}

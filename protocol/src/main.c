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
 float expectation[MAXN], float elps_time[MAXN], int prot,
 int calc_exp, int sim, int rand_ag) 
{
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int agents;
	char *pname;
		
	switch (prot) {
		case (ANY):
			pname = "ANY";
			break;
		case (LNS):
			pname = "LNS";
			break;
		case (CO):
			pname = "CO";
			break;
		case (TOK):
			pname = "TOK";
			break;
		case (SPI):
			pname = "SPI";
			break;
		default:
			INTERNAL_ERROR("Unknown protocol name!");
	}
	
	char *mode=(sim)?"SIM":"EXACT";
	
	char *ra=(rand_ag)?"RA":"RC";
			
	/* create the name of the file with timestamp */
	sprintf(filename, 
	"../../results/%s-%s-%s-%d-%d-%d-%dh-%dm-%ds-agents_%d_to_%d.CSV",
	pname, mode, ra,
	localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
	localTime.tm_hour, localTime.tm_min, localTime.tm_sec, 
	agents_min, agents_max);
	
	FILE * fp = fopen (filename, "w+");
    
    EXIT_IF_ERROR_OPENING_FILE(fp, filename);
	
	fprintf(fp,"agents, ");
	for (agents=agents_min; agents<=agents_max; agents++)
		fprintf(fp,"%d, ", agents);
	
	fprintf(fp,"\nexpectation, ");
	for (agents=agents_min; agents<=agents_max; agents++)
		fprintf(fp,"%f, ", expectation[agents]);
				
	if (!sim) {
		fprintf(fp,"\nstates, ");
		
		for (agents=agents_min; agents<=agents_max; agents++)	
			fprintf(fp,"%d, ", no_states[agents]);
		
		fprintf(fp,"\ntime, ");
		
		for (agents=agents_min; agents<=agents_max; agents++)	
			fprintf(fp,"%f, ", elps_time[agents]);
		
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
	int agents_min, agents_max, prot;
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
				prot = LNS;
			else if ( strcmp(argv[1], "ANY") == 0 )
				prot = ANY;
			else if ( strcmp(argv[1], "CO") == 0 )
				prot = CO;
			else print_usage_and_exit(argc, argv);
		break;	
		default:
			print_usage_and_exit(argc, argv);
	}

	if ( agents_min > agents_max || agents_max > MAXN) 
		print_usage_and_exit(argc, argv);			
	
	printf("MAXM = %d\n", MAXM);
				
	/* The nauty parameter m is a value such that an array of
	m setwords is sufficient to hold n bits. The type setword
	is defined in nauty.h. The number of bits in a setword is
	WORDSIZE, which is 16, 32 or 64. Here we calculate
	m = ceiling(n/WORDSIZE). */
	int m = SETWORDSNEEDED(MAXN);
	
	printf("m = %d\n", m);
	
	printf("WORDSIZE = %d\n", WORDSIZE);
	
	/* The following optional call verifies that we are linking
	to compatible versions of the nauty routines. */
	nauty_check(WORDSIZE,m,MAXN,NAUTYVERSIONID);
	
	/* expectation[i] = expected execution length for i agents */
	float expectation[MAXN];

	/* totalStates[i] = number of different states for i agents */
	int no_states[MAXN];
		
	clock_t start, end;
	int agents;
	float elps_time[MAXN];
	
	//~ printf("%d\n", prot);
	
	printf("MAXM = %d\n", MAXM);
		
	if (sim) {
		srand(time(NULL));
		for (agents=agents_min; agents<= agents_max; agents++)
			expectation[agents] = simulate(agents, prot, rand_ag);
	}
	else for (agents=agents_min; agents<=agents_max; agents++) {
			 start = clock();		
			 expectation[agents] = 
				find_expectation(agents, &no_states[agents], 
				prot, calc_exp, rand_ag);								
			 end = clock();
			 elps_time[agents] = ( (float) end - start )/CLOCKS_PER_SEC;	
		 }
	
	print_results(agents_min, agents_max, no_states, expectation, 
		elps_time, prot, calc_exp, sim, rand_ag);
	
	//~ graph_test(agents_min);
	
	//~ printf("MAXM = %d\n", SETWORDSNEEDED(MAXN));
	
	//~ printf("size of graph = %lu\n", sizeof(graph));
	
	return 0;
}

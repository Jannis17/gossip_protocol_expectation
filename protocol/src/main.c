#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "test.h"
#include "memory.h"
#include "state.h"
#include "simulations.h"

void print_results
(int n, int no_states[MAXN], float expectation[MAXN], 
 float elps_time[MAXN], int prot, int calc_exp, int sim, int rand_ag) 
{
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int i;
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
	"../../results/%s-%s-%s-%d-%d-%d-%dh-%dm-%ds-agents_1_to_%d.CSV",
	pname, mode, ra,
	localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
	localTime.tm_hour, localTime.tm_min, localTime.tm_sec, n);
	
	FILE * fp = fopen (filename, "w+");
    
    EXIT_IF_ERROR_OPENING_FILE(fp, filename);
	
	fprintf(fp,"agents, ");
	for (i=1; i<=n; i++)
		fprintf(fp,"%d, ", i);
	
	fprintf(fp,"\nexpectation, ");
	for (i=1; i<=n; i++)
		fprintf(fp,"%f, ", expectation[i]);
				
	if (!sim) {
		fprintf(fp,"\nstates, ");
		
		for (i=1; i<=n; i++)	
			fprintf(fp,"%d, ", no_states[i]);		
	}	
	
	fprintf(fp,"\ntime, ");
		
	for (i=1; i<=n; i++)	
		fprintf(fp,"%f, ", elps_time[i]);
	
	fclose(fp);
}

void print_usage_and_exit(int argc, char * argv[])
{
	printf("Usage: %s [name] [n] -s=[max_sim] -ra -ne \n\n", argv[0]);
	printf("name: ANY or LNS\n");
	printf("n:  number of agents (in the range 1..%d) \n", MAXN);
	printf("-ne:  if it is present, the program will not calculate the expectation. If it is used with the s option, it has no effect.\n");
	printf("-s: if it is present, the expected duration is computed using max_sim simulations. If it is absent, the program caluclates exact values\n");
	printf("-ra: if it is present, we have randomization over agents. If it is absent, we have randomization over calls\n");
	printf("\nThe results will be generated in a file with timestamp in the folder gossip_protocol_expectation/results.\n");
			
	exit(1);
}

int main (int argc, char * argv[]){
	int max_n, prot;
	int calc_exp = 1;
	int rand_ag = 0;
	int sim =0;
	int max_sim=0;
	
	switch(argc) {
		case 6:
			if (strcmp(argv[5], "-ne") == 0)
				calc_exp = 0;
			else print_usage_and_exit(argc, argv);
		case 5:
			if (strcmp(argv[4], "-ra") == 0)
				rand_ag = 1;
			else print_usage_and_exit(argc, argv);
		case 4:
			if (sscanf(argv[3], "-s=%d", &max_sim) ==1)
				sim = 1;
			else print_usage_and_exit(argc, argv);
		case 3:
			max_n = atoi(argv[2]);
			if ( strcmp(argv[1], "LNS") == 0)
				prot = LNS;
			else if ( strcmp(argv[1], "ANY") == 0 )
				prot = ANY;
			else if ( strcmp(argv[1], "CO") == 0 )
				prot = CO;
			else if ( strcmp(argv[1], "TOK") == 0 )
				prot = TOK;
			else if ( strcmp(argv[1], "SPI") == 0 )
				prot = SPI;
			else print_usage_and_exit(argc, argv);
		break;	
		default:
			print_usage_and_exit(argc, argv);
	}

	if ( max_n > MAXN || max_n<=0 ) 
		print_usage_and_exit(argc, argv);			
			
	/* expectation[i] = expected execution length for i i */
	float expectation[MAXN];

	/* totalStates[i] = number of different states for i i */
	int no_states[MAXN];
		
	clock_t start, end;
	int n, m;
	float elps_time[MAXN];
		
	srand(time(NULL));
		
	//~ for (n=3; n<=max_n; n++) {
		//~ /* calculate the number of words needed to hold n bits */
		 //~ m = SETWORDSNEEDED(n);
		 			
		 //~ /* verify that we are linking to compatible versions of the
		  //~ * nauty routines. */
		 //~ nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
		 
		 //~ start = clock();		
		 
		 //~ /* compute the simulated or exact expectation */
		 //~ expectation[n] = (sim)?simulated(n, m, prot, rand_ag, max_sim):
	     //~ exact_expectation(n, m, &no_states[n], prot, calc_exp, rand_ag);								
		 
		 //~ end = clock();
		 //~ elps_time[n] = ((float) end - start)/CLOCKS_PER_SEC;
	//~ }
	
	//~ expectation[1]=0;
	//~ expectation[2]=1;
	//~ elps_time[1]=elps_time[2]=0;
	//~ no_states[1]=1;
	//~ no_states[2]=2;
			
	//~ print_results(max_n, no_states, expectation, elps_time, prot, 
		//~ calc_exp, sim, rand_ag);
	
	m = SETWORDSNEEDED(max_n);
	graph_test(max_n,m);
		
	return 0;
}

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
(int n, int no_states[MAXN], float expectation[MAXN_SIM], 
 float elps_time[MAXN_SIM], int prot, int calc_exp, int sim,
 int no_ordered_tuples[MAXN], int o_ANY) 
{
	time_t timeNow = time(NULL);
	struct tm localTime = *localtime(&timeNow);
	char filename[300];
	int i;
	char *pname;
	char *name_of_states;
		
	switch (prot) {
		case (ANY):
			pname = "ANY";
			name_of_states =  o_ANY? "ordered tuples" :"unordered tuples";
			break;
		case (LNS):
			pname = "LNS";
			name_of_states = "ordered tuples";
			break;
		case (CO):
			pname = "CO";
			name_of_states = "weighted graphs";
			break;
		case (TOK):
			pname = "TOK";
			name_of_states = "coloured ordered tuples";
			break;
		case (SPI):
			pname = "SPI";
			name_of_states = "coloured ordered tuples";
			break;
		default:
			INTERNAL_ERROR("Unknown protocol name!");
	}
	
	char *mode=(sim)?"SIM":"EXACT";
			
	/* create the name of the file with timestamp */
	sprintf(filename, 
	"../../results/%s-%s-%d-%d-%d-%dh-%dm-%ds-agents_1_to_%d.CSV",
	pname, mode,
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
		fprintf(fp,"\n%s, ", name_of_states);
		
		for (i=1; i<=n; i++)	
			fprintf(fp,"%d, ", no_states[i]);		
	}	
	
	if (!sim && (prot == CO || prot == SPI || prot == TOK) ) {
		fprintf(fp,"\nordered tuples, ");
		
		for (i=1; i<=n; i++)	
			fprintf(fp,"%d, ", no_ordered_tuples[i]);		
	}	
		
	if (!sim){
		fprintf(fp,"\ntime, ");
		
		for (i=1; i<=n; i++)	
			fprintf(fp,"%f, ", elps_time[i]);
	}
	fclose(fp);
}

void print_usage_and_exit(int argc, char * argv[])
{
	printf("Usage: %s [name] [n] -s=[max_sim] -o -ne \n\n", argv[0]);
	printf("name: ANY, LNS, CO or SPI\n");
	printf("n:  maximum number of agents \n");
	printf("-ne:  if it is present, the program will not calculate the expectation. If it is used with the s option, it has no effect.\n");
	printf("-s: if it is present, the expected duration is estimated using max_sim simulations. If it is absent, the program caluclates exact values\n");
	printf("-o: It should be used only with the name option ANY. If it is present, the program produces the ANY ordered tuples. If it is absent the program produces the ANY unordered tuples\n\n");
	printf("If the option -s is used then n has to be in the range 1..%d. If -s is absent, n has to be in the range 1..%d", MAXN_SIM, MAXN);	
	printf("\nThe results will be generated in a file marked with timestamp in the folder gossip_protocol_expectation/results.\n");
			
	exit(1);
}

int main (int argc, char * argv[]){
	int max_n = 0, prot;
	int calc_exp = 1;
	int o_ANY = 0;
	int sim =0;
	int max_sim=0;
	
	switch(argc) {
		case 6:
			if (strcmp(argv[5], "-ne") == 0)
				calc_exp = 0;
			else if (strcmp(argv[5], "-o") == 0)	
				 o_ANY = 1;
			else if (sscanf(argv[5], "-s=%d", &max_sim) ==1)
				 sim = 1;
			else print_usage_and_exit(argc, argv);
		case 5:
			if (strcmp(argv[4], "-ne") == 0)
				calc_exp = 0;
			else if (strcmp(argv[4], "-o") == 0)	
				 o_ANY = 1;
			else if (sscanf(argv[4], "-s=%d", &max_sim) ==1)
				 sim = 1;
			else print_usage_and_exit(argc, argv);
		case 4:
			if (strcmp(argv[3], "-ne") == 0)
				calc_exp = 0;
			else if (strcmp(argv[3], "-o") == 0)	
				 o_ANY = 1;
			else if (sscanf(argv[3], "-s=%d", &max_sim) ==1)
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

	if ( max_n<=0 || (sim && max_n > MAXN_SIM) 
		 || (!sim && max_n > MAXN) || (o_ANY && prot!= ANY)) 
		print_usage_and_exit(argc, argv);			
			
	/* expectation[i] = expected execution length for i agents */
	float expectation[MAXN_SIM];

	/* no_tates[i] = number of different states for i agents */
	int no_states[MAXN];
	
	/* no_ordered_tuples[i] = number of non-iso ordered tuples for
	 * i agents */
	int no_ordered_tuples[MAXN];
		
	clock_t start, end;
	int n, m;
	float elps_time[MAXN_SIM];
	
	pars_t pars;
		
	srand(time(NULL));
		
	for (n=3; n<=max_n; n++) {
		/* calculate the number of words needed to hold n bits */
		m = SETWORDSNEEDED(n);
		 			
		/* verify that we are linking to compatible versions of the
		 * nauty routines. */
		nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
		 
		pars.n = n;
		pars.m = m;
		pars.prot=prot;
		pars.calc_exp=calc_exp;
	    pars.max_sim=max_sim;
		pars.o_ANY=o_ANY;
		  
		start = clock();
		/* compute the simulated or exact expectation */
		expectation[n] = (sim)?simulated(pars):
	    exact_expectation(&no_states[n], &no_ordered_tuples[n], pars);								
		 
		end = clock();
		elps_time[n] = ((float) end - start)/CLOCKS_PER_SEC;
	}
	
	expectation[1]=0;
	expectation[2]=1;
	elps_time[1]=elps_time[2]=0;
	no_states[1]=1;
	no_states[2]=2;
	no_ordered_tuples[1]=1;
	no_ordered_tuples[2]=2;
			
	print_results(max_n, no_states, expectation, elps_time, prot,
		calc_exp, sim, no_ordered_tuples, o_ANY);
	
	//~ m = SETWORDSNEEDED(max_n);
	//~ graph_test(max_n,m);
		
	return 0;
}

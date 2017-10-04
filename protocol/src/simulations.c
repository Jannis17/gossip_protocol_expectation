#include "simulations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "queue.h"
#include "compar.h"
#include "test.h"
#include "../../nauty26r7/nauty.h"

int count_avail_calls(graph avail_calls[MAXN*MAXM], int n) {
	int count=0;
	
	size_t i;
		
	for (i=0; i < MAXM*(size_t) n; i++)
		count+=POPCOUNT(avail_calls[i]);
	
	return count;
}

void execute_r_call(int r_call, graph secrets[MAXN*MAXM], 
	graph avail_calls[MAXN*MAXM],int n) {
	int i,j;
	
	for (i=0; i < MAXM*(size_t) n; i++)
		for (j=0; j < MAXM*(size_t) n; j++) {
			r_call-=ISELEMENT(GRAPHROW(avail_calls,i,MAXM), j);
			if (r_call == 0) {
				make_call(secrets, i, j);
				return;
			}
		}
			
}


float simulate (int n) {
	int s, r_call, duration=0;		
	graph secrets[MAXN*MAXM];
	graph avail_calls[MAXN*MAXM];
						
	for(s=0; s<MAX_SIM;s++) {
		init_secrets_graph(secrets, n);
		init_avail_calls_graph(avail_calls,n);
		while (1) {
			r_call = rand() % count_avail_calls(avail_calls,n);
			execute_r_call(r_call, secrets, avail_calls, n);
			duration++;
			if (edges_of(secrets,n) == n * n)
				break;
		}
	}
		
	return ((float) duration)/MAX_SIM;
}

#include "simulations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "state.h"
#include "test.h"
#include "../../nauty26r7/nauty.h"
#include "../../nauty26r7/naututil.h"

void execute_r_call(int r_call, graph secrets[MAXN*MAXM], 
	graph avail_calls[MAXN*MAXM],int n, int protocol_name) {
	int i, j, k;
		
	for (i=0; i < MAXM*(size_t) n; i++)
		for (j=0; j < MAXM*(size_t) n; j++) {
			r_call-=ISELEMENT(GRAPHROW(avail_calls,i,MAXM), j);
			if (!r_call) {
				make_call(secrets, i, j);
				if (protocol_name == LNS) {
					for(k=0;k<n;k++)
						if (ISELEMENT(GRAPHROW(secrets,i,MAXM),k)) {
							DELELEMENT(GRAPHROW(avail_calls,i,MAXM),k);
							DELELEMENT(GRAPHROW(avail_calls,j,MAXM),k);							
						}
						else {
							ADDELEMENT(GRAPHROW(avail_calls,i,MAXM),k);
							ADDELEMENT(GRAPHROW(avail_calls,j,MAXM),k);
						}
				}
				else if (protocol_name == CO) {
					DELELEMENT(GRAPHROW(avail_calls,i,MAXM),j);
					DELELEMENT(GRAPHROW(avail_calls,j,MAXM),i);
				}
				return;
			}
		}
}

float simulate (int n, int protocol_name) {
	int s, r_call, duration=0;		
	graph secrets[MAXN*MAXM];
	graph avail_calls[MAXN*MAXM];
							
	for(s=0; s<MAX_SIM;s++) {
		init_secrets_graph(secrets, n);
		init_avail_calls_graph(avail_calls,n);
		while (1) {
			r_call = rand() % edges_of(avail_calls,n) + 1;
			printf("r_call = %d\n", r_call);
			execute_r_call(r_call, secrets, avail_calls, n, protocol_name);
			duration++;
			
			if (edges_of(secrets,n) == n * n)
				break;
		}
	}
		
	return ((float) duration)/MAX_SIM;
}

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



void execute_call(int caller, int callee, graph secrets[MAXN*MAXM], 
	graph avail_calls[MAXN*MAXM],int n, int m, int prot) 
{
	size_t k;
		
	update_secrets(secrets, caller, callee, n, m);
	if (prot == LNS) {
		for(k=0;k<n;k++)
		if (ISELEMENT(GRAPHROW(secrets,caller,m),k)) {
			DELELEMENT(GRAPHROW(avail_calls,caller,m),k);
			DELELEMENT(GRAPHROW(avail_calls,callee,m),k);							
		} else {
			ADDELEMENT(GRAPHROW(avail_calls,caller,m),k);
			ADDELEMENT(GRAPHROW(avail_calls,callee,m),k);
		}
	} else if (prot == CO) {
				DELELEMENT(GRAPHROW(avail_calls,caller,m),callee);
				DELELEMENT(GRAPHROW(avail_calls,callee,m),caller);
			}		
}

int count_callers(graph avail_calls[MAXN*MAXM], int n, int m) 
{
	int count=0;
	
	size_t i, j;
	
	for(i=0; i < MAXM*(size_t) n ; i++)
		for(j=0; j < MAXM*(size_t) n ; j++) 		
			if (ISELEMENT(GRAPHROW(avail_calls,i,m),j)) { 
				count++;
				break;
			}
	
	return count;
}

int count_callees(graph avail_calls[MAXN*MAXM], int i, int n, int m) {
	int count=0;
	
	size_t j;
	
	for(j=0; j < MAXM*(size_t) n ; j++) 		
		if (ISELEMENT(GRAPHROW(avail_calls,i,m),j))
			count++;
	
	return count;
}

int get_caller(graph avail_calls[MAXN*MAXM], int r_caller, int n, int m)
{
	size_t i;
	
	for(i=0; i < MAXM*(size_t) n ; i++) {
		if (count_callees(avail_calls, i, n, m) > 0)
			r_caller--;
		if (!r_caller)
			break;
	}
	
	return i;
}

int get_callee(graph avail_calls[MAXN*MAXM], int caller, 
	int r_callee, int n, int m){
	size_t j;
	
	for(j=0; j < m*(size_t) n ; j++) {
		if (ISELEMENT(GRAPHROW(avail_calls,caller,m),j))
			r_callee--;
		if (!r_callee)
			break;
	}
	
	return j;
}

void get_call_parts(graph avail_calls[MAXN*MAXM], int r_call, 
	int *caller, int *callee, int n, int m){
	
	size_t i,j;
	
	*caller=*callee=0;
	
	for (i=0; i < m*(size_t) n; i++)
		for (j=0; j < m*(size_t) n; j++) {
			r_call-=ISELEMENT(GRAPHROW(avail_calls,i,m), j);
			if (!r_call) {
				*caller = i;
				*callee =j;
				return;
			}
		}
}


float simulated(int n, int m, int prot, int rand_ag, int max_sim) {
	int s, r_caller, r_callee, duration=0;		
	graph secrets[MAXN*MAXM];
	graph avail_calls[MAXN*MAXM];
							
	for(s=0; s<max_sim;s++) {
		diagonal(secrets, n, m);
		init_avail_calls_graph(avail_calls,n,m);
		while (1) {
			if (rand_ag) {
				r_caller = get_caller(avail_calls, 
					rand() % count_callers(avail_calls,n, m)+1, n, m);
				r_callee = get_callee(avail_calls, r_caller,
					rand() % count_callees(avail_calls,r_caller,n, m)+1, n, m);
			} else
				get_call_parts(avail_calls, 
					rand() % edges_of(avail_calls,n,m) + 1, &r_caller, 
					&r_callee, n, m);
			execute_call(r_caller, r_callee, secrets, avail_calls, n, m, prot);
			duration++;
			
			if (edges_of(secrets,n,m) == n * n)
				break;
		}
	}
		
	printf("Agents %d done!\n", n);
	return ((float) duration)/max_sim;
}

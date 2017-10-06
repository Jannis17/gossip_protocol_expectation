#ifndef H_GRAPH_H_
#define H_GRAPH_H_

#include "../../nauty26r7/nauty.h"

int edges_of (graph g[MAXN*MAXM], int n);

void diagonal (graph g[MAXN*MAXM], int n);

void init_avail_calls_graph (graph g[MAXN*MAXM], int n);

void copy_graph (graph to[MAXN*MAXM], graph from[MAXN*MAXM], int n);

void find_can_label 
	(graph from[MAXN*MAXM], graph to[MAXN*MAXM], int n);
	
void make_call(graph g[MAXN*MAXM], int i, int j);

int no_LNS_calls(graph g[MAXN * MAXM], int i, int j);

int can_call(graph g[MAXN * MAXM], int i, int j);

#endif /* H_GRAPH_H_ */

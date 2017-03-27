#ifndef H_GRAPH_H_
#define H_GRAPH_H_

#include "../../nauty26r7/nauty.h"

int edges_of (graph g[MAXN*MAXM], int n);

int comp_graphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n);

void init_secrets_graph (graph g[MAXN*MAXM], int n);

void copy_graph (graph to[MAXN*MAXM], graph from[MAXN*MAXM], int n);

void find_can_label 
	(graph from[MAXN*MAXM], graph to[MAXN*MAXM], int n);
	
void make_call(graph g[MAXN*MAXM], int i, int j);

void print_graph(graph g[MAXN*MAXM], int n);

int poss_calls (graph g[MAXN * MAXM], int i, int j);

void graph_test(int n);

int comp_perm_graphs(graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n);

#endif /* H_GRAPH_H_ */

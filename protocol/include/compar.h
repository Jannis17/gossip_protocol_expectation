#ifndef H_COMPAR_H_
#define H_COMPAR_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

#define LESS -1
#define EQUAL 0
#define GREATER 1

 int cmp_graph_nodes (const void *p, const void *q);

 int comp_can_secrets(const void* item1, const void* item2);

 int comp_children(const void* item1, const void* item2);

 int comp_fixed_name_secrets(const void* item1, const void* item2);

 int comp_graphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n);

#endif /* H_COMPAR_H_ */

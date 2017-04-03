#ifndef H_COMPAR_H_
#define H_COMPAR_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

#define LESS -1
#define EQUAL 0
#define GREATER 1

extern int comp_nodes (const void *p, const void *q);

extern int comp_can_secrets(const void* item1, const void* item2);

extern int comp_children(const void* item1, const void* item2);

extern int comp_fixed_name_secrets(const void* item1, const void* item2);

extern int comp_graphs (graph g1[MAXN*MAXM], graph g2[MAXN*MAXM], int n);

#endif /* H_COMPAR_H_ */

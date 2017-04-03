#ifndef H_MEMORY_H_
#define H_MEMORY_H_

#include "state.h"

#define EXIT_IF_OUT_OF_MEMORY(ptr)			\
	do {					\
		if(!(ptr)) {			\
			fprintf(stderr, "Out of memory\n");	\
			exit(1);		\
		}				\
	} while(0)
	
#define EXIT_IF_ERROR_OPENING_FILE(fp, filename)			\
	do {					\
		if(!(fp)) {			\
			fprintf(stderr, "Error opening file: %s\n", (filename));	\
			exit(1);		\
		}				\
	} while(0)

#define INTERNAL_ERROR(message)			\
	do {					\
		fprintf(stderr, "Internal error: %s\n", (message));	\
		exit(1);	\
	} while(0)


#define MALLOC_SAFE(p, siz) \
do {					\
	(p)= malloc((siz)); \
	EXIT_IF_OUT_OF_MEMORY(p); \
} while(0)

#define MALLOC_ZERO(p, siz)\
do {					\
  MALLOC_SAFE((p), (siz)); \
  memset((p), 0x00, (siz)); \
} while(0)

#define FREE_SAFE(p) \
do {	\
	if ((p)) {	\
		free((p));		\
		(p) = NULL;	\
	}	\
} while(0)

extern protocol_state_t* new_protocol_state 
(graph g[MAXN*MAXM], int agents, int protocol_name);

extern child_t *new_child
( graph secrets[MAXN*MAXM], int agents, 
  int protocol_name, int calls_to_child);
  
extern void destroy_child(child_t *child);

extern void destroy_hash(int agents, twin_queues hash[MAXN*MAXN]);

void init_hash
(twin_queues hash[MAXN*MAXN], int agents, int protocol_name);

protocol_state_t* new_protocol_state 
(graph g[MAXN*MAXM], int agents, int protocol_name);

child_t *new_child
( graph secrets[MAXN*MAXM], int agents, 
  int protocol_name, int calls_to_child);

#endif /* H_MEMORY_H_ */

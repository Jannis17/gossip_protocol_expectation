#ifndef H_MEMORY_H_
#define H_MEMORY_H_

#include "state.h"
#include "queue.h"

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

pstate_t* 
new_pstate(graph secrets[MAXN*MAXM], int calls[MAXN][MAXN],
int token[MAXN], int total_calls, pars_t pars);
  
void
destroy_hash(int agents, twin_queues hash[MAXN*MAXN]);

child_t*
new_child(pstate_t* childs_state, int calls_to_child);
  
void
destroy_protocol_state (pstate_t ** s);
 
void
destroy_twin_queues(twin_queues* twin_q);

void
malloc_safe_2D_float(float ***p, int n); 

void
free_safe_2D_float(float ***p, int n);

void
copy_tokens(int from[MAXN], int to[MAXN], int n);

int
count_tokens(int token[MAXN], int n);

#endif /* H_MEMORY_H_ */

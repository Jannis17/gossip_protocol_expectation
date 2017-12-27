#ifndef H_MAIN_H_
#define H_MAIN_H_

#define LNS 4
#define CO 3
#define SPI 2
#define TOK 1
#define ANY 0

/* MAXN = ULTIMATELY MAXIMUM number of agents */
#define MAXN 40

#include "../../nauty26r7/nauty.h"

typedef struct pars_tag {
	int n;//number of agents
	int m;//number of setwords 
	int prot;//protocol name
	int calc_exp;//calculate expectation flag
	int rand_ag;//randomization over agents flag
	int max_sim;
} pars_t;

#endif /* H_MAIN_H_ */

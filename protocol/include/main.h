#ifndef H_MAIN_H_
#define H_MAIN_H_

#define CO 2
#define LNS 1
#define ANY 0

/* MAXN = ULTIMATELY MAXIMUM number of agents */
#define MAXN 50

/* MAX_SIM = maximum number of simulations */
#define MAX_SIM 100

#include "../../nauty26r7/nauty.h"

#define SWITCH_PROT_NAME(protocol_name, LNS_COMM, ANY_COMM, CO_COMM)			\
	do {					\
		switch (protocol_name) {	\
				case LNS:	\
					LNS_COMM;	\
					break;	\
				case ANY:	\
					ANY_COMM;	\
					break;	\
				case CO:	\
					CO_COMM;	\
					break;	\
				default:	\
					INTERNAL_ERROR("Unknown protocol name!");	\
			}	\
	} while(0)

#endif /* H_MAIN_H_ */

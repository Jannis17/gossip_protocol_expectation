#ifndef H_MAIN_H_
#define H_MAIN_H_

#define LNS 1
#define ANY 0

/* MAXN = ULTIMATELY MAXIMUM number of agents */
#define MAXN 10

#include "../../nauty26r7/nauty.h"

#define SWITCH_PROT_NAME(protocol_name, LNS_COMM, ANY_COMM)			\
	do {					\
		switch (protocol_name) {	\
				case LNS:	\
					LNS_COMM;	\
					break;	\
				case ANY:	\
					ANY_COMM;	\
					break;	\
				default:	\
					printf("Internal Error: Unknown protocol name!");\
					exit(1);\
			}	\
	} while(0)

#endif /* H_MAIN_H_ */

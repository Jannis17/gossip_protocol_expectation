#ifndef H_STATE_H_
#define H_STATE_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

#define LESS -1
#define EQUAL 0
#define GREATER 1

#define MAXSTATES 10000000

#define FOR_ALL_EDGES(VAR, AGENTS)					\
	for((VAR)= (AGENTS)-1; i < (AGENTS) * (AGENTS); (VAR)++)

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
					printf("Internal Error: Unknown protocol name!");	\
					exit(1);\
			}	\
	} while(0)


float findExpectation (int agents, int* no_states, int protocol_name);

#endif /* H_STATE_H_ */

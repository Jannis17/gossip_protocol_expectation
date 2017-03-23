#ifndef H_MEMORY_H_
#define H_MEMORY_H_

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
	

#endif /* H_MEMORY_H_ */

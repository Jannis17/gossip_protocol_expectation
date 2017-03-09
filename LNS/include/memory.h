#ifndef H_MEMORY_H_
#define H_MEMORY_H_

#define EXIT_IF_NULL(ptr)			\
	do {					\
		if(!(ptr)) {			\
			fprintf(stderr, "Out of memory\n");	\
			exit(1);		\
		}				\
	} while(0)

#define MALLOC_1DARRAY (array, dim, type)			\
	do {					\
		(array) = (type *) malloc( (dim) * sizeof(type));	\
		EXIT_IF_NULL(array);	\
	} while(0)

#endif


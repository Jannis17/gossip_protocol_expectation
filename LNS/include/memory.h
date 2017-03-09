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

#define MALLOC_1DARRAY (array, dim, type)			\
	do {					\
		(array) = (type *) malloc( (dim) * sizeof(type));	\
		EXIT_IF_OUT_OF_MEMORY(array);	\
	} while(0)

#endif

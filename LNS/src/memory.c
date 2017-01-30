#include "mainHeader.h"
#include "memory.h"


/* exits if ptr == NULL */
void exitIfNULL(void * ptr)
{
	if (ptr == NULL) 
	{
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
}

/* allocates memory for a 1d-array of floats */
void alloc1darrayDouble(float **array, int dim) 
{
	*array = (float *) malloc(dim*sizeof(float));
	exitIfNULL(*array);	
}


/* allocates memory for a 2d-array of floats */
void alloc2darrayDouble(float ***array, int dim1, int dim2) 
{
	int i;
	
	*array = (float **) malloc(dim1*sizeof(float *));
	exitIfNULL(*array);
	
	for(i=0; i<dim1; i++) {
		alloc1darrayDouble( &((*array)[i]), dim2);
		exitIfNULL((*array)[i]);
	}
}

/* frees a 2d-array of floats that was allocated by alloc2darray*/
void free2darrayDouble(float ***array, int dim1) 
{
	int i;
			
	for (i=0; i<dim1; i++)
		free((*array)[i]);
	
	free(*array);
}

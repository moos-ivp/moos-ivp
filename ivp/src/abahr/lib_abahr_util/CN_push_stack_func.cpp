// function that pushes a new row vector on top of existing stack
//
// INPUT ARGUMENTS:
//
//   old_stack: matrix where each line corresponds to a stack level, contains new stack after execution
//
//   new_vector: new vector which gets pushed on stack
//
//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 10/08/2006        abahr       created

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CN.h"
using namespace std;


void CN_push_stack_func(gsl_matrix **old_stack,bool *stack_init,gsl_vector *new_vector, bool debug)
{
	int n,m;
	
	int num_rows;
	int num_cols;
	
	num_rows = (*old_stack)->size1;
	num_cols = (*old_stack)->size2;
	
	if(debug) MOOSTrace("CN_push_stack_func: Actual size of stack: %dx%d\n",num_rows,num_cols);
    if(debug)
    {
    	if(*stack_init)
    	{
    		MOOSTrace("CN_push_stack_func: stack is intialized\n");
    	}
    	else
    	{
    		MOOSTrace("CN_push_stack_func: stack is NOT initialized\n");
    	}
    }
	
	// if the stack has been initialized add a line to it and move the elements up 
	if(*stack_init)
	{
		// create new stack which is one line longer than my_stack
		gsl_matrix *new_stack = gsl_matrix_calloc(num_rows+1,num_cols);
		
		// copy old entries from cn to cn_new and move them all one row down
	    for(n=0;n<=num_rows-1;n++)
		{
			for(m=0;m<=num_cols-1;m++)
			{
				gsl_matrix_set(new_stack,n+1,m,gsl_matrix_get(*old_stack,n,m));
			}
		}
		
		// copy new entry from vector to new_stack
		for(m=0;m<=num_cols-1;m++)
		{
			gsl_matrix_set(new_stack,0,m,gsl_vector_get(new_vector,m));
		}
		
		// free old matrix cn and set pointer to new matrix
		gsl_matrix_free(*old_stack);
		*old_stack=new_stack;
	}
	
	// if the stack has NOT been initialized fill the first line 
	else
	{
		// create new stack which has the same size
		gsl_matrix *new_stack = gsl_matrix_calloc(num_rows,num_cols);
		
		// copy new entry from vector to new_stack
		for(m=0;m<=num_cols-1;m++)
		{
			gsl_matrix_set(new_stack,0,m,gsl_vector_get(new_vector,m));
		}
		
		// free old matrix cn and set pointer to new matrix
		gsl_matrix_free(*old_stack);
		*old_stack=new_stack;
		*stack_init=true;
	}
	MOOSTrace("CN_push_stack_func: Finished stack push\n");
	
}

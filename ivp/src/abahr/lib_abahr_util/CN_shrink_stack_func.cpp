// function that sort the rows of the matrix in ascending order of the values in sort_column and then shortens the stack
// to new_depth rows by elminiating the rows with a row number > new_dpeth
//
// INPUT ARGUMENTS:
//
//   old_stack:      matrix where each line corresponds to a stack level
//   new_depth:      new depth of stack after shrinking
//   sort_column:    column along which the stack rows will be sorted and eliminated
//                   !!! ATTENTION: COLUMN MUST BE BETWEEN 0 AND num_cols-1 !!!
//   
//
// MODIFIED RETURN ARGUMENTS:   
//
//   old_stack:   my_stack sorted along sort_column with the rows with highest values in sort_column removed such that
//               the new stack depth is new_depth
//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 10/08/2006        abahr       created

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CN.h"
using namespace std;

int CN_shrink_stack_func(gsl_matrix **old_stack,int new_depth,int sort_column)
{
	int n;
	
	int num_rows;
	int num_cols;
	
	int sort_pos;
	
	int error_flag=0;
	
	num_rows = (*old_stack)->size1;
	num_cols = (*old_stack)->size2;
	
	// if the stack is shorter than new_depth, keep the length for new_stack
	if(num_rows<=new_depth)
	{
		new_depth=num_rows;
	}
		
	// create new stack which has the length new_depth
	gsl_matrix *new_stack = gsl_matrix_calloc(new_depth,num_cols);
	
	// create buffer for row to be transferred from the old_stack to new_stack
	gsl_vector *transfer_row = gsl_vector_calloc(num_cols);
	
	// create a vector which contains sort_column of old_stack and extract it
	gsl_vector *stack_column = gsl_vector_calloc(num_rows);
	gsl_matrix_get_col(stack_column,*old_stack,sort_column);
	
	// create vector which contains the sort permutation
	gsl_permutation *sort_permu = gsl_permutation_calloc(num_rows);
	
	// sort stack_column, store result in sort_permu
	error_flag=gsl_sort_vector_index(sort_permu,stack_column);

	for(n=0;n<=num_rows-1;n++)
	{
		sort_pos = gsl_permutation_get(sort_permu,n);
		
		// if the value in sort_pos will be within the new stack, copy the row out of old_stack into new_stack
		if(sort_pos<=new_depth-1)
		{
			gsl_matrix_get_row(transfer_row,*old_stack,n);
			gsl_matrix_set_row(new_stack,sort_pos,transfer_row);
		}
	}
	
	// free elements in the order they were created
	gsl_vector_free(transfer_row);
	gsl_vector_free(stack_column);
	gsl_permutation_free(sort_permu);
	
	// free old matrix cn and set pointer to new matrix
	gsl_matrix_free(*old_stack);
	*old_stack=new_stack;
	
	return error_flag;
}

// add a new set of data to the cn table  to the line after the "last_entry" indicator
// if the "last_entry" indicator is at the last line, move evrything up one row and then add entry at the last line
// cn:
//
//     we assume the following matrix (my = my data ; ov = CNA's data), each line is a step in time
//
//     COL 0     COL 1       COL 2       COL 3       COL 4       COL 5       
//
//     my x-pos  my y-pos    my cov(xx)  my cov(xy)  my cov(yx)  my cov(yy)
//
//
//
//
//     COL 6     COL 7       COL 8       COL 9       COL 10      COL 11       
//
//     ov x-pos  ov y-pos    ov cov(xx)  ov cov(xy)  ov cov(yx)  ov cov(yy)
//
//
//
//     COL 12    COL 13
//
//     ov id     range between my and ov
//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 9/8/2006        abahr       created
// 17/8/2006	   abahr       changed style of cn matrix col 3-5 and col 9-11

#include "CN.h"
using namespace std;

int CN_add_to_cn_table(gsl_matrix **cn, bool *cn_init,int max_length, double my_x, double my_y, double my_cov_xx, double my_cov_xy, double my_cov_yx, double my_cov_yy, double ov_x, double ov_y, double ov_cov_xx, double ov_cov_xy, double ov_cov_yx, double ov_cov_yy, int ov_id, double range, bool debug)
{
	int error_flag=0;
	
	int n,m;
	
	int num_rows;
	int num_cols;
	
	num_rows = (*cn)->size1;
	num_cols = (*cn)->size2;
	
	if(debug) MOOSTrace("CN_add_to_cn_table: Actual size of cn-table: %dx%d ; max_length=%d\n",num_rows,num_cols,max_length);
    if(debug)
    {
    	if(*cn_init)
    	{
    		MOOSTrace("CN_add_to_cn_table: cn is intialized\n");
    	}
    	else
    	{
    		MOOSTrace("CN_add_to_cn_table: cn is NOT initialized\n");
    	}
    }
              
	// check if the last_entry indicator is larger than the size of the table
	if(num_rows>max_length)
	{
		if(debug) MOOSTrace("CN_add_to_cn_table: Length of cn table exceeds maximum length\n");
		error_flag=1;
	} 
	
	// check if the number of rows is the same as max_length
	// OR if the matrix has not been initialized, (but has length 1)
	if((num_rows==max_length) || (!*cn_init))
	{
		if(debug) MOOSTrace("CN_add_to_cn_table: Creating new %dx%d stack\n",num_rows,num_cols);
		
		// if yes, create a new matrix of the same length, drop the first line and move everything one line up
		gsl_matrix *cn_new = gsl_matrix_calloc(num_rows,num_cols);
		
		// if the we are dealing with the case of an initialized matrix, 
		// copy entries from cn to cn_new while moving everything one line up
		// otherwise skip that step
		if(*cn_init)
		{
			if(debug) MOOSTrace("CN_add_to_cn_table: cn has maximum length. Pushing new line in\n");
			// copy entries from cn to cn_new while moving everything one line up
		    for(n=1;n<=num_rows-1;n++)
			{
				for(m=0;m<=num_cols-1;m++)
				{
					gsl_matrix_set(cn_new,n-1,m,gsl_matrix_get(*cn,n,m));
				}
			}
		}
		
		else
		{
			if(debug) MOOSTrace("CN_add_to_cn_table: Filling FIRST line of cn\n");
		}
					
		// copy new set of elements into the last row
		gsl_matrix_set(cn_new,num_rows-1,0,my_x);
		gsl_matrix_set(cn_new,num_rows-1,1,my_y);
		gsl_matrix_set(cn_new,num_rows-1,2,my_cov_xx);
		gsl_matrix_set(cn_new,num_rows-1,3,my_cov_xy);
		gsl_matrix_set(cn_new,num_rows-1,4,my_cov_yx);
		gsl_matrix_set(cn_new,num_rows-1,5,my_cov_yy);
		gsl_matrix_set(cn_new,num_rows-1,6,ov_x);
		gsl_matrix_set(cn_new,num_rows-1,7,ov_y);
		gsl_matrix_set(cn_new,num_rows-1,8,ov_cov_xx);
		gsl_matrix_set(cn_new,num_rows-1,9,ov_cov_xy);
		gsl_matrix_set(cn_new,num_rows-1,10,ov_cov_yx);
		gsl_matrix_set(cn_new,num_rows-1,11,ov_cov_yy);
		gsl_matrix_set(cn_new,num_rows-1,12,double(ov_id));
		gsl_matrix_set(cn_new,num_rows-1,12,range);
		
		// free old matrix cn and set pointer to new matrix
		gsl_matrix_free(*cn);
		*cn=cn_new;
	}
	
	// check if the number of rows is smaller than the maximum allowed length of the stack
	// ... OR if the matrix has only one line indicating that it has just been initialized
	if((num_rows<max_length) && (*cn_init))
	{
		if(debug) MOOSTrace("CN_add_to_cn_table: cn has not maximum length yet. Adding line at bottom\n");
		// if yes, create a new matrix which is one line longer
		gsl_matrix *cn_new = gsl_matrix_calloc(num_rows+1,num_cols);
		
		// copy entries from cn to cn_new
	    for(n=0;n<=num_rows-1;n++)
		{
			for(m=0;m<=num_cols-1;m++)
			{
				gsl_matrix_set(cn_new,n,m,gsl_matrix_get(*cn,n,m));
			}
		}
		gsl_matrix_set(cn_new,num_rows,0,my_x);
		gsl_matrix_set(cn_new,num_rows,1,my_y);
		gsl_matrix_set(cn_new,num_rows,2,my_cov_xx);
		gsl_matrix_set(cn_new,num_rows,3,my_cov_xy);
		gsl_matrix_set(cn_new,num_rows,4,my_cov_yx);
		gsl_matrix_set(cn_new,num_rows,5,my_cov_yy);
		gsl_matrix_set(cn_new,num_rows,6,ov_x);
		gsl_matrix_set(cn_new,num_rows,7,ov_y);
		gsl_matrix_set(cn_new,num_rows,8,ov_cov_xx);
		gsl_matrix_set(cn_new,num_rows,9,ov_cov_xy);
		gsl_matrix_set(cn_new,num_rows,10,ov_cov_yx);
		gsl_matrix_set(cn_new,num_rows,11,ov_cov_yy);
		gsl_matrix_set(cn_new,num_rows,12,double(ov_id));
		gsl_matrix_set(cn_new,num_rows,12,range);
		
		// free old matrix cn and set pointer to new matrix
		gsl_matrix_free(*cn);
		*cn=cn_new;
	}
	
	if(error_flag==0)
	{
		*cn_init=true;
	}
	
	if(debug) MOOSTrace("CN_add_to_cn_table: Leaving CN_add_to_cn_table\n");
	return error_flag;
}

// computes the likelihood (log probability) that a transition from point a to point b via a vector d took place
//
// INPUT ARGUMENTS:
//
//   a: 2x1 column vector (x,y)' of point a
//   b: 2x1 column vector (x,y)' of point b
//
//   d: distance vector (dx,dy)'
//
//   cov_a : 2x2 covariance matrix for a
//   cov_b : 2x2 covariance matrix for b
//   cov_d : 2x2 covariance matrix for d
//
// MODIFIED INPUT ARGUMENTS:
//
//   trans_l: transition-likelihood from point a to point b via d
//
// OUTPUT ARGUMENTS:
//
//   error_flag: 1 if any of the determinants of the matrices which are to be inverted is 0 
//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 7/8/2006        abahr       created
// 17/8/2006	   abahr       changed style of cn matrix col 3-5 and col 9-11

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CN.h"
using namespace std;

int CN_compute_transition_likelihood_func(gsl_vector *a,gsl_matrix *cov_a,gsl_vector *b,gsl_matrix *cov_b,gsl_vector *d,gsl_matrix *cov_d,double *trans_l)
{
	int error_flag=0;
	
	// allocate intermediate point and intermediate covariance as well as matrices for vectors
	gsl_matrix *p_ma = gsl_matrix_calloc(2,2);
	gsl_matrix *b_ma = gsl_matrix_calloc(2,2);
	gsl_matrix *d_ma = gsl_matrix_calloc(2,2);
	gsl_matrix *q_ma = gsl_matrix_calloc(2,2);
   	gsl_matrix *cov_p = gsl_matrix_calloc(2,2);
	
	// allocate inverse matrices
	gsl_matrix *cov_p_inv = gsl_matrix_calloc(2,2);
	gsl_matrix *cov_b_inv = gsl_matrix_calloc(2,2);
	
	// allocate LU decomposition, permutation vector, determinat, and lu-sign
	gsl_matrix *LU = gsl_matrix_calloc(2,2);
    gsl_permutation *permu = gsl_permutation_alloc(2);
    int lusign;
    double deter;
		
	// compute determinants to check if inverse exists
	gsl_matrix_memcpy(LU,cov_p);
    gsl_linalg_LU_decomp(LU,permu,&lusign);
    deter=gsl_linalg_LU_det(LU,lusign);
    if(deter==0) error_flag=1;
    
    gsl_matrix_memcpy(LU,cov_b);
    gsl_linalg_LU_decomp(LU,permu,&lusign);
    deter=gsl_linalg_LU_det(LU,lusign);
    if(deter==0) error_flag=1;
    
    if(error_flag==0)
    {
		// compute position and covariance of intermediate point
		// this is where one would get moving a mean(d)-distance from mean(a)
		
		// blow up vectors to matrices
		gsl_matrix_set_col(d_ma,0,d);
			
		// MATLAB code: p=a+d;
		gsl_matrix_set_col(p_ma,0,a);
		gsl_matrix_add(p_ma,d_ma);
		
		// MATLAB code: cov_p=cov_a+cov_d;
		gsl_matrix_memcpy(cov_p,cov_a);
		gsl_matrix_add(cov_p,cov_d);
		
		// compute inverse matrices
		gsl_matrix_memcpy(LU,cov_p);
	    
	    gsl_linalg_LU_decomp(LU,permu,&lusign);
	    gsl_linalg_LU_invert(LU,permu,cov_p_inv);
	    
	    gsl_matrix_memcpy(LU,cov_b);
	    
	    gsl_linalg_LU_decomp(LU,permu,&lusign);
	    gsl_linalg_LU_invert(LU,permu,cov_b_inv);
	       	
		// compute point q which is the statistically closest point between p and b
		
		// MATLAB code q=(((cov_p^-1)+(cov_b^-1))^-1)*(((cov_p)^-1)*p+((cov_b)^-1)*b);
		
		// allocate variables for intermediate steps
	    gsl_matrix *cov_p_inv_cov_b_inv_sum_inv = gsl_matrix_alloc(2,2);
	    gsl_matrix *cov_p_inv_p_ma_cov_b_inv_b_ma_sum = gsl_matrix_alloc(2,2);
	    gsl_matrix *cov_p_inv_p_ma = gsl_matrix_alloc(2,2);
	    gsl_matrix *cov_b_inv_b_ma = gsl_matrix_alloc(2,2);    
	    
		gsl_matrix_memcpy(cov_p_inv_cov_b_inv_sum_inv,cov_p_inv);
		gsl_matrix_add(cov_p_inv_cov_b_inv_sum_inv,cov_b_inv);
		
		// compute determinants to check if inverse exists
   		gsl_matrix_memcpy(LU,cov_p_inv_cov_b_inv_sum_inv);
	    gsl_linalg_LU_decomp(LU,permu,&lusign);
    	deter=gsl_linalg_LU_det(LU,lusign);
	    
	    if(deter==0) error_flag=1;    
            
	    if(error_flag==0)
	    {
			gsl_matrix_memcpy(LU,cov_p_inv_cov_b_inv_sum_inv);
		    
		    gsl_linalg_LU_decomp(LU,permu,&lusign);
		    gsl_linalg_LU_invert(LU,permu,cov_p_inv_cov_b_inv_sum_inv);
		    
		    gsl_matrix_memcpy(cov_p_inv_p_ma,cov_p_inv);
		    gsl_matrix_mul_elements(cov_p_inv_p_ma,p_ma);
		    
		    gsl_matrix_memcpy(cov_b_inv_b_ma,cov_b_inv);
		    gsl_matrix_mul_elements(cov_b_inv_b_ma,b_ma);
			
			gsl_matrix_memcpy(cov_p_inv_p_ma_cov_b_inv_b_ma_sum,cov_p_inv_p_ma);
			gsl_matrix_add(cov_p_inv_p_ma_cov_b_inv_b_ma_sum,cov_b_inv_b_ma);
			
			gsl_matrix_memcpy(q_ma,cov_p_inv_cov_b_inv_sum_inv);
			gsl_matrix_mul_elements(q_ma,cov_p_inv_p_ma_cov_b_inv_b_ma_sum);
				
			// MATLAB code: trans_l=(p-q)'*(cov_p^-1)*(p-q)+(b-q)'*(cov_b^-1)*(b-q);
			
			// allocate variables for intermediate steps
		    gsl_matrix *p_ma_q_ma_diff = gsl_matrix_alloc(2,2);
		    gsl_matrix *p_ma_q_ma_diff_t = gsl_matrix_alloc(2,2);
		    gsl_matrix *b_ma_q_ma_diff = gsl_matrix_alloc(2,2);
		    gsl_matrix *b_ma_q_ma_diff_t = gsl_matrix_alloc(2,2);
		    gsl_matrix *summand_1 = gsl_matrix_alloc(2,2);
		    gsl_matrix *summand_2 = gsl_matrix_alloc(2,2);
		    gsl_matrix *trans_l_ma = gsl_matrix_alloc(2,2);
			        	
			gsl_matrix_memcpy(p_ma_q_ma_diff,p_ma);
			gsl_matrix_sub(p_ma_q_ma_diff,q_ma);
			gsl_matrix_transpose_memcpy(p_ma_q_ma_diff_t,p_ma_q_ma_diff);
			
			gsl_matrix_memcpy(b_ma_q_ma_diff,b_ma);
			gsl_matrix_sub(b_ma_q_ma_diff,q_ma);
			gsl_matrix_transpose_memcpy(b_ma_q_ma_diff_t,b_ma_q_ma_diff);
			
			gsl_matrix_memcpy(summand_1,p_ma_q_ma_diff_t);
			gsl_matrix_mul_elements(summand_1,cov_p_inv);
			gsl_matrix_mul_elements(summand_1,p_ma_q_ma_diff);
			
			gsl_matrix_memcpy(summand_2,b_ma_q_ma_diff_t);
			gsl_matrix_mul_elements(summand_2,cov_b_inv);
			gsl_matrix_mul_elements(summand_2,b_ma_q_ma_diff);
			
			gsl_matrix_memcpy(trans_l_ma,summand_1);
			gsl_matrix_add(trans_l_ma,summand_2);
			
			// reduce trans_l_ma matrix to a single value
			*trans_l=gsl_matrix_get(trans_l_ma,0,0);
			
			gsl_matrix_free(p_ma_q_ma_diff);
			gsl_matrix_free(p_ma_q_ma_diff_t);
			gsl_matrix_free(b_ma_q_ma_diff);
			gsl_matrix_free(b_ma_q_ma_diff_t);
			
			gsl_matrix_free(summand_1);
			gsl_matrix_free(summand_2);
			gsl_matrix_free(trans_l_ma);
	    }
		gsl_matrix_free(cov_p_inv_cov_b_inv_sum_inv);
		gsl_matrix_free(cov_p_inv_p_ma_cov_b_inv_b_ma_sum);
		gsl_matrix_free(cov_p_inv_p_ma);
		gsl_matrix_free(cov_b_inv_b_ma);
    }
    else 
    {
    	error_flag=1;
    }
    
    gsl_matrix_free(p_ma);
	gsl_matrix_free(b_ma);
	gsl_matrix_free(d_ma);
	gsl_matrix_free(q_ma);
	gsl_matrix_free(cov_p);
	
	gsl_matrix_free(cov_p_inv);
	gsl_matrix_free(cov_b_inv);
	
	gsl_matrix_free(LU);
	gsl_permutation_free(permu);	

    return error_flag;
}

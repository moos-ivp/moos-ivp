// auxilary function to call CN_interset_func by just specifying two rows (r1 and r2) in the matrix cn
//
// INPUT ARGUMENTS:
//   
// r1,r2: row indeces
//
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
// MODIFIED INPUT ARGUMENTS:
//
//   sol: 		2x2 solution matrix with 1 if the solution is valid and 0 if the solution is not valid
//   xy_sol:  	x and y-values of both solutions [x_1 y_1
//                                                x_2 y_2]
//   sigma_xy: 	a 2 element array of 2x2 matrices for the position covariances
//
// RETURN ARGUMENT:
//
//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 15/08/2006       abahr       created
// 17/8/2006	   	abahr       changed style of cn matrix col 3-5 and col 9-11

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CN.h"
using namespace std;


int CN_intersect_call_func(gsl_matrix *cn,int r1,int r2, double cov_range, double cov_odometry, gsl_matrix *sol,gsl_matrix *xy_sol,gsl_matrix *sigma_xy[2], bool debug)
{
	int dummy;
	int num_of_solutions;
	
	double x11,y11,x22,y22,dx,dy,r11,r22;
	
	gsl_matrix *G = gsl_matrix_calloc(8,8);
	
	if(r2<r1)
	{
	    dummy=r2;
	    r2=r1;
	    r1=dummy;
	}
	
	// extract elements for computation of intersection points
	// MATLAB code: x11=cna(r1,1);
	// MATLAB code: y11=cna(r1,2);
	
	x11=gsl_matrix_get(cn,r1,0);
	y11=gsl_matrix_get(cn,r1,1);
	
	// MATLAB code: x22=cna(r2,1);
	// MATLAB code: y22=cna(r2,2);
	
	x22=gsl_matrix_get(cn,r2,6);
	y22=gsl_matrix_get(cn,r2,7);
	
	// MATLAB code: dx=dist(r1,r2,1);
	// MATLAB code: dy=dist(r1,r2,2);
	
	dx=gsl_matrix_get(cn,r2,0)-gsl_matrix_get(cn,r1,0);
    dy=gsl_matrix_get(cn,r2,1)-gsl_matrix_get(cn,r1,1);
	
	// MATLAB code: r11=range(r1);
	// MATLAB code:	r22=range(r2);
	
	r11=gsl_matrix_get(cn,r1,13);
	r22=gsl_matrix_get(cn,r2,13);
	
	// MATLAB code: [sol,x,y]=CN_intersect_func(x11,y11,x22,y22,dx,dy,r11,r22);
	if(debug) 
	{
		MOOSTrace("CN_intersect_call_func: Intersect for: x11=%5.3lf y11=%5.3lf x22=%5.3lf y22=%5.3lf\n",x11,y11,x22,y22);
		MOOSTrace("                                       dx=%5.3lf dy=%5.3lf r11=%5.3lf r22=%5.3lf \n",dx,dy,r11,r22);
	}
	
	num_of_solutions=CN_intersect_func(x11, y11, x22, y22, dx, dy, r11, r22, sol, xy_sol, debug);	
	
	if(num_of_solutions!=0)
	{
	    // for debugging purposes plot complex solutions
	    // if(~isempty(find(imag(x)~=0)) || ~isempty(find(imag(y)~=0)))
	    //     CN_plot_intersect_func(x11,y11,x22,y22,dx,dy,r11,r22);
	    // end
	
	    // extract elements for computation of the intersection points' covariance
	
	
	    // covariance of CNA1's position
	    
	    // MATLAB code: G(1,1)=cn(r1,9);
	    // MATLAB code: G(1,2)=cn(r1,11);
    	// MATLAB code: G(2,1)=cn(r1,12);
	    // MATLAB code: G(2,2)=cn(r1,10);
	    // !!! changed structure of cn table from MATLAB code !!!
	    
	    gsl_matrix_set(G,0,0,gsl_matrix_get(cn,r1,8));
	    gsl_matrix_set(G,0,1,gsl_matrix_get(cn,r1,9));
	    gsl_matrix_set(G,1,0,gsl_matrix_get(cn,r1,10));
	    gsl_matrix_set(G,1,1,gsl_matrix_get(cn,r1,11));
	    
	    // covariance of CNA2's position
	    // MATLAB code: G(3,3)=cn(r2,9);
	    // MATLAB code: G(3,4)=cn(r2,11);
	    // MATLAB code: G(4,3)=cn(r2,12);
	    // MATLAB code: G(4,4)=cn(r2,10);
	    // !!! changed structure of cn table from MATLAB code !!!
	
		gsl_matrix_set(G,2,2,gsl_matrix_get(cn,r2,8));
	    gsl_matrix_set(G,2,3,gsl_matrix_get(cn,r2,9));
	    gsl_matrix_set(G,3,2,gsl_matrix_get(cn,r2,10));
	    gsl_matrix_set(G,3,3,gsl_matrix_get(cn,r2,11));
	
	    // set covariances for range measurements
	    // MATLAB code: G(5,5)=cov_range;
	    // MATLAB code: G(6,6)=cov_range;
	    
	    gsl_matrix_set(G,4,4,cov_range);
	    gsl_matrix_set(G,5,5,cov_range);
	
	    // set covariances for odometry measurements
	    // MATLAB code: G(7,7)=cov_odometry(1,1)*dx;
	    // MATLAB code: G(8,8)=cov_odometry(2,2)*dy;
	    
	    // assume single variance value instead of covariance matrix for C code
	    
	    gsl_matrix_set(G,6,6,cov_odometry*dx);
	    gsl_matrix_set(G,7,7,cov_odometry*dy);
	
		// MATLAB code: sigma_xy=CN_compute_covariances_func(x11,y11,x22,y22,r11,r22,dx,dy,G,sol);
		
		CN_compute_covariances_func(x11, y11, x22, y22, r11, r22, dx, dy, G,sol,sigma_xy);
	}
		
	else
	{
	    // MATLAB code: sigma_xy{1}=[NaN NaN;NaN NaN];
	    // MATLAB code: sigma_xy{2}=[NaN NaN;NaN NaN];
	    
	    gsl_matrix_set_all(sigma_xy[0],-1);
	    gsl_matrix_set_all(sigma_xy[1],-1);	    
	}

	gsl_matrix_free(G);
	
	return num_of_solutions;
}


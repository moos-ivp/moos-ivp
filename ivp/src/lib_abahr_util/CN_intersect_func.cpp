// compute solutions for range circle intersection for a given set
//
// INPUT ARGUMENTS:
//
//   x11,y11: position of CNA at time of first range measurement (t1)
//   r11: range between AUV and CNA at x11,y11 at time t1
//
//   x22,y22: position of CNA at time of second range measurement (t2) (this CNA is not necessarily the same as the one from the
//          first range measurement
//   r22: range between AUV and CNA at x22,y22 at time t2
//
//   dx,dy: relative movement of AUV between t1 and t2
//
// MODIFIED INPUT ARGUMENTS:
//
//   sol: 2x2 solution matrix with 1 if the solution is valid and 0 if the solution is not valid
//   xy_sol:  x and y-values of both solutions [ x_1 y_1
//                                               x_2 y_2]
//
// RETURN ARGUMENT:
//
//   
//   
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 3/8/2006        abahr       created

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CN.h"
using namespace std;


int CN_intersect_func(double x11, double y11, double x22, double y22, double dx, double dy, double r11, double r22, gsl_matrix *sol, gsl_matrix *xy_sol, bool debug)
{
	
	int j=0;
	int n;
	int m;
	
	double eps=0.001;
	complex<double> check_1,check_2;
	complex<double> x[2],y[2];
	
	if(debug) MOOSTrace("CN_intersect_func: Solving intersection equations\n");
	
	x[0]=1/2*(dx+x11+x22-(((r11-r22)*(r11+r22)*(dx+x11-x22))/(pow((dx+x11-x22),2)+pow((dy+y11-y22),2)))+(sqrt(-(pow((dx+x11-x22),2)+(dy+r11-r22+y11-y22)*(dy-r11+r22+y11-y22))*(pow((dx+x11-x22),2)+(dy-r11-r22+y11-y22)*(dy+r11+r22+y11-y22))*pow((dy+y11-y22),2)))/(pow((dx+x11-x22),2)+pow((dy+y11-y22),2)));
	x[1]=1/2*(dx+x11+x22-(((r11-r22)*(r11+r22)*(dx+x11-x22))/(pow((dx+x11-x22),2)+pow((dy+y11-y22),2)))-(sqrt(-(pow((dx+x11-x22),2)+(dy+r11-r22+y11-y22)*(dy-r11+r22+y11-y22))*(pow((dx+x11-x22),2)+(dy-r11-r22+y11-y22)*(dy+r11+r22+y11-y22))*pow((dy+y11-y22),2)))/(pow((dx+x11-x22),2)+pow((dy+y11-y22),2)));
	y[0]=(1/(2*(pow((dx+x11-x22),2)+pow((dy+y11-y22),2))))*((dy+y11)*(-pow(r11,2)+pow(r22,2)+pow((dx+x11-x22),2)+pow((dy+y11),2))+sqrt(-pow((dx+x11-x22),2)*(pow((dx+x11-x22),2)+(dy+r11-r22+y11-y22)*(dy-r11+r22+y11-y22))*(pow((dx+x11-x22),2)+(dy-r11-r22+y11-y22)*(dy+r11+r22+y11-y22)))+(pow(r11,2)-pow(r22,2)+pow((dx+x11-x22),2)-pow((dy+y11),2))*y22-(dy+y11)*pow(y22,2)+pow(y22,3));
	y[1]=(1/(2*(pow((dx+x11-x22),2)+pow((dy+y11-y22),2))))*((dy+y11)*(-pow(r11,2)+pow(r22,2)+pow((dx+x11-x22),2)+pow((dy+y11),2))-sqrt(-pow((dx+x11-x22),2)*(pow((dx+x11-x22),2)+(dy+r11-r22+y11-y22)*(dy-r11+r22+y11-y22))*(pow((dx+x11-x22),2)+(dy-r11-r22+y11-y22)*(dy+r11+r22+y11-y22)))+(pow(r11,2)-pow(r22,2)+pow((dx+x11-x22),2)-pow((dy+y11),2))*y22-(dy+y11)*pow(y22,2)+pow(y22,3));

	// feed values back into equation
	
	// reset matrix which indicates valid solutions
	gsl_matrix_set_zero(sol);

	if(debug) MOOSTrace("CN_intersect_func: Checking solutions\n");
	

	for(n=0;n<=1;n++)
	{
	    for(m=0;m<=1;m++)
		{
	        check_1=pow((x[n]-(x11+dx)),2)+pow((y[m]-(y11+dy)),2)-pow(r11,2);
	        check_2=pow((x[n]-x22),2)+pow((y[m]-y22),2)-pow(r22,2);
	        if((abs(check_1)<eps) && (abs(check_2)<eps))
			{
	            if((imag(x[n])==0) && (imag(y[m])==0))
				{
	                j=j+1;
	                gsl_matrix_set(xy_sol,j,1,real(x[n]));
	                gsl_matrix_set(xy_sol,j,2,real(y[m]));
	                gsl_matrix_set(sol,n,m,1);
	            }
	        }
	    }
	}
	
	if(debug) MOOSTrace("CN_intersect_func: Finished intersection\n");
	
	// return number of valid solutions
	return j;
}

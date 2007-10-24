#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <values.h>

#include "1BTrack.h"

using namespace std;

C1BTrack::C1BTrack()
{
  
  nvar = NUM_P;
  
  search_times = 10000;
  search_eps = 1.0e-15;
  mu1 = 0.0002;
  mu2 = 0.9;
  
  //allocate memory for our measurements
  vposx    = new double[MAX_BEARINGS];
  vposy    = new double[MAX_BEARINGS];
  bearings = new double[MAX_BEARINGS];
  dt       = new double[MAX_BEARINGS];

  meas_num = 0;
  solution = false;
  zindex = 0;
  reset_init = 0;
}

C1BTrack::~C1BTrack()
{
  //give back memory
  free(vposx);
  free(vposy);
  free(bearings);
  free(dt);
}

void C1BTrack::reset()
{
  meas_num=0;
  solution = false;
  zindex = 0;
  cout << "1BTrack reset()\n"<<endl;
}

//void C1BTrack::set_range(double rg)
//{
//  range_guess = rg;
//}

//how many observations do we use?
void C1BTrack::set_Z_length(int len)
{
  zlength = len;
}

void C1BTrack::set_guess(double xdot,double ydot,double tx,double ty)
{
  xguess = tx;
  yguess = ty;
  xdotguess = xdot;
  ydotguess = ydot;
  cout << "guess = "<< xguess<<","<<yguess<<","<<xdot<<","<<ydot<<endl;
}

void C1BTrack::set_measurement(double veh_x, double veh_y, double bear,double diff)
{
  if (zindex >= zlength)
    zindex = 0;

    vposx[zindex] = veh_x;
    vposy[zindex] = veh_y;
    bearings [zindex] = bear;
    dt[zindex] = diff;
    zindex++;
    meas_num++;

    if(meas_num >= zlength)
      meas_num = zlength;
    //cout<<"zindex = "<<zindex<<" vpos_x = "<<veh_x<<" vposy = "<<veh_y<<" bear = "<<bear<<" diff "<<diff<<endl;
    //cout<<"meas_num = "<<meas_num<<endl;
}

status C1BTrack::get_track()
{
  bool silent = true;
  
  double x[nvar];	
  //double range_guess;

  status stat;

  x[0] = xguess;
  x[1] = yguess;
  x[2] = xdotguess;
  x[3] = ydotguess;


  //range_guess = 250.0;

  /*  if(!solution){
    //initial guess
    x[0] = vposx[zindex-1] + range_guess*cos(bearings[zindex-1]);
    x[1] = vposy[zindex-1] + range_guess*sin(bearings[zindex-1]);
    x[2] = 1.0;
    x[3] = -1.0;
  }
  else
    {
      if (reset_init == 0){
    x[0] = vposx[zindex-1] + range_guess*cos(bearings[zindex-1]);
    x[1] = vposy[zindex-1] + range_guess*sin(bearings[zindex-1]);
    x[2] = xdot;
    x[3] = ydot;
     
      }
      else{
    x[0] = vposx[zindex-1] + range_guess*cos(bearings[zindex-1]);
    x[1] = vposy[zindex-1] + range_guess*sin(bearings[zindex-1]);
    x[2] = 1.0;
    x[3] = 1.0;
    cout << "reset init guess" << endl;
      }
      }*/

  if(!silent)
    cout << "xguess = " << x[0] << " yguess = " << x[1] << endl;
  
  stat = QuasiNewtonMethod(nvar, x, &fopt, search_times, search_eps);
   
  if(stat == success){
    xzero = x[0];
    yzero = x[1];
    xdot  = x[2];
    ydot  = x[3];
    
    solution = true;
  }
  return stat;
}

dbl C1BTrack::track_func(int n, double x[])
{
 
  int i;
  double sum,time_step,x0,x1,x2,x3,xpot,ypot,bear_pot,dtsum;

  sum   = 0.0; //time_step = 1.0;
  dtsum = 0.0;

  x0 = x[0]; //target initial X
  x1 = x[1]; //target initial Y
  x2 = x[2]; //target x dot
  x3 = x[3]; //target y dot

  //calculate negative log-likelihood function
  for (i=0; i < meas_num; i++)
  {
    dtsum = dt[i];
    //calculate what bearing measurement should be given x0,x1,x2,x3     
    //assuming constant velocity
    //xpot = x0 + ((time_step*i)*x2);
    xpot = x0 + (dtsum*x2);
    ypot = x1 + (dtsum*x3);    
    bear_pot = atan2((ypot-vposy[i]),(xpot-vposx[i]));
    
    sum = sum + pow((bearings[i] - bear_pot),2);
  }
 
  return (sum);
}

double *C1BTrack::grad_track(int n,double x[],double grad[])
{

  double x0,x1,x2,x3,g0sum,g1sum,g2sum,g3sum,hx0,hx1,hx2,hx3;
  double xpot,ypot,time_step,bear_pot,diff,dtsum;
  int i;

  x0 = x[0]; //target initial X
  x1 = x[1]; //target initial Y
  x2 = x[2]; //target x dot
  x3 = x[3]; //target y dot
  
  //time_step = 1.0;
  g0sum = 0.0;  g1sum = 0.0; g2sum = 0.0; g3sum = 0.0;

  dtsum = 0.0;

  //calculate the gradient of the negative log-likelihood function
  for (i=0; i < meas_num; i++)
  {
    dtsum = dt[i];

    //calculate what bearing measurement should be given x0,x1,x2,x3     
    //xpot = x0 + ((time_step*i)*x2);
    xpot = x0 + (dtsum*x2);
    ypot = x1 + (dtsum*x3);
    bear_pot = atan2((ypot-vposy[i]),(xpot-vposx[i]));
    //calculate gradient
    hx0 = (-(ypot-vposy[i])/ (pow((ypot-vposy[i]),2) + pow((xpot-vposx[i]),2)));
    hx1 = ( (xpot-vposx[i])/ (pow((ypot-vposy[i]),2) + pow((xpot-vposx[i]),2)));
    hx2 = dtsum*hx0;
    hx3 = dtsum*hx1;

    diff = 2*(bearings[i] - bear_pot);

    g0sum = g0sum + (diff*(-1.0*hx0));
    g1sum = g1sum + (diff*(-1.0*hx1));
    g2sum = g2sum + (diff*(-1.0*hx2));
    g3sum = g3sum + (diff*(-1.0*hx3));

}

  grad[0] = g0sum;   
  grad[1] = g1sum;
  grad[2] = g2sum;
  grad[3] = g3sum;
  
  return grad;
}

void C1BTrack::initialize()
{
  
  xtmp    = new dbl[nvar];
  gtmp    = new dbl[nvar];	
  h       = new dbl[nvar*nvar];
  dl      = new dbl[nvar];
  gm      = new dbl[nvar];
  dg      = new dbl[nvar];
  gd      = new dbl[nvar];
  matq    = new dbl[nvar*nvar];
  hnew    = new dbl[nvar*nvar];
  xnew    = new dbl[nvar];
  grad    = new dbl[nvar];
  gradnew = new dbl[nvar];
  d       = new dbl[nvar];
}

void C1BTrack::update(dbl *h, dbl *dl, dbl *gm)
{
	int	i, j, k, l;
	dbl	sum;
	dbl	a, p, q;
	
	if ((sum = vectorvector(nvar, dl, gm)) == 0.00) {
		return;
	}
	
	for (i=0; i<nvar; i++) {
		for (j=0; j<nvar; j++) {
			matq[i*nvar+j] = - dl[i]*gm[j]/sum;
		}
		matq[i*nvar+i] += 1.00;
	}
	
	for (i=0; i<nvar; i++) {
		for (j=0; j<nvar; j++) {
			a = 0;
			for (k=0; k<nvar; k++) {
				p = matq[i*nvar+k];
				for (l=0; l<nvar; l++) {
					a += p*h[k*nvar+l]*matq[j*nvar+l];
				}
			}
			hnew[i*nvar+j] = a;
		}
	}
	
	for (i=0; i<nvar; i++) {
		for (j=0; j<nvar; j++) {
			h[i*nvar+j] = hnew[i*nvar+j] + dl[i]*dl[j]/sum;
		}
	}
}

dbl C1BTrack::theta_1(dbl *xinit, dbl *d, dbl finit, dbl p1, dbl ro)
{
	int	i;
	
	for (i=0; i<nvar; i++) xtmp[i] = xinit[i] + ro*d[i];
	return ((*this.*objective)(nvar, xtmp) - finit - p1*ro);
}

dbl C1BTrack::theta_2(dbl *xinit, dbl *d, dbl p2, dbl ro)
{
	int	i;
	
	for (i=0; i<nvar; i++) xtmp[i] = xinit[i] + ro*d[i];
	(*this.*gradient)(nvar, xtmp, gtmp);
	return (vectorvector(nvar, gtmp, d) - p2);
}


dbl C1BTrack::Wolfe_line_search(dbl *xinit, dbl *d)
{
	int	k;
	dbl	t1, t2, ro, dro, a, b;
	dbl	finit, p1, p2;
	
	ro = 1;
	dro = 1;
	
	finit = (*this.*objective)(nvar, xinit);
	(*this.*gradient)(nvar, xinit, gtmp);
	p1 = mu1 * vectorvector(nvar, gtmp, d);
	p2 = mu2 * vectorvector(nvar, gtmp, d);
	
	for (k=0; k<search_times; k++) {
		t1 = theta_1(xinit, d, finit, p1, ro);
		t2 = theta_2(xinit, d, p2, ro);
#if Debug > 1
		if (k % 1 == 0) {
			fprintf(stderr, "%d: t1=%lg t2=%lg\n", k, t1, t2);
		}
#endif
		if (t1 <= 0) {
			if (t2 >= 0) {
				return(ro);
			} else {
				ro += dro;
			}
		} else {
			a = ro - dro;
			b = ro;
			break;
		}
	}
	if (k == search_times) {
		return(0.00);
	}
	
	for (;;) {
#if Debug > 1
		fprintf(stderr, "a=%lg b=%lg\n", a, b);
#endif
		ro = (a + b) / 2.00;
		if (b - a < search_eps) {
			return(ro);
		}
		t1 = theta_1(xinit, d, finit, p1, ro);
		t2 = theta_2(xinit, d, p2, ro);
#if Debug > 1
		fprintf(stderr, "t1=%lg t2=%lg\n", t1, t2);
#endif
		if (t1 <=  0 && t2 >= 0) {
			return(ro);
		}
		if (t1 > 0) {
			b = ro;
		} else {
			a = ro;
		}
	}
}



void C1BTrack::terminate()
{
	free(xtmp);
	free(gtmp);
	free(h);
	free(dl);
	free(gm);
	free(dg);
	free(gd);
	free(matq);
	free(hnew);	
	free(xnew);
	free(grad);
	free(gradnew);
	free(d);
}

status C1BTrack::QuasiNewtonMethod(int n, dbl *xinit, dbl *fopt, int timeout, dbl eps)
{
	int	k;
	dbl	sum, al;
	status	stat;
	static dbl	finit, fnew;
	
	nvar = n;
	objective = &C1BTrack::track_func;
	gradient  = &C1BTrack::grad_track;
	
	initialize();
	
	matrixunit(nvar, h);
	finit = MAXFLOAT;
	
	stat = failure;
	
	for (k=0; k<timeout; k++) {
		fnew = finit;
		finit = (*this.*objective)(nvar, xinit);
		if (finit <= -MAXFLOAT || finit >= MAXFLOAT) {
			fprintf(stderr, "overflow\n");
			stat = failure;
			break;
			/* return(failure); */
		}
#if Debug
		if (k % SKIPTIME == 0) {
			fprintf(stderr, "k = %d   f = %lg\n", k, finit);
			/* vectorfprint(stderr, nvar, xinit); */
		}
#endif
		if (fnew - finit < eps) {
			stat = success;
			break;
			/* *fopt = finit; */
			/* return(success); */
		}
		
		(*this.*gradient)(nvar, xinit, grad);
		
		matrixvector(nvar, nvar, d, h, grad);
		scalarvector(nvar, d, -1.00, d);
		
		sum = vectorvector(nvar, grad, d);
		if (sum >= 0) {
			matrixunit(nvar, h);
			matrixvector(nvar, nvar, d, h, grad);
			scalarvector(nvar, d, -1.00, d);
		}
		
		al = Wolfe_line_search(xinit, d);
		if (al == 0.00) {
			fprintf(stderr, "bfgs.c: alpha = %lf\n", al);
			break;
		}
		
		scalarvector(nvar, dl, al, d);
		vectoradd(nvar, xnew, xinit, dl);
		
		(*this.*gradient)(nvar, xnew, gradnew);
		vectorsub(nvar, gm, gradnew, grad);
		
		update(h, dl, gm);
		
		vectorcopy(nvar, xinit, xnew);
	}
	
	if (k >= timeout) fprintf(stderr, "timeout\n");
	
	/* fprintf(stderr, "***** end *****\n"); */
	*fopt = finit;
	terminate();
	return (stat);
	
	/* *fopt = finit; */
	/* return(failure); */
}




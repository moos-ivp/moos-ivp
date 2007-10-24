#include "opt.h"

#define NUM_P 4
#define MAX_BEARINGS 300 

class C1BTrack
{
 public:

  C1BTrack();  
  virtual ~C1BTrack();

  //get estimated track parameters from bearings
  //send it an initial starting guess
  status get_track();

  //store a measurement(vehicle x,y and bearing to target)
  void set_measurement(double,double,double,double);
  //void set_range(double range_guess);
  void set_Z_length(int);
  void reset();
  void set_guess(double,double,double,double);

  //results from the optimization
  double xzero,yzero,xdot,ydot,xguess,yguess,xdotguess,ydotguess;
  double fopt,range_guess;
  int reset_init;
  protected:

  //measurements
  dbl *vposx;
  dbl *vposy;
  dbl *bearings;
  dbl *dt;

  bool solution;

  //measurement index
  int meas_num;

  //track function to minimize
  //the negative log-likelihood function in this case
  dbl track_func(int, dbl *); 

  //computes the gradient of the track function
  double *grad_track(int n,double x[],double grad[]);

  //the number of parameters we're solving for
  int nvar;
  
  int zindex;

  //the number of observations to use
  int zlength;
 
  //******************* BFGS optimization **********************************
  status QuasiNewtonMethod(int n, dbl *xinit, dbl *fopt, int timeout, dbl eps);
  void initialize();
  void terminate();
  void update(dbl *h, dbl *dl, dbl *gm);
  dbl Wolfe_line_search(dbl *xinit, dbl *d);
  dbl theta_1(dbl *xinit, dbl *d, dbl finit, dbl p1, dbl ro);
  dbl theta_2(dbl *xinit, dbl *d, dbl p2, dbl ro);

  //parameters for the BFGS algorithm
  int search_times;
  double search_eps;
  dbl mu1,mu2;

  //a pointer to the function to be minimized
  dbl	(C1BTrack::*objective)(int, dbl *);
  //a pointer to the gradient function
  dbl	*(C1BTrack::*gradient)(int, dbl *, dbl *);
  dbl	*xtmp;
  dbl	*gtmp;
  dbl	*h, *dl, *gm;
  dbl	*dg, *gd;
  dbl	*matq;
  dbl	*hnew;
  dbl	*xnew, *grad, *gradnew, *d;
//**************************************************************
};

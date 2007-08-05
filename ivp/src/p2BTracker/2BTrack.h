#ifndef TRACK_HEADER
#define TRACK_HEADER


#include "opt.h"

#define M_NOISE 0.01
#define S_NOISE 0.001
#define MAX_BEARINGS 1000

class C2BTrack
{
 public:

  C2BTrack();  
  virtual ~C2BTrack();

  //store a set of measurements
  void set_measurement(double veh1_x, double veh1_y, double bear1,double veh2_x, double veh2_y, double bear2,double tint, double msigma1,double msigma2);
  //initialize the EKF
  void init_filter(double x0, double x1, double x2, double x3);
  //filter the measurements
  void Kalman_Loop();
  //reset the tracker
  void reset();
  //return covariance matrix entries
  void estimate_covariance(double*,double*,double*,double*);


  //filtered solution
  double xtpos,ytpos,xtdot,ytdot;

  //unfiltered solution
  dbl xuf;
  dbl yuf;

 protected:

  //Kalman filter matrices
  dbl *F;
  dbl *R;
  dbl *Q;
  dbl *P;
  dbl *X;
  dbl *H;
  dbl *K;
  dbl *HH;
  dbl *temp1; dbl *temp2; dbl *temp3; dbl *temp8; dbl *temp9; dbl *temp10;
  dbl *temp4; dbl *temp5; dbl *temp6; dbl *temp7; dbl *temp11;
  dbl *z;

  dbl dt;

  //measurements
  dbl v1posx;
  dbl v1posy;
  dbl v2posx;
  dbl v2posy;
  dbl bearings1;
  dbl bearings2;

  //measurement index
  //int meas_num;

};
#endif

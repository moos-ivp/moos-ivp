/******************************************************************************
 *                                                                            *
 *  Module: 2BTrack                                                           *
 *  Author: Don Eickstedt, MIT/NUWC (code), Jack Ianneillo, SAIC (algorithm)  * 
 *  Date:   21 Feb, 2007                                                      *
 *                                                                            *
 *  Description: This module implements an extended Kalman filter (EKF) for   *
 *  estimating the position and velocity of a moving target given two (near)  *
 *  simultaneous bearing measurements.                                        * 
 *                                                                            * 
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include <values.h>

#include "2BTrack.h"

/******************************************************************************
 *                                                                            *
 *  Method: C2BTrack::C2BTrack()                                              *
 *                                                                            *
 *  Description: The constructor for the C2BTrack class                       * 
 *                                                                            * 
 ******************************************************************************/
C2BTrack::C2BTrack()
{
  
  //initialize F (state transition matrix)
  F = new dbl[16];
  vectorfill(16,F,0.0);

  //initialize R (measurement error matrix) with default noise value
  R = new dbl[4];
  vectorfill(4,R,0.0);
  R[0] = (M_NOISE*M_NOISE); R[3] = (M_NOISE*M_NOISE) ;
  
  //initialize X (state vector)
  X = new dbl[4];
  vectorfill(4,X,0.0);

  //initialize P (initial state covariance matrix)
  P = new dbl[16];
  vectorfill(16,P,0.0);
  P[0] = 10.0; P[5] = 20000.0; P[10] = 10.0; P[15] = 20000.0;

  //initialize Q
  Q = new dbl[16];
  vectorfill(16,Q,0.0);
  
  
  //initialize H ((linearized) measurement matrix)
  H = new dbl[8];
  vectorfill(8,H,0.0);

  //initialize K (Kalman gain matrix)
  K = new dbl[8];
  vectorfill(8,K,0.0);
  
  //initialize observation vector
  z = new dbl[2];
  vectorfill(2,z,0.0);

  //initialize HH
  HH = new dbl[2];  vectorfill(2,HH,0.0);

  //initialize temporary EKF matrices
  temp1 = new dbl[8]; temp2 = new dbl[8]; temp3 = new dbl[4]; temp4 = new dbl[4];
  temp5 = new dbl[4]; temp6 = new dbl[8]; temp7 = new dbl[2]; temp8 = new dbl[16];
  temp9 = new dbl[16]; temp10 = new dbl[16]; temp11 = new dbl[16];
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTrack::~C2BTrack()                                             *
 *                                                                            *
 *  Description: The destructor for the C2BTrack class                        * 
 *                                                                            * 
 ******************************************************************************/
C2BTrack::~C2BTrack()
{
  //free dynamically allocated memory
  free(F);
  free(R);
  free(X);
  free(P);
  free(Q);
  free(H);
  free(K);
  free(HH);  
  free(temp1); free(temp2); free(temp3); free(temp8); free(temp9); free(temp10);
  free(temp4); free(temp5); free(temp6); free(temp7); free(temp11);
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTrack::reset()                                                 *
 *                                                                            *
 *  Description: Reset the EKF to starting values                             * 
 *                                                                            * 
 ******************************************************************************/
void::C2BTrack::reset()
{

 //zero important matrices and set initial covariance matrix values
 vectorfill(16,F,0.0);
 vectorfill(4,R,0.0);
  R[0] = (M_NOISE*M_NOISE); R[3] = (M_NOISE*M_NOISE) ;
 vectorfill(4,X,0.0);
 vectorfill(16,P,0.0);
  P[0] = 10.0; P[5] = 20000.0; P[10] = 10.0; P[15] = 20000.0;
 vectorfill(16,Q,0.0);
 vectorfill(8,H,0.0);
 vectorfill(8,K,0.0);
 vectorfill(2,z,0.0);
 vectorfill(2,HH,0.0);
}

/***************************************************************************************
 *                                                                                     *
 *  Method: C2BTrack::set_measurement(double,double,double,double,double,double,double)*
 *                                                                                     *
 *  Description: Set the measurement for the EKF. It is assumed that the EKF will be   *
 *  called directly after setting the measurement                                      *  
 *                                                                                     * 
 ***************************************************************************************/
void C2BTrack::set_measurement(double veh1_x, double veh1_y, double bear1,double veh2_x, double veh2_y, double bear2, double tint,double msigma1,double msigma2)
{
  //the time interval since the last measurement
  dt = tint;

  //update Q and F matrices
  Q[0] = S_NOISE*dt*dt; Q[1] = S_NOISE*(dt*dt*dt)/2.0; Q[4] = S_NOISE*(dt*dt*dt)/2.0; Q[5] = (dt*dt*dt*dt*S_NOISE)/4.0; 
  Q[10] = S_NOISE*dt*dt; Q[11] = S_NOISE*(dt*dt*dt)/2.0; Q[14] = S_NOISE*(dt*dt*dt)/2.0; Q[15] = (dt*dt*dt*dt*S_NOISE)/4.0; 
  
  F[0] = 1.0;  F[4] = dt;  F[5] = 1.0;
  F[10] = 1.0; F[14] = dt; F[15] = 1.0;

  R[0] = (msigma1*msigma1); R[3] = (msigma2*msigma2) ;
  
  //copy to object variables
  v1posx = veh1_x;
  v1posy = veh1_y;
  bearings1 = bear1;

  v2posx = veh2_x;
  v2posy = veh2_y;
  bearings2 = bear2;

}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTrack::init_filter()                                           *
 *                                                                            *
 *  Description: Initialize the EKF state                                     * 
 *                                                                            * 
 ******************************************************************************/
void C2BTrack::init_filter(double x0, double x1, double x2, double x3)
{
  //X[0]=xdot, x[1]=x, x[2]=ydot,x[3]=y
  X[0] = x0;
  X[1] = x1;
  X[2] = x2;
  X[3] = x3;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTrack::Kalman_Loop()                                           *
 *                                                                            *
 *  Description: Iterate the EKF. It is assumed that the measurement has      *
 *  been set immediately prior to calling this method.                        * 
 *                                                                            * 
 ******************************************************************************/
void C2BTrack::Kalman_Loop()
{
  //set observation vector
  z[0] = bearings1;
  z[1] = bearings2;

  //compute linearized measurement matrix
  double term1 = (X[3]-v1posy);
  double term2 = (X[1]-v1posx);
  double term3 = pow(term1,2) + pow(term2,2);
  double term4 = (X[3]-v2posy);
  double term5 = (X[1]-v2posx);
  double term6 = pow(term4,2)+ pow(term5,2);
  H[1] = -term1/term3;
  H[3] =  term2/term3;
  H[5] = -term4/term6;
  H[7] =  term5/term6;
 
  //calculate Kalman gain K=PH'inv(HPH' + R)
  matrixmatrix(2,4,4,temp1,H,P);
  matrixtranspose(4,2,temp2,H);
  matrixmatrix(2,2,4,temp3,temp1,temp2);
  matrixadd(2,2,temp4,temp3,R);
  matrixinverse(2,temp5,temp4,1e-6);
  matrixmatrix(4,2,4,temp6,P,temp2);
  matrixmatrix(4,2,2,K,temp6,temp5);
 
  //model measurement
  HH[0] = atan(term1/term2);
  HH[1] = atan(term4/term5);
 
  //state update
  matrixsub(2,1,temp7,z,HH);
  matrixmatrix(4,1,2,temp3,K,temp7);
  matrixadd(4,1,temp5,X,temp3);
  matrixcopy(4,1,X,temp5);

  //calculate unfiltered solution
  double theta1 = tan(z[0]);
  double theta2 = tan(z[1]);
  //xuf = ((theta1*v1posx) - (theta2*v2posx) - v1posy + v2posy)/(theta1-theta2);
  //yuf = 0.5*(((theta1+theta2)*xuf) - (theta1*v1posx) - (theta2*v2posx) + v1posy + v2posy);

  //update covariance matrix P=(I-KH)P
  matrixunit(4,temp8);
  matrixmatrix(4,4,2,temp9,K,H);
  matrixsub(4,4,temp10,temp8,temp9);
  matrixmatrix(4,4,4,temp11,temp10,P);
  matrixcopy(4,4,P,temp11);

  // P=FPF' + Q
  matrixmatrix(4,4,4,temp8,F,P);
  matrixtranspose(4,4,temp9,F);
  matrixmatrix(4,4,4,temp10,temp8,temp9);
  matrixadd(4,4,temp11,temp10,Q);
  matrixcopy(4,4,P,temp11);

  //filtered solution
  xtpos = X[1];
  ytpos = X[3];
  xtdot = X[0];
  ytdot = X[2];

  //X=FX
  matrixmatrix(4,1,4,temp5,F,X);
  matrixcopy(4,1,X,temp5);  
}

/***************************************************************************************
 *                                                                                     *
 *  Method: C2BTrack::estimate_covariance(double*,double*,double*,double*)             *
 *                                                                                     *
 *  Description: Returns the components of the position estimate covariance matrix.    *
 *                                                                                     * 
 ***************************************************************************************/
void C2BTrack::estimate_covariance(double* sigma_xy,double* sigma_yx,double* sigmasq_x,double* sigmasq_y)
{
  *sigma_xy = P[7];
  *sigma_yx = P[13];
  *sigmasq_x    = P[5];
  *sigmasq_y    = P[15];
}

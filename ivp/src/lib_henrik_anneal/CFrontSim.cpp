/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE:                                                      */
/*    DATE:                                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/


#include <iostream>
#include "CFrontSim.h"

using namespace std;

void CFrontSim::setVars(double offset, double degrees, double amplitude, 
			double period, double wavelength, 
			double alpha_in, double beta_in,
			double temp_N, double temp_S)

{
  // Initialize FrontSim class
  //
  amp = amplitude;
  xi_0 = offset;
  angle = degrees;
  k=2*M_PI/wavelength;
  omega=2*M_PI/period;
  alpha = alpha_in;
  beta = beta_in;
  T_N = temp_N;
  T_S = temp_S;
  T_mean = (T_N+T_S)*0.5;
  T_fac = (T_N-T_S)/M_PI;
  //  cout << "CFrontSim: offset = " << offset << endl; 
  // cout << "CFrontSim: T_mean = " << T_mean << endl; 
  // cout << "CFrontSim: T_fac = " << T_fac << endl; 

}

void CFrontSim::setSigma(double temp_sigma) 
{
  T_sigma = temp_sigma;
}

void CFrontSim::setRegion(double x1, double x2,
			  double y1, double y2)
{
  x_min = x1;
  x_max = x2;
  y_min = y1;
  y_max = y2;

}

double CFrontSim::tempFunction(double t, double x, double y)
{ 
  double theta = angle*M_PI/180.0;
  double x_prime = x * cos(theta) + (y-xi_0)*sin(theta);
  double y_prime = (y-xi_0)*cos(theta) - x*sin(theta); 
  double y_front = amp*exp(-x_prime/alpha)*sin(x_prime*k-omega*t);
  double val = T_mean + T_fac*atan((y_prime-y_front)/beta);
  return(val);
}

double CFrontSim::tempMeas(double t, double x, double y)
{
  double dev = Ran.gauss_real(tempFunction(t,x,y),T_sigma);
  return(dev);
}
 




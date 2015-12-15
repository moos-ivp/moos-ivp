// H. Schmidt, henrik@mit.edu 07.02.10
// Laboratory for Autonomous Marine Sensing Systems (LAMSS)
// Massachusetts Institute of Technology 
// 
// CFrontSim.h  
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __CFRONTSIM_h__
#define __CFRONTSIM_h__
#include "MOOS/libMOOS/MOOSLib.h"

//#include "MOOSLIB/MOOSLib.h"
// #include "AngleUtils.h"
#include "Random.h"

#include <math.h>
#include <vector>
 
class CFrontSim 
{
 public:

  void setVars(double offset, double degrees, double amplitude , 
	       double period, double wavelength, double alph_0, double beta_0, 
	       double T_N, double T_S);
  void setSigma(double temp_sigma);
  void setRegion(double x1, double x2,
		 double y1, double y2); 
  double tempFunction(double t, double x, double y);
  double tempMeas(double t, double x, double y);

 protected:

  CRandom Ran;

  double amp;
  double xi_0;
  double angle;
  double k;
  double omega;
  double alpha;
  double beta;
  double T_N;
  double T_S;
  double T_fac;
  double T_sigma;
  double T_mean;

  double x_min;
  double x_max;
  double y_min;
  double y_max;

};
  
#endif /* __CFRONTSIM_h__ */

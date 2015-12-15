// H. Schmidt, henrik@mit.edu 07.02.10
// Laboratory for Autonomous Marine Sensing Systems (LAMSS)
// Massachusetts Institute of Technology 
// 
// CSimAnneal.h  
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

#ifndef __CSIMANNEAL_h__
#define __CSIMANNEAL_h__

//#include "MOOSLIB/MOOSLib.h"
#include "MOOS/libMOOS/MOOSLib.h"

#include "Random.h"
#include "CFrontSim.h"
#include <math.h>
#include <vector>
#include <iostream>
#include "MBUtils.h"

class CMeasurement
{
 public:
  double t;
  double x;
  double y;
  double temp;
};
 
class CSimAnneal 
{
 public:

  CSimAnneal();
  virtual ~CSimAnneal();

  void setVars(int num, double temp_fac, bool adapt);
  bool setInitVal(std::vector<double> val);
  bool setMinVal(std::vector<double> val);
  bool setMaxVal(std::vector<double> val);
  void getEstimate(std::vector<double>& est);
  void clearMeas();
  void addMeas(CMeasurement new_meas);
  CMeasurement parseMeas(std::string report);
  double calcEnergy();
  double heatBath(double temperature);
  double measModel(double t, double x, double y);
 
 protected:

  CRandom Ran;
  unsigned int num_vars;
  double k;
  bool adaptive;

  std::vector<double> variables;
  std::vector<double> var_min;
  std::vector<double> var_max;
  std::vector<double> var_norm;

  bool got_min;
  bool got_max;
  
  double Energy;

  std::vector<CMeasurement> measurements;
  std::vector<CMeasurement> model;

  CFrontSim front;
};
  
#endif /* __CSimAnneal_h__ */

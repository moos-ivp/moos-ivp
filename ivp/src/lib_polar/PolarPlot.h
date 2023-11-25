/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PolarPlot.h                                          */
/*    DATE: July 16th 2021                                       */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef POLAR_PLOT_HEADER
#define POLAR_PLOT_HEADER

#include <map>
#include <string>
#include <vector>

class PolarPlot {
public:
  PolarPlot();
  ~PolarPlot() {}

  bool addSetPoint(double hdg, double val);
  bool addSetPoints(std::string);
  
  std::map<double, double> getSetPoints() {return(m_setpts);}

  std::vector<double> getSetHdgs() const;
  std::vector<double> getSetVals() const;

  std::vector<double> getSetHdgsRotated() const;

  std::vector<double> getAllHdgs();
  std::vector<double> getAllVals();

  void setWindAngle(double angle);
  void modWindAngle(double degrees);
  
  void grabPoint(double hdg, double val) {};
  void modPoint(double hdg, double val);
  void removePoint(double hdg, double val);
  
  void   interpLinear();

  double getPolarPct(double);

  double getWindAngle() {return(m_wind_angle);}
  
  std::string getSpec() const;   

  bool set() const {return(m_setpts.size() > 0);}
  
  unsigned int size() const {return(m_setpts.size());}
  
protected:
  double interpLinear(double);           
  bool   getNeighbors(double hdg, double& hmin, double& vmin,
		      double& hmax, double& vmax);  
  double getClosestHdg(double hdg, double val);
  
protected:

  // key is heading, mapped value is a spd value
  std::map<double, double> m_setpts;

  std::map<int, double> m_allpts;

  bool   m_interpolated;
  
  double m_max_value;

  double m_wind_angle;
};

PolarPlot stringToPolarPlot(std::string);


#endif


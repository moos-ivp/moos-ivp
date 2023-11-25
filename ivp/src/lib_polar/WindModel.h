/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WindModel.h                                          */
/*    DATE: Jan 24th 2022                                        */
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

#ifndef WIND_MODEL_HEADER
#define WIND_MODEL_HEADER

#include <string>
#include "XYArrow.h"
#include "PolarPlot.h"

class WindModel
{
public:
  WindModel();
  ~WindModel() {}

 public: // Setters
  bool   setConditions(std::string);

  bool   setWindDir(double);  // Set
  bool   setWindSpd(double);  // and Drift
  bool   modWindDir(double);
  bool   modWindSpd(double);

  void   setTimeUTC(double v) {m_time_utc=v;}

 public: // Getters
  double getWindDir() const {return(m_wind_dir);}
  double getWindSpd() const {return(m_wind_spd);}

  double getTimeUTC() const {return(m_time_utc);}
  
  bool   set() const {return(m_set);}
  
 public:
  double getWindDir(double x, double y) const;
  double getWindSpd(double x, double y) const;

  double getMaxSpeed(PolarPlot, double hdg) const;
  
 public: // Visuals
  XYArrow getArrow() const {return(m_arrow);}

  std::string getArrowSpec() const;
  std::string getArrowSpec(double x, double y) const;
  std::string getArrowSpec(std::string params) const;

  std::string getModelSpec() const;

protected:
  bool   m_set;

  double m_wind_dir;
  double m_wind_spd;

  double m_time_utc;
  
  XYArrow m_arrow;
};

WindModel stringToWindModel(std::string);

#endif 



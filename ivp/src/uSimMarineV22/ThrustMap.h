/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ThrustMap.h                                          */
/*    DATE: Dec 16th 2010                                        */
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

#ifndef THRUST_MAP_HEADER
#define THRUST_MAP_HEADER

#include <string>
#include <vector>
#include <map>
#include "Figlog.h"

class ThrustMap
{
public:
  ThrustMap();
  ~ThrustMap() {}

 public: // Setters
  bool   addPair(double, double);
  void   setThrustFactor(double);
  void   setMinMaxThrust(double, double);
  void   setReflect(bool v)  {m_reflect_negative=v;}
 
 public: // Getters
  double getSpeedValue(double thrust) const;
  double getThrustValue(double speed) const;
  double getThrustFactor() const;
  bool   isAscending() const;
  bool   isConfigured();
  bool   usingThrustFactor() const;
  bool   usingReflect() const {return(m_reflect_negative);}

  std::string getMapPos() const;
  std::string getMapNeg() const;

 public: // Actions
  void   print() const;
  void   clear();
  void   clearFiglog() {m_figlog.clear();}

 protected:
  bool   isAscendingMap(std::map<double, double>) const;
  double getSpeedValueNeg(double thrust) const;
  double getSpeedValuePos(double thrust) const;
  double getThrustValueNeg(double speed) const;
  double getThrustValuePos(double speed) const;

 protected:
  std::map<double, double> m_pos_mapping;
  std::map<double, double> m_neg_mapping;

  double m_min_thrust;
  double m_max_thrust;
  bool   m_reflect_negative;

  double m_thrust_factor;

  Figlog m_figlog;
};

#endif 
















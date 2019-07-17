/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidObstacle.h                                  */
/*    DATE: Aug 2nd, 2006                                        */
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
 
#ifndef AOF_AVOID_OBSTACLE_HEADER
#define AOF_AVOID_OBSTACLE_HEADER

#include "AOF.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_AvoidObstacle: public AOF {
public:
  AOF_AvoidObstacle(IvPDomain);
  ~AOF_AvoidObstacle() {}

 public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
 public: // More virtuals defined Declare a known min/max eval range
  bool   minMaxKnown() const {return(true);}
  double getKnownMin() const {return(0);}
  double getKnownMax() const {return(100);}

  void   setObstacleOrig(XYPolygon poly) {m_obstacle_orig=poly;}
  void   setObstacleBuff(XYPolygon poly) {m_obstacle_buff=poly;}
  bool   initialize();

  std::string getDebugMsg() {return(m_debug_msg);}

 private: // Config variables
  double m_osx;
  double m_osy;
  double m_osh;
  double m_allowable_ttc;

  bool   m_osx_set;
  bool   m_osy_set;
  double m_osh_set;
  bool   m_allowable_ttc_set;

 private: // State variables
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed"  variable in IvPDomain

  bool   m_obstacle_on_port_curr;
  
  // A vector over the number of obstacles
  XYPolygon   m_obstacle_orig;
  XYPolygon   m_obstacle_buff;

  std::string m_debug_msg;
  
  // A vector over 360 (typically) heading values
  std::vector<double>  m_cache_distance;
};

#endif





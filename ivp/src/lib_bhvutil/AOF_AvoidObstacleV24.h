/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: AOF_AvoidObstacleV24.h                               */
/*    DATE: Sep 11, 2019 (derived from AOF_AvoidObstacle)        */
/*    DATE: Aug 5th, 2023 (AvoidObstacleX + PlatModel)           */
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
 
#ifndef AOF_AVOID_OBSTACLE_V24_HEADER
#define AOF_AVOID_OBSTACLE_V24_HEADER

#include "AOF.h"
#include "ObShipModelV24.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_AvoidObstacleV24: public AOF {
public:
  AOF_AvoidObstacleV24(IvPDomain);
  ~AOF_AvoidObstacleV24() {}

 public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
 public: // More virtuals defined Declare a known min/max eval range
  bool   minMaxKnown() const {return(true);}
  double getKnownMin() const {return(0);}
  double getKnownMax() const {return(100);}

  void   setObShipModel(ObShipModelV24 obm) {m_obship_model=obm;}
  bool   initialize();

 private: // Config variables
  ObShipModelV24 m_obship_model;

 private: // State variables
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed"  variable in IvPDomain
};

#endif

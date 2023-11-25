/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_AvoidObstacleX.h                                 */
/*    DATE: Sep 11, 2019 (derived from AOF_AvoidObstacle)        */
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
 
#ifndef AOF_AVOID_OBSTACLEX_HEADER
#define AOF_AVOID_OBSTACLEX_HEADER

#include "AOF.h"
#include "ObShipModel.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_AvoidObstacleX: public AOF {
public:
  AOF_AvoidObstacleX(IvPDomain);
  ~AOF_AvoidObstacleX() {}

 public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
 public: // More virtuals defined Declare a known min/max eval range
  bool   minMaxKnown() const {return(true);}
  double getKnownMin() const {return(0);}
  double getKnownMax() const {return(100);}

  void   setObShipModel(ObShipModel obm) {m_obship_model=obm;}
  bool   initialize();

 private: // Config variables
  ObShipModel m_obship_model;

 private: // State variables
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed"  variable in IvPDomain
};

#endif


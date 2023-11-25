/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_AvoidCollision.h                                 */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
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

#ifndef AOF_AVOID_COLLISION_DEPTH_HEADER
#define AOF_AVOID_COLLISION_DEPTH_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class IvPDomain;
class AOF_AvoidCollisionDepth: public AOF {
public:
  AOF_AvoidCollisionDepth(IvPDomain);
  ~AOF_AvoidCollisionDepth() 
    {if(cpa_engine) delete(cpa_engine);}

 public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

  double getROC() {return(m_rate_of_closure);}

 protected:
  double metric(double dist) const;
  double evalDiveDepth(double, double) const;


 protected:
  double m_tol;  // Ownship Time on Leg
  double m_osx;  // Ownship X position (meters)
  double m_osy;  // Ownship Y position (meters)
  double m_osh;  // Ownship Heading (degrees)
  double m_osv;  // Ownship Speed (m/s)

  double m_cnx;  // Contact X position (meters)
  double m_cny;  // Contact Y position (meters)
  double m_cnh;  // Contact heading
  double m_cnv;  // Contact speed

  double m_rate_of_closure;

  double m_collision_distance;
  double m_all_clear_distance;

  double m_collision_depth;
  double m_max_decision_depth;

  bool   m_tol_set;
  bool   m_osy_set;
  bool   m_osx_set;
  bool   m_osh_set;
  bool   m_osv_set;

  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;
  bool   m_collision_distance_set;
  bool   m_all_clear_distance_set;
  bool   m_collision_depth_set;

  CPAEngine *cpa_engine;

  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed"  variable in IvPDomain
  int    m_dep_ix;  // Index of "depth"  variable in IvPDomain
};

#endif



















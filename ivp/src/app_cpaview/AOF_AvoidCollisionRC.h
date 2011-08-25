/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollisionRC.h                               */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef AOF_AVOID_COLLISION_RC_HEADER
#define AOF_AVOID_COLLISION_RC_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class IvPDomain;
class AOF_AvoidCollisionRC: public AOF {
public:
  AOF_AvoidCollisionRC(IvPDomain);
  ~AOF_AvoidCollisionRC() 
    {if(cpa_engine) delete(cpa_engine);};

public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double metric(double) const;

protected:
  double os_tol;   // Ownship Time on Leg
  double os_lat;   // Ownship Lat position
  double os_lon;   // Ownship Lon position
  double cn_lat;   // Contact Lat position
  double cn_lon;   // Contact Lon position
  double cn_crs;   // Contact heading
  double cn_spd;   // Contact speed
  double collision_distance;
  double all_clear_distance;

  bool   os_tol_set;
  bool   os_lat_set;
  bool   os_lon_set;
  bool   cn_lat_set;
  bool   cn_lon_set;
  bool   cn_crs_set;
  bool   cn_spd_set;
  bool   collision_distance_set;
  bool   all_clear_distance_set;

  CPAEngine *cpa_engine;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif









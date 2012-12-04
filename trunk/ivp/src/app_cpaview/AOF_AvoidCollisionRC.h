/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollisionRC.h                               */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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










/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_AvoidCollision.h                                 */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*          November 18th 2006 (RC Version written)              */
/*****************************************************************/
 
#ifndef AOF_AVOID_COLLISION_HEADER
#define AOF_AVOID_COLLISION_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class IvPDomain;
class AOF_AvoidCollision: public AOF {
public:
  AOF_AvoidCollision(IvPDomain);
  ~AOF_AvoidCollision() 
    {if(cpa_engine) delete(cpa_engine);};

public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double metric(double) const;

protected:
  double m_os_tol;   // Ownship Time on Leg
  double m_os_lat;   // Ownship Lat position
  double m_os_lon;   // Ownship Lon position
  double m_cn_lat;   // Contact Lat position
  double m_cn_lon;   // Contact Lon position
  double m_cn_crs;   // Contact heading
  double m_cn_spd;   // Contact speed
  double m_collision_distance;
  double m_all_clear_distance;

  bool   m_os_tol_set;
  bool   m_os_lat_set;
  bool   m_os_lon_set;
  bool   m_cn_lat_set;
  bool   m_cn_lon_set;
  bool   m_cn_crs_set;
  bool   m_cn_spd_set;
  bool   m_collision_distance_set;
  bool   m_all_clear_distance_set;

  CPAEngine *cpa_engine;

  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif








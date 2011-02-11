/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Waypoint.h                                       */
/*    DATE: Sep 28th 2006                                        */
/*****************************************************************/
 
#ifndef AOF_WAYPOINT_HEADER
#define AOF_WAYPOINT_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_Waypoint: public AOF {
 public:
  AOF_Waypoint(IvPDomain);
  ~AOF_Waypoint() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  // Initialization parameters
  double m_osx;   // Ownship x position at time Tm.
  double m_osy;   // Ownship y position at time Tm.
  double m_ptx;   // x component of next the waypoint.
  double m_pty;   // y component of next the waypoint.
  double m_desired_spd;

 // Initialization parameter set flags
  bool   m_osx_set;  
  bool   m_osy_set;  
  bool   m_ptx_set;  
  bool   m_pty_set;   
  bool   m_desired_spd_set;

  // Cached values for more efficient evalBox calls
  double m_angle_to_wpt;
  double m_max_speed;
  int    m_crs_ix;     // Index of "course" var in IvPDomain
  int    m_spd_ix;     // Index of "speed"  var in IvPDomain
};

#endif








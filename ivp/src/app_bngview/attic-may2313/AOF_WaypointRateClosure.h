/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_WaypointRateClosure.h                            */
/*    DATE: Sep 28th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef AOF_WAYPOINT_RATE_CLOSURE_HEADER
#define AOF_WAYPOINT_RATE_CLOSURE_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_WaypointRateClosure: public AOF {
 public:
  AOF_WaypointRateClosure(IvPDomain);
  ~AOF_WaypointRateClosure() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double os_y;   // Ownship Lat position at time Tm.
  double os_x;   // Ownship Lon position at time Tm.
  double pt_y;   
  double pt_x;  
  double desired_spd;
  double max_speed;

  bool   os_y_set;  
  bool   os_x_set;  
  bool   pt_y_set;   
  bool   pt_x_set;  
  bool   desired_spd_set;

  double angle_to_wpt;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif










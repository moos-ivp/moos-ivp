/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Waypoint2D.h                                     */
/*    DATE: Jan 4th 2006                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef AOF_WAYPOINT2D_HEADER
#define AOF_WAYPOINT2D_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "AngleUtils.h"

class AOF_Waypoint2D: public AOF {
 public:
  AOF_Waypoint2D(IvPDomain);
  ~AOF_Waypoint2D() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize() {
    current_angle = relAng(osLON, osLAT, ptLON, ptLAT);
    return(true);
  };

protected:
  double osLAT;   // Ownship Lat position at time Tm.
  double osLON;   // Ownship Lon position at time Tm.
  double ptLAT;   
  double ptLON;  
  double desired_speed;
  double current_angle;
};

#endif









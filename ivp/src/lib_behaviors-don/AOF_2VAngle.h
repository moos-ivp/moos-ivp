/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_2VAngle.h                                  */
/*    DATE: 23 July 05                                     */
/***********************************************************/
 
#ifndef AOF_2VANGLE_HEADER
#define AOF_2VANGLE_HEADER

#include "AOF.h"

#define LR  1
#define OPT 2


class IvPDomain;
class AOF_2VAngle: public AOF {
 public:
  AOF_2VAngle(IvPDomain, double,double,double,double,double,double,double,double);
  ~AOF_2VAngle() {};

public:    
  double evalBox(const IvPBox*) const;    // virtual defined
  double AngleMetric(double,double) const;

protected:

  
  double osCourse;        //ownship course
  double osX;
  double osY;
  double tx,ty,thead,tspeed,v2_x,v2_y;
  double min_range,cgamCN,sgamCN,gamCN,curr_dist;

  double crsBase,spdBase,tolBase;
  double crsDelta,tolDelta,spdDelta;
};

#endif

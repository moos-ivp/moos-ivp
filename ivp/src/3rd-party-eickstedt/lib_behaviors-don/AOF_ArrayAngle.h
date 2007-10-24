/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_ArrayAngle.h                               */
/*    DATE: 23 July 05                                     */
/***********************************************************/
 
#ifndef AOF_ARRAYANGLE_HEADER
#define AOF_ARRAYANGLE_HEADER

#include "AOF.h"

class IvPDomain;

class AOF_ArrayAngle: public AOF {
 public:
  AOF_ArrayAngle(IvPDomain);
  ~AOF_ArrayAngle() {};

  double evalBox(const IvPBox*) const;    // virtual defined
  double Arraymetric(double,double) const;
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:

  int    mode,crs_ix,spd_ix;
  double osCourse;        //ownship course
  double raydirec,t_bearing,desired_angle;
  double leftabs,rightabs,hwidth;

};

#endif

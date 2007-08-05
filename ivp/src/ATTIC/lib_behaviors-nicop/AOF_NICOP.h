/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: AOF_NICOP.h                                          */
/*    DATE: Dec 8th 2005                                         */
/*****************************************************************/
 
#ifndef AOF_NICOP_HEADER
#define AOF_NICOP_HEADER

#include "AOF.h"
#include "IvPDomain.h"

class AOF_NICOP: public AOF {
 public:
  AOF_NICOP(IvPDomain, double, double, double x=0, double y=5);
  ~AOF_NICOP() {};

public:    
  double evalBox(const IvPBox*) const;   // virtual defined

protected:
  double metric(double) const;

  double crsBase;
  double crsDelta;
  double spdBase;
  double spdDelta;
  double spdHigh;
  double tolBase;
  double tolDelta;

  double osX;
  double osY;
};

#endif


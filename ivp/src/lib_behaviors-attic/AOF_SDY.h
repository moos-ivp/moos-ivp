/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SDY.h                                            */
/*    DATE: May 10th, 2001                                       */
/*****************************************************************/
 
#ifndef AOF_SDY_HEADER
#define AOF_SDY_HEADER

#include "AOF.h"
#include "UFunc_EXP.h"

class IvPDomain;
class AOF_SDY: public AOF {
public:
  AOF_SDY(IvPDomain);
  ~AOF_SDY() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

private:
  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain

  double desired_crs;
  bool   desired_crs_set;

  UFunc_EXP ufunc_exp;
};

#endif








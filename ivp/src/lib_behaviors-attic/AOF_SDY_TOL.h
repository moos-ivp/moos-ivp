/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SDY_TOL.h                                        */
/*    DATE: May 10th, 2001                                       */
/*****************************************************************/
 
#ifndef AOF_SDY_TOL_HEADER
#define AOF_SDY_TOL_HEADER

#include "AOF.h"
#include "UFunc_EXP.h"

class IvPDomain;
class AOF_SDY_TOL: public AOF {
public:
  AOF_SDY_TOL(IvPDomain);
  ~AOF_SDY_TOL() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   initialize();

private:
  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
  int    tol_ix;  // Index of "tol" variable in IvPDomain

  double desired_crs;
  bool   desired_crs_set;

  UFunc_EXP ufunc_exp;
};

#endif








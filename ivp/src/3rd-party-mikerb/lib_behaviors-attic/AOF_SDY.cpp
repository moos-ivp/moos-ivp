/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SDY.cpp                                          */
/*    DATE: March 21st, 2001                                     */
/*****************************************************************/
 
#include <math.h>
#include <string>
#include "AOF_SDY.h"
#include "AngleUtils.h"
#include "UFunc_EXP.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_SDY::AOF_SDY(IvPDomain gdomain) : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");

  desired_crs_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_SDY::setParam(const string& param, double param_val)
{
  if(param == "desired_course") {
    desired_crs = param_val;
    desired_crs_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_SDY::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);

  if(!desired_crs)
    return(false);

  double max_possible = pow(359, 1.8) + pow(30, 3.0);

  ufunc_exp.setExp(32);
  ufunc_exp.setCenterVal(0);
  ufunc_exp.setDeltaMax(max_possible);
  ufunc_exp.setMultiplier(1.0);

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_SDY::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;

  domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);

  double delta  = fabs(desired_crs - eval_crs);
  if(delta > 180.0) delta = fabs(delta - 360.0);

  double val_diff = pow(delta, 1.8);
  double uval     = ufunc_exp.eval(val_diff) * 100.0;

  return(uval);
}



/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Linear.cpp                                       */
/*    DATE: January 3rd, 2008                                    */
/*****************************************************************/

#include "AOF_Linear.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Linear::evalBox(const IvPBox *ptbox) const
{
  double x_val = extract("x", ptbox);
  double y_val = extract("y", ptbox);
  
  return((m_coeff * x_val) + (n_coeff * y_val));
}

//----------------------------------------------------------------
// Procedure: evalBox

bool AOF_Linear::setParam(const std::string& param, double val) 
{
  if(param == "mcoeff")
    m_coeff = val;
  else if(param == "ncoeff")
    n_coeff = val;
  else
    return(false);
  return(true);
};


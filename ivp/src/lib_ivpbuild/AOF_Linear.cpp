/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Linear.cpp                                       */
/*    DATE: January 3rd, 2008                                    */
/*****************************************************************/

#include "AOF_Linear.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Linear::AOF_Linear(IvPDomain domain) : AOF(domain)
{
  m_coeff = 0;
  n_coeff = 0;
  m_coeff_set = false;
  n_coeff_set = false;
  
  x_index = domain.getIndex("x");
  y_index = domain.getIndex("y");
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Linear::evalBox(const IvPBox *ptbox) const
{
  double x_val = m_domain.getVal(x_index, ptbox->pt(x_index));
  double y_val = m_domain.getVal(y_index, ptbox->pt(y_index));
  
  return((m_coeff * x_val) + (n_coeff * y_val));
}

//----------------------------------------------------------------
// Procedure: evalBox

bool AOF_Linear::setParam(const std::string& param, double val) 
{
  if(param == "mcoeff") {
    m_coeff = val;
    m_coeff_set = true;
  }
  else if(param == "ncoeff") {
    n_coeff = val;
    n_coeff_set = true;
  }
  else
    return(false);
  return(true);
};

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_Linear::initialize()
{
  if(m_coeff_set && n_coeff_set && (x_index != -1) && (y_index != -1))
    return(true);
  return(false);
};

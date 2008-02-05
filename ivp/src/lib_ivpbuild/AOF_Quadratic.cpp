/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Quadratic.cpp                                    */
/*    DATE: Feb 2rd, 2008                                        */
/*****************************************************************/

#include "AOF_Quadratic.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Quadratic::AOF_Quadratic(IvPDomain domain) : AOF(domain)
{
  m_coeff  = 0;
  n_coeff  = 0;
  x_center = 0;
  y_center = 0;
  x_index  = domain.getIndex("x");
  y_index  = domain.getIndex("y");
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Quadratic::setParam(const std::string& param, double val) 
{
  if(param == "mcoeff")
    m_coeff = val;
  else if(param == "ncoeff")
    n_coeff = val;
  else if(param == "x_center")
    x_center = val;
  else if(param == "y_center")
    y_center = val;
  else
    return(false);
  return(true);
};

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_Quadratic::initialize()
{
  if((x_index != -1) && (y_index != -1))
    return(true);
  return(false);
};

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Quadratic::evalBox(const IvPBox *ptbox) const
{
  double x_val = m_domain.getVal(x_index, ptbox->pt(x_index));
  double y_val = m_domain.getVal(y_index, ptbox->pt(y_index));
  
  return((m_coeff * (x_val - x_center) * (x_val - x_center)) + 
	 (n_coeff * (y_val - y_center) * (y_val - y_center)));
}


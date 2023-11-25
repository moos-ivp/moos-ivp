/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandomVariable.cpp                                   */
/*    DATE: Dec 18th 2009                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include "RandomVariable.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandomVariable::RandomVariable()
{
  // Initial values for state variables.
  m_value   = 0;

  // Initial values for configuration parameters
  m_varname = "random_var";
  m_keyname = "";

  m_snap = 0.001;
}

//---------------------------------------------------------
// Procedure: setParam

bool RandomVariable::setParam(string param, double value)
{
  if(param == "min") {
    m_min_val = value;
    if(m_max_val < m_min_val)
      m_max_val = m_min_val;
  }
  else if(param == "max") {
    m_max_val = value;
    if(m_min_val > m_max_val)
      m_min_val = m_max_val;
  }
  else if(param == "snap") {
    if(value > 0)
      m_snap = value;
  }
  else
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: getStringValue

string RandomVariable::getStringValue() const
{
  return(m_value_str);
}
  
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandomVariable::getStringSummary() const
{
  string str = "varname=" + m_varname;
  if(m_keyname != "")
    str += ",keyname=" + m_keyname;
  if(m_type != "")
    str += ",type=" + m_type;
  str += ",min=" + doubleToStringX(m_min_val);
  str += ",max=" + doubleToStringX(m_max_val);
  str += ",val=" + doubleToStringX(m_value);
  return(str);
}










/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "RandVarUniform.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandVarUniform::RandVarUniform() : RandomVariable()
{
  m_min_val = 0;
  m_max_val = 1;
}


//---------------------------------------------------------
// Procedure: setParam

bool RandVarUniform::setParam(string param, double value)
{
  bool result = RandomVariable::setParam(param, value);
  return(result);
}

//---------------------------------------------------------
// Procedure: reset

void RandVarUniform::reset()
{
  // Unlikely but check and handle special case anyway.
  if(m_min_val >= m_max_val) {
    m_value = m_min_val;
    return;
  }

  int    rand_int = rand() % 10000;
  double rand_pct = (double)(rand_int) / 10000;
  m_value = m_min_val + ((m_max_val-m_min_val) * rand_pct);
  m_value_str = doubleToStringX(m_value, 3);
}
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandVarUniform::getStringSummary() const
{
  string str = RandomVariable::getStringSummary();
  if(str != "")
    str += ",";

  str += "type=uniform";
  return(str);
}
  









/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandomPair.cpp                                       */
/*    DATE: Nov 22nd 2016                                        */
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
#include "RandomPair.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandomPair::RandomPair()
{
  // Initial values for state variables.
  m_value1   = 0;
  m_value2   = 0;

  // Initial values for configuration parameters
  m_varname1 = "random_var1";
  m_varname2 = "random_var2";
  m_keyname  = "";
}

//---------------------------------------------------------
// Procedure: setParam

bool RandomPair::setParam(string param, double value)
{
  return(false); // indicates not handled
}

//---------------------------------------------------------
// Procedure: setParam

bool RandomPair::setParam(string param, string value)
{
  return(false); // indicates not handled
}

//---------------------------------------------------------
// Procedure: getStringValue1

string RandomPair::getStringValue1() const
{
  return(m_value_str1);
}
  
//---------------------------------------------------------
// Procedure: getStringValue2

string RandomPair::getStringValue2() const
{
  return(m_value_str2);
}
  
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandomPair::getStringSummary() const
{
  string str = "varname1=" + m_varname1 + ",varname2=" + m_varname2;
  if(m_keyname != "")
    str += ",keyname=" + m_keyname;
  if(m_type != "")
    str += ",type=" + m_type;
  str += ",val1=" + doubleToStringX(m_value1);
  str += ",val2=" + doubleToStringX(m_value2);
  return(str);
}










/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariable.cpp                                   */
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
#include "RandPairPoly.h"
#include "MBUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandPairPoly::RandPairPoly() : RandomPair()
{
  m_type = "poly";
}

//---------------------------------------------------------
// Procedure: setParam

bool RandPairPoly::setParam(string param, string value)
{
  bool handled = RandomPair::setParam(param, value);
  if(handled)
    return(true);
  
  if(param == "poly") {
    XYPolygon poly = string2Poly(value);
    if(poly.size() != 0) {
      m_poly = poly;
      return(true);
    }
  }

  return(false);
}

//---------------------------------------------------------
// Procedure: reset

void RandPairPoly::reset()
{
  // Sanity check
  if(m_poly.size() == 0)
    return;

  m_value1 = 1;
  m_value2 = 2;
}
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandPairPoly::getStringSummary() const
{
  string str = RandomPair::getStringSummary();
  if(str != "")
    str += ",";

  str += "type=poly";
  return(str);
}
 

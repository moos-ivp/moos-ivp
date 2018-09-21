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
    cout << "value:[" << value << "]" << endl;
    XYPolygon poly = string2Poly(value);
    if(poly.is_convex()) {
      m_poly = poly;
      return(true);
    }
    else
      exit(1);
  }

  return(false);
}

//---------------------------------------------------------
// Procedure: reset

void RandPairPoly::reset()
{
  // Sanity check
  if(!m_poly.is_convex())
    return;
  
  bool success = false;
  unsigned int max_tries = 500;
  
  double bnd_x_low = m_poly.get_min_x();
  double bnd_x_hgh = m_poly.get_max_x();
  double bnd_y_low = m_poly.get_min_y();
  double bnd_y_hgh = m_poly.get_max_y();

  double x_range = 10000 * (bnd_x_hgh - bnd_x_low);
  double y_range = 10000 * (bnd_y_hgh - bnd_y_low);

  int int_x_range = (int)(x_range);
  int int_y_range = (int)(y_range);

  for(unsigned int i=0; (i<max_tries) && !success; i++) {

    int rand_x = rand() % int_x_range;
    int rand_y = rand() % int_y_range;

    double rand_dx = bnd_x_low + ((double)(rand_x)) / 10000;
    double rand_dy = bnd_y_low + ((double)(rand_y)) / 10000;

    if(m_poly.contains(rand_dx, rand_dy)) {
      m_value1 = rand_dx;
      m_value2 = rand_dy;
      success = true;
    }
  }
  if(!success) {
    m_value1 = m_poly.get_center_x();
    m_value2 = m_poly.get_center_y();
  }

  m_value_str1 = doubleToString(m_value1, 3);
  m_value_str2 = doubleToString(m_value2, 3);
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
 





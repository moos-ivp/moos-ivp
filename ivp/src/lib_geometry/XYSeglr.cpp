/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSeglr.cpp                                          */
/*    DATE: Apr 27th, 2015                                       */
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

#include <cmath>
#include <cstdlib>
#include <cstring>
#include "XYSeglr.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: clear

void XYSeglr::clear()
{
  m_seglr = Seglr();
}

//---------------------------------------------------------------
// Procedure: getRayBaseX()

double XYSeglr::getRayBaseX() const
{
  unsigned int vsize = m_seglr.size();
  if(vsize == 0)
    return(0);

  return(m_seglr.getVX(vsize-1));
}

//---------------------------------------------------------------
// Procedure: getRayBaseY()

double XYSeglr::getRayBaseY() const
{
  unsigned int vsize = m_seglr.size();
  if(vsize == 0)
    return(0);

  return(m_seglr.getVY(vsize-1));
}


//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: Get a string specification of the seglr. We set 
//            the vertex precision to be at the integer by default.

string XYSeglr::getSpec(int precision) const
{
  string spec = m_seglr.getSpec(precision);

  string obj_spec = XYObject::get_spec();

  if(obj_spec != "") {
    if(spec != "")
      spec += ",";
    spec += obj_spec;
  }

  return(spec);
}


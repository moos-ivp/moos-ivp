/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYPoint.cpp                                          */
/*    DATE: July 17th, 2008                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include <cstring>
#include "XYPoint.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: clear

void XYPoint::clear()
{
  XYObject::clear();

  m_x     = 0; 
  m_y     = 0; 
  m_z     = 0; 
  m_valid = false;
}

//---------------------------------------------------------------
// Procedure: set_spec_digits
//      Note: Determines the number of significant digits used when
//            creating the string representation of the point. It
//            affects only the x,y,z parameters.

void XYPoint::set_spec_digits(unsigned int digits)
{
  m_sdigits = digits;
  if(m_sdigits > 6)
    m_sdigits = 6;
}

//---------------------------------------------------------------
// Procedure: apply_snap

void XYPoint::apply_snap(double snapval)
{
  m_x = snapToStep(m_x, snapval);
  m_y = snapToStep(m_y, snapval);
  m_z = snapToStep(m_z, snapval);
}

//---------------------------------------------------------------
// Procedure: projectPt
//   Purpose: 

void XYPoint::projectPt(const XYPoint& pt, double ang, double dist)
{
  projectPoint(ang, dist, pt.x(), pt.y(), m_x, m_y);
}

//---------------------------------------------------------------
// Procedure: get_spec
//   Purpose: 

string XYPoint::get_spec(string param) const
{
  string spec;

  spec += "x="  + doubleToStringX(m_x, m_sdigits);
  spec += ",y=" + doubleToStringX(m_y, m_sdigits);

  // Since z=0 is inferred if left unspecified, don't add it to the 
  // string representation unless nonzero.
  if(m_z != 0)
    spec += ",z=" + doubleToStringX(m_z, m_sdigits);

  string remainder = XYObject::get_spec(param);
  if(remainder != "")
    spec += "," + remainder;

  return(spec);
}










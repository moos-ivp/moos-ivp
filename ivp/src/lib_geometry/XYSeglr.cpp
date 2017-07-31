/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYSeglr.cpp                                          */
/*    DATE: Apr 27th, 2015                                       */
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

#include <cmath>
#include <cstdlib>
#include <cstring>
#include "XYSeglr.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

XYSeglr::XYSeglr()
{
  m_raylen = 5;   // length of the rendered ray in meters
  m_headsz = 3;   // Size of the rendered head in meters
}

//---------------------------------------------------------------
// Constructor

XYSeglr::XYSeglr(Seglr seglr)
{
  m_seglr = seglr;
  m_raylen = 5;   // length of the rendered ray in meters
  m_headsz = 3;   // Size of the rendered head in meters  
}

//---------------------------------------------------------------
// Procedure: clear()

void XYSeglr::clear()
{
  m_seglr = Seglr();
}

//---------------------------------------------------------------
// Procedure: setRayLen()

void XYSeglr::setRayLen(double len)
{
  m_raylen = len;
  if(m_raylen < 0)
    m_raylen = 0;
}

//---------------------------------------------------------------
// Procedure: setHeadSize()

void XYSeglr::setHeadSize(double val)
{
  m_headsz = val;
  if(m_headsz < 0)
    m_headsz = 0;
}

//---------------------------------------------------------------
// Procedure: valid()

bool XYSeglr::valid() const
{
  if(m_seglr.size() == 0)
    return(false);
  
  return(true);
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

string XYSeglr::get_spec(int precision) const
{
  string spec = m_seglr.getSpec(precision);

  string obj_spec = XYObject::get_spec();

  if(obj_spec != "") {
    if(spec != "")
      spec += ",";
    spec += obj_spec;
  }

  if(spec != "")
    spec += ",";
  spec += "ray_len=" + doubleToStringX(m_raylen,2);
  spec += ",head_size=" + doubleToStringX(m_headsz,1);
  
  return(spec);
}






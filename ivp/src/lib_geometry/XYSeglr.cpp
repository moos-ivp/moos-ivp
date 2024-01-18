/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
// Constructor()

XYSeglr::XYSeglr(double ray_angle)
{
  init();
  m_ray_angle = ray_angle;
}

//---------------------------------------------------------------
// Constructor()

XYSeglr::XYSeglr(double x0, double y0)
{
  init();
  m_vx.push_back(x0);
  m_vy.push_back(y0);
 }

//---------------------------------------------------------------
// Constructor()

XYSeglr::XYSeglr(double x0, double y0, double ray_angle)
{
  init();
  m_vx.push_back(x0);
  m_vy.push_back(y0);
  m_ray_angle = ray_angle;   
}

//---------------------------------------------------------------
// Constructor()

XYSeglr::XYSeglr(double x0, double y0, double x1, double y1)
{
  init();
  m_vx.push_back(x0);
  m_vy.push_back(y0);
  m_vx.push_back(x1);
  m_vy.push_back(y1);
}

//---------------------------------------------------------------
// Procedure: init()

void XYSeglr::init()
{
  m_ray_angle = 0;   
  m_raylen    = 5;   // length of the rendered ray in meters
  m_headsz    = 3;   // Size of the rendered head in meters
  m_cpa       = -1;
  m_cpa_stem  = -1;
}

//---------------------------------------------------------------
// Procedure: addVertex()

void XYSeglr::addVertex(double x, double y)
{
  m_vx.push_back(x);
  m_vy.push_back(y);
}

//---------------------------------------------------------------
// Procedure: setVertex()

void XYSeglr::setVertex(double x, double y, unsigned int ix)
{
  if((ix >= m_vx.size()) || (ix >= m_vy.size()))
    return;

  m_vx[ix] = x;
  m_vy[ix] = y;
}

//---------------------------------------------------------------
// Procedure: setRayAngle()
//      Note: No error checking. Consumers/users will need to do this.

void XYSeglr::setRayAngle(double angle)
{
  m_ray_angle = angle;
}

//---------------------------------------------------------------
// Procedure: setRayLen()
//      Note: The visual length, for rendering purposes

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
// Procedure: setCacheCPA()

void XYSeglr::setCacheCPA(double val)
{
  if(val < 0)
    val = 0;
  m_cpa = val;
}

//---------------------------------------------------------------
// Procedure: setCacheStemCPA()

void XYSeglr::setCacheStemCPA(double val)
{
  if(val < 0)
    val = 0;
  m_cpa_stem = val;
}

//---------------------------------------------------------------
// Procedure: setCacheCPAPoint()

void XYSeglr::setCacheCPAPoint(XYPoint point)
{
  if(point.valid())
    m_cpa_pt = point;
}

//---------------------------------------------------------------
// Procedure: valid()

bool XYSeglr::valid() const
{
  if(m_vx.size() != m_vy.size())
    return(false);
  
  if(m_vx.size() == 0)
    return(false);
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: getRayBaseX()

double XYSeglr::getRayBaseX() const
{
  unsigned int vsize = m_vx.size();
  if(vsize == 0)
    return(0);
  
  return(m_vx[vsize-1]);
}

//---------------------------------------------------------------
// Procedure: getRayBaseY()

double XYSeglr::getRayBaseY() const
{
  unsigned int vsize = m_vy.size();
  if(vsize == 0)
    return(0);

  return(m_vy[vsize-1]);
}

//---------------------------------------------------------------
// Procedure: getBaseSegList()

XYSegList XYSeglr::getBaseSegList() const
{
  XYSegList segl;

  for(unsigned int i=0; i<m_vx.size(); i++)
    segl.add_vertex(m_vx[i], m_vy[i]);

  return(segl);
}


//---------------------------------------------------------------
// Procedure: getVX()

double XYSeglr::getVX(unsigned int ix) const
{
  if(ix >= m_vx.size())
    return(0);
  return(m_vx[ix]);
}

//---------------------------------------------------------------
// Procedure: getVY()

double XYSeglr::getVY(unsigned int ix) const
{
  if(ix >= m_vy.size())
    return(0);
  return(m_vy[ix]);
}

//---------------------------------------------------------------
// Procedure: translateTo()

void XYSeglr::translateTo(double new_base_x, double new_base_y)
{
  if((m_vx.size() == 0) || (m_vy.size() == 0))
    return;
    
  double curr_base_x = m_vx[0];
  double curr_base_y = m_vy[0];
  
  double trans_x = new_base_x - curr_base_x;
  double trans_y = new_base_y - curr_base_y;

  for(unsigned int i=0; i<m_vx.size(); i++) {
    m_vx[i] += trans_x;
    m_vy[i] += trans_y;
  }
}

//---------------------------------------------------------------
// Procedure: reflect()

void XYSeglr::reflect()
{
  for(unsigned int i=0; i<m_vx.size(); i++) 
    m_vx[i] = -m_vx[i];
  m_ray_angle = (360.0 - m_ray_angle);
}

//---------------------------------------------------------------
// Procedure: getMinX()

double XYSeglr::getMinX() const
{
  double min_x = 0;
  for(unsigned int i=0; i<m_vx.size(); i++) {
    if((i==0) || (m_vx[i] < min_x))
      min_x = m_vx[i];
  }
  return(min_x);
}

//---------------------------------------------------------------
// Procedure: getMaxX()

double XYSeglr::getMaxX() const
{
  double max_x = 0;
  for(unsigned int i=0; i<m_vx.size(); i++) {
    if((i==0) || (m_vx[i] > max_x))
      max_x = m_vx[i];
  }
  return(max_x);
}

//---------------------------------------------------------------
// Procedure: getMinY()

double XYSeglr::getMinY() const
{
  double min_y = 0;
  for(unsigned int i=0; i<m_vy.size(); i++) {
    if((i==0) || (m_vy[i] < min_y))
      min_y = m_vy[i];
  }
  return(min_y);
}

//---------------------------------------------------------------
// Procedure: getMaxY()

double XYSeglr::getMaxY() const
{
  double max_y = 0;
  for(unsigned int i=0; i<m_vy.size(); i++) {
    if((i==0) || (m_vy[i] > max_y))
      max_y = m_vy[i];
  }
  return(max_y);
}

//---------------------------------------------------------------
// Procedure: get_spec()
//   Purpose: Get a string specification of the seglr. We set 
//            the vertex precision to be at the integer by default.

string XYSeglr::get_spec(int precision) const
{
  string spec = "pts={";

  for(unsigned int i=0; i<m_vx.size(); i++) {
    string sx = doubleToString(m_vx[i], precision);
    string sy = doubleToString(m_vy[i], precision);

    string pair = sx + "," + sy;
    if(i>0)
      spec += ":";
    spec += pair;
  }
  spec += "},ray=" + doubleToString(m_ray_angle, precision);

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

  if(m_cpa >= 0)
    spec += ",cpa=" + doubleToStringX(m_cpa,2);
  if(m_cpa_pt.valid()) {
    spec += ",cpax=" + doubleToStringX(m_cpa_pt.x(),2);
    spec += ",cpay=" + doubleToStringX(m_cpa_pt.y(),2);
  }
  
  return(spec);
}


//-----------------------------------------------------------------
// Procedure: string2Seglr
//   Example: "pts={0,0:5,5:20,5},ray=45,ray_len=10,head_size=3"

XYSeglr string2Seglr(const string& str)
{
  XYSeglr null_seglr;
  XYSeglr new_xy_seglr;

  vector<double> xpts;
  vector<double> ypts;

  double ray_angle = 0;
  string rest = str;
  double ray_len = -1;   
  double head_size = -1;
  
  while(rest != "") {
    string left = biteStringX(rest, '=');

    if(left == "pts") {
      string pstr = biteStringX(rest, '}');
      
      // Empty set of points is an error
      if(pstr == "")
	return(null_seglr);

      // Points should begin with an open brace (but discard now)
      if(pstr[0] != '{') 
	return(null_seglr);
      else
	pstr = pstr.substr(1);

      // If more components after pts={}, then it should begin w/ comma
      if(rest != "") {
	if(rest[0] != ',')
	  return(null_seglr);
	else
	  rest = rest.substr(1);
      }

      vector<string> svector = parseString(pstr, ':');
      for(unsigned int i=0; i<svector.size(); i++) {
	string vertex = stripBlankEnds(svector[i]);
	string xstr = biteStringX(vertex, ',');
	string ystr = biteStringX(vertex, ',');
	  
	if(!isNumber(xstr) || !isNumber(ystr))
	  return(null_seglr);
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	xpts.push_back(xval);
	ypts.push_back(yval);
      }
    }
    else if(left == "ray") {
      string right = biteStringX(rest, ',');
      double angle = atof(right.c_str());
      ray_angle = angle;
    }      
    else if(left == "ray_len") {
      string right = biteStringX(rest, ',');
      ray_len = atof(right.c_str());
    }      
    else if(left == "head_size") {
      string right = biteStringX(rest, ',');
      head_size = atof(right.c_str());
    }      
    else {
      string right = biteStringX(rest, ',');
      new_xy_seglr.set_param(left, right);
    }
  }	  				  

  for(unsigned int i=0; i<xpts.size(); i++) 
    new_xy_seglr.addVertex(xpts[i], ypts[i]);
  new_xy_seglr.setRayAngle(ray_angle);
  
  if(ray_len >= 0)
    new_xy_seglr.setRayLen(ray_len);
  if(head_size >= 0)
    new_xy_seglr.setHeadSize(head_size);
  
  return(new_xy_seglr);
}






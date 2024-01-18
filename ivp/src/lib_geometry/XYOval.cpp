/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYOval.cpp                                           */
/*    DATE: Mar 11th 2022                                        */
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
#include "XYOval.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor()

XYOval::XYOval(double x, double y, double rad, double len, double ang)
{
  m_x    = x;
  m_y    = y;
  m_rad  = rad;
  m_len  = len;
  m_ang  = ang;

  m_x_set   = true;
  m_y_set   = true;
  m_ang_set = true;
  
  // Number of significant digits in x,y,rad fields in string spec
  m_sdigits = 2;

  m_draw_degs = 5;

  m_cx1 = 0;
  m_cy1 = 0;
  m_cx2 = 0;
  m_cy2 = 0;
  m_boundary_cache_set = false;
  clearPointCache();
}

//-------------------------------------------------------------
// Procedure: setXY()

void XYOval::setXY(double x, double y)
{
  m_x  = x;
  m_y  = y;
  
  m_x_set = true;
  m_y_set = true;

  m_boundary_cache_set = false;
  clearPointCache();
}

//-------------------------------------------------------------
// Procedure: setRadius()

void XYOval::setRadius(double rad)
{
  if(rad <= 0)
    return;

  m_rad = rad;

  m_boundary_cache_set = false;
  clearPointCache();
}

//-------------------------------------------------------------
// Procedure: setLength()

void XYOval::setLength(double len)
{
  if(len <= 0)
    return;

  m_len = len;

  m_boundary_cache_set = false;
  clearPointCache();
}

//-------------------------------------------------------------
// Procedure: setAngle()

void XYOval::setAngle(double ang)
{
  m_ang     = ang;
  m_ang_set = true;

  m_boundary_cache_set = false;
  clearPointCache();
}


//-------------------------------------------------------------
// Procedure: get_spec()

string XYOval::get_spec(string param) const
{
  string spec;
  
  spec += "x=";
  spec += doubleToStringX(m_x, m_sdigits) + ","; 
  spec += "y=";
  spec += doubleToStringX(m_y, m_sdigits) + ","; 
  spec += "rad=";
  spec += doubleToStringX(m_rad, m_sdigits) + ","; 
  spec += "len=";
  spec += doubleToStringX(m_len, m_sdigits) + ","; 
  spec += "ang=";
  spec += doubleToStringX(m_ang, m_sdigits) + ","; 
  spec += "draw_degs=";
  spec += doubleToStringX(m_draw_degs);

  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);
  
  return(spec);
}

//-------------------------------------------------------------
// Procedure: setPointCache()

void XYOval::setPointCache(double gdegs) 
{
  cout << "setting point cache degs:" << gdegs << endl;
  if(!valid())
    return;
  if(!m_boundary_cache_set)
    return;

  double degs = 5;
  if((gdegs == 2) || (gdegs == 10) || (gdegs == 15))
    degs = gdegs;
  cout << "setting point cache degs(2):" << gdegs << endl;
  
  m_pt_cache.clear();

  double rng_low = -90;
  double rng_hgh = +90;

  for(double deg=rng_low; deg<=rng_hgh; deg+=degs) {
    double new_x, new_y;
    projectPoint(m_ang + deg, m_rad, m_cx1, m_cy1, new_x, new_y);
    m_pt_cache.push_back(new_x);
    m_pt_cache.push_back(new_y);
  }

  for(double deg=rng_low; deg<=rng_hgh; deg+=degs) {
    double new_x, new_y;
    projectPoint(m_ang + 180 + deg, m_rad, m_cx2, m_cy2, new_x, new_y);
    m_pt_cache.push_back(new_x);
    m_pt_cache.push_back(new_y);
  }
}

//-------------------------------------------------------------
// Procedure: setBoundaryCache()

void XYOval::setBoundaryCache()
{
  if(m_boundary_cache_set)
    return;
  if(!valid())
    return;

  double ilen = (m_len - (2 * m_rad)) / 2;
  if(ilen <= 0)
    return;

  XYPolygon rect;
  rect.add_vertex(m_x - m_rad, m_y + ilen);
  rect.add_vertex(m_x - m_rad, m_y - ilen);
  rect.add_vertex(m_x + m_rad, m_y - ilen);
  rect.add_vertex(m_x + m_rad, m_y + ilen);
  rect.rotate(m_ang);
  m_poly = rect;

  projectPoint(m_ang, ilen, m_x, m_y, m_cx1, m_cy1);
  projectPoint(m_ang-180, ilen, m_x, m_y, m_cx2, m_cy2);

  m_xmax = m_cx1 + m_rad;
  if((m_cx2 + m_rad) > m_xmax)
    m_xmax = m_cx2 + m_rad;

  m_ymax = m_cy1 + m_rad;
  if((m_cy2 + m_rad) > m_ymax)
    m_ymax = m_cy2 + m_rad;
  
  m_xmin = m_cx1 - m_rad;
  if((m_cx2 - m_rad) < m_xmin)
    m_xmin = m_cx2 - m_rad;

  m_ymin = m_cy1 - m_rad;
  if((m_cy2 - m_rad) < m_ymin)
    m_ymin = m_cy2 - m_rad;
  
  m_boundary_cache_set = true;
}

//-------------------------------------------------------------
// Procedure: containsPoint()

bool XYOval::containsPoint(double x, double y)
{
  if(!valid())
    return(false);
  setBoundaryCache();

  // First way to pass: if point is in inner rectacle
  if(m_poly.contains(x,y))
    return(true);

  // Second way to pass: if point is in first circle
  double dist1 = hypot(m_cx1-x, m_cy1-y);
  if(dist1 <= m_rad)
    return(true);

  // Third way to pass: if point is in second circle
  double dist2 = hypot(m_cx2-x, m_cy2-y);
  if(dist2 <= m_rad)
    return(true);
  
  return(false);
}

//-------------------------------------------------------------
// Procedure: getCenterPt()

XYPoint XYOval::getCenterPt() 
{
  XYPoint pt(m_x, m_y);
  return(pt);
}

//-------------------------------------------------------------
// Procedure: getEndPt1()

XYPoint XYOval::getEndPt1() 
{
  setBoundaryCache();
  XYPoint pt(m_cx1, m_cy1);
  return(pt);
}

//-------------------------------------------------------------
// Procedure: getEndPt2()

XYPoint XYOval::getEndPt2()
{
  setBoundaryCache();
  XYPoint pt(m_cx2, m_cy2);   
  return(pt);
}

//-------------------------------------------------------------
// Procedure: getRectPoly()

XYPolygon XYOval::getRectPoly()
{
  setBoundaryCache();
  return(m_poly);
}

//-------------------------------------------------------------
// Procedure: getOvalPoly()

XYPolygon XYOval::getOvalPoly(double degs)
{
  setBoundaryCache();
  setPointCache(degs);
  
  XYPolygon null_poly;
  unsigned int pt_size = m_pt_cache.size();
  if((pt_size % 2) != 0)
    return(null_poly);

  XYPolygon oval_poly;

  vector<double> ptx;
  vector<double> pty;
  
  for(unsigned int i=0; i<m_pt_cache.size(); i++) {
    if((i%2) == 0)
      ptx.push_back(m_pt_cache[i]);
    else
      pty.push_back(m_pt_cache[i]);
  }	    
  if(ptx.size() != pty.size())
    return(null_poly);

  for(unsigned int i=0; i<ptx.size(); i++) {
    oval_poly.add_vertex(ptx[i], pty[i]);
  }

  return(oval_poly);
}

//-------------------------------------------------------------
// Procedure: valid()

bool XYOval::valid() const
{
  if(!m_x_set || !m_y_set || !m_ang_set)
    return(false);

  if(m_rad <= 0)
    return(false);

  if(m_len < (2 * m_rad))
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: print()

void XYOval::print()
{
  cout << "x:" << m_x << endl;
  cout << "y:" << m_y << endl;
  cout << "len:" << m_len << endl;
  cout << "rad:" << m_rad << endl;
  cout << "ang:" << m_ang << endl;

  cout << "poly:" << m_poly.get_spec() << endl;
  cout << "cx1:" << m_cx1 << endl;
  cout << "cy1:" << m_cy1 << endl;
  cout << "cx2:" << m_cx2 << endl;
  cout << "cy2:" << m_cy2 << endl;

  cout << "boundary_cache_set:" << boolToString(m_boundary_cache_set) << endl;
  cout << "draw_degs:" << m_draw_degs << endl;
  cout << "pt_cache_size:" << m_pt_cache.size();
}

//-------------------------------------------------------------
// Procedure: stringToOval()

XYOval stringToOval(string str)
{
  XYOval null_oval;
  XYOval oval;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval = atof(value.c_str());
    
    if(param == "x")
      oval.setX(dval);
    else if(param == "y")
      oval.setY(dval);
    else if((param == "rad") && (dval > 0))
      oval.setRadius(dval);
    else if((param == "len") && (dval > 0))
      oval.setLength(dval);
    else if(param == "ang")
      oval.setAngle(dval);
    else if(param == "draw_degs")
      oval.setDrawDegs(dval);
    else
      oval.set_param(param, value);
  }

  if(!oval.valid())
    return(null_oval);

  return(oval);
}



 

/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYArrow.cpp                                          */
/*    DATE: Jan 23rd 2022 (Support visualization of wind dir)    */
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
#include "XYArrow.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.1415926
#endif

//-------------------------------------------------------------
// Procedure: Constructor

XYArrow::XYArrow()
{
  init();
}

//-------------------------------------------------------------
// Procedure: Constructor

XYArrow::XYArrow(double ctr_x, double ctr_y)
{
  init();
  m_ctr_x = ctr_x;
  m_ctr_y = ctr_y;
}

//-------------------------------------------------------------
// Procedure: init()

void XYArrow::init()
{
  XYObject::set_edge_color("off");
  XYObject::set_vertex_color("off");
  XYObject::set_color("fill", "yellow");
  XYObject::set_transparency(0.5);
  
  m_ctr_x = 0;
  m_ctr_y = 0;
  m_base_wid = 5;
  m_base_len = 20;
  m_head_wid = 10;
  m_head_len = 10;
  m_angle = 0;

  m_cache_set = false;
  
  m_bx1 = 0;
  m_by1 = 0;
  m_bx2 = 0;
  m_by2 = 0;
  m_bx3 = 0;
  m_by3 = 0;
  m_bx4 = 0;
  m_by4 = 0;

  m_hx1 = 0;
  m_hy1 = 0;
  m_hx2 = 0;
  m_hy2 = 0;
  m_hx3 = 0;
  m_hy3 = 0;

  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//-------------------------------------------------------------
// Procedure: setSuperLength()
//   Purpose: Given just the length, use default ratios to set
//            the head and base values. 

bool XYArrow::setSuperLength(double superlen)
{
  if(superlen <= 0)
    return(false);

  m_base_len = 0.75 * superlen;
  m_base_wid = 0.25 * m_base_wid;
  m_head_len = 0.25 * superlen;
  m_head_wid = 3 * m_base_wid;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setBaseCenter()

void XYArrow::setBaseCenter(double ctr_x, double ctr_y)
{
  m_ctr_x = ctr_x;
  m_ctr_y = ctr_y;
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: setBaseCenterX()

void XYArrow::setBaseCenterX(double ctr_x)
{
  m_ctr_x = ctr_x;
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: setBaseCenterY()

void XYArrow::setBaseCenterY(double ctr_y)
{
  m_ctr_y = ctr_y;
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: setBase()

bool XYArrow::setBase(double base_wid, double base_len)
{
  if((base_wid <= 0) || (base_len <= 0))
    return(false);

  m_base_wid = base_wid;
  m_base_len = base_len;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setBaseWid()

bool XYArrow::setBaseWid(double base_wid)
{
  if(base_wid <= 0)
    return(false);

  m_base_wid = base_wid;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setBaseLen()

bool XYArrow::setBaseLen(double base_len)
{
  if(base_len <= 0)
    return(false);

  m_base_len = base_len;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHead()

bool XYArrow::setHead(double head_wid, double head_len)
{
  if((head_wid <= 0) || (head_len <= 0))
    return(false);

  m_head_wid = head_wid;
  m_head_len = head_len;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHeadWid()

bool XYArrow::setHeadWid(double head_wid)
{
  if(head_wid <= 0)
    return(false);

  m_head_wid = head_wid;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setHeadLen()

bool XYArrow::setHeadLen(double head_len)
{
  if(head_len <= 0)
    return(false);

  m_head_len = head_len;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: resize()

bool XYArrow::resize(double factor)
{
  if(factor <= 0.01)
    return(false);
  
  m_base_wid *= factor;
  m_base_len *= factor;
  m_head_wid *= factor;
  m_head_len *= factor;

  m_cache_set = false;
  return(true);
}

//-------------------------------------------------------------
// Procedure: modCenterX()

void XYArrow::modCenterX(double delta)
{
  m_ctr_x += delta;
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: modCenterY()

void XYArrow::modCenterY(double delta)
{
  m_ctr_y += delta;
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: setAngle()

void XYArrow::setAngle(double angle)
{
  m_angle = angle360(angle);
  m_cache_set = false;
}

//-------------------------------------------------------------
// Procedure: setPointCache()

void XYArrow::setPointCache()
{
  // ==============================================
  // Part 1: Set Base coordinates
  // ==============================================
  double min_ang = relAng(0, 0, m_base_wid, m_base_len);
  double max_ang = 180 - min_ang;

  double head_ang_rgt = relAng(0, 0, m_head_wid/2, m_base_len/2);
  double head_ang_lft = angle360(-head_ang_rgt);
  head_ang_rgt = angle360(head_ang_rgt + m_angle);
  head_ang_lft = angle360(head_ang_lft + m_angle);
  
  
  double ang1 = angle360(min_ang + m_angle);
  double ang2 = angle360(max_ang + m_angle);
  double ang3 = angle360(min_ang + 180 + m_angle);
  double ang4 = angle360(max_ang + 180 + m_angle);
  double blen = hypot(m_base_wid/2, m_base_len/2);
  
  projectPoint(ang1, blen, m_ctr_x, m_ctr_y, m_bx1, m_by1);
  projectPoint(ang2, blen, m_ctr_x, m_ctr_y, m_bx2, m_by2);
  projectPoint(ang3, blen, m_ctr_x, m_ctr_y, m_bx3, m_by3);
  projectPoint(ang4, blen, m_ctr_x, m_ctr_y, m_bx4, m_by4);

  // ==============================================
  // Part 2: Set Arrow coordinates
  // ==============================================

  // Arrow tip first
  double tipdist = (m_base_len/2 + m_head_len);
  projectPoint(m_angle, tipdist, m_ctr_x, m_ctr_y, m_hx1, m_hy1);

  // Then sides
  double side_dist = hypot(m_base_len/2, m_head_wid/2);
  projectPoint(head_ang_lft, side_dist, m_ctr_x, m_ctr_y, m_hx2, m_hy2);
  projectPoint(head_ang_rgt, side_dist, m_ctr_x, m_ctr_y, m_hx3, m_hy3);

  m_xmax = m_bx1;
  if(m_bx2 > m_xmax)
    m_xmax = m_bx2;
  if(m_bx3 > m_xmax)
    m_xmax = m_bx3;

  if(m_hx1 > m_xmax)
    m_xmax = m_hx1;
  if(m_hx2 > m_xmax)
    m_xmax = m_hx2;
  if(m_hx3 > m_xmax)
    m_xmax = m_hx3;

  m_ymax = m_by1;
  if(m_by2 > m_ymax)
    m_ymax = m_by2;
  if(m_by3 > m_ymax)
    m_ymax = m_by3;

  if(m_hy1 > m_ymax)
    m_ymax = m_hy1;
  if(m_hy2 > m_ymax)
    m_ymax = m_hy2;
  if(m_hy3 > m_ymax)
    m_ymax = m_hy3;
  
  m_xmin = m_bx1;
  if(m_bx2 > m_xmin)
    m_xmin = m_bx2;
  if(m_bx3 > m_xmin)
    m_xmin = m_bx3;

  if(m_hx1 > m_xmin)
    m_xmin = m_hx1;
  if(m_hx2 > m_xmin)
    m_xmin = m_hx2;
  if(m_hx3 > m_xmin)
    m_xmin = m_hx3;

  m_ymin = m_by1;
  if(m_by2 > m_ymin)
    m_ymin = m_by2;
  if(m_by3 > m_ymin)
    m_ymin = m_by3;

  if(m_hy1 > m_ymin)
    m_ymin = m_hy1;
  if(m_hy2 > m_ymin)
    m_ymin = m_hy2;
  if(m_hy3 > m_ymin)
    m_ymin = m_hy3;
  
  m_cache_set = true;
}


//-------------------------------------------------------------
// Procedure: getMinX()

double XYArrow::getMinX()
{
  if(!m_cache_set)
    setPointCache();
  return(m_xmin);
}

//-------------------------------------------------------------
// Procedure: getMaxX()

double XYArrow::getMaxX()
{
  if(!m_cache_set)
    setPointCache();
  return(m_xmax);
}

//-------------------------------------------------------------
// Procedure: getMinY()

double XYArrow::getMinY()
{
  if(!m_cache_set)
    setPointCache();
  return(m_ymin);
}

//-------------------------------------------------------------
// Procedure: getMaxY()

double XYArrow::getMaxY()
{
  if(!m_cache_set)
    setPointCache();
  return(m_ymax);
}

//-------------------------------------------------------------
// Procedure: getHeadCtrX()

double XYArrow::getHeadCtrX()
{
  return((m_hx1 + m_hx2 + m_hx3)/3);
}

//-------------------------------------------------------------
// Procedure: getHeadCtrY()

double XYArrow::getHeadCtrY()
{
  return((m_hy1 + m_hy2 + m_hy3)/3);
}


//-------------------------------------------------------------
// Procedure: getBaseVertices()

vector<double> XYArrow::getBaseVertices()
{
  if(!m_cache_set)
    setPointCache();

  vector<double> rvector;
  rvector.push_back(m_bx1);
  rvector.push_back(m_by1);
  rvector.push_back(m_bx2);
  rvector.push_back(m_by2);
  rvector.push_back(m_bx3);
  rvector.push_back(m_by3);
  rvector.push_back(m_bx4);
  rvector.push_back(m_by4);

  return(rvector);
}


//-------------------------------------------------------------
// Procedure: getHeadVertices()

vector<double> XYArrow::getHeadVertices() 
{
  if(!m_cache_set)
    setPointCache();

  vector<double> rvector;
  rvector.push_back(m_hx1);
  rvector.push_back(m_hy1);
  rvector.push_back(m_hx2);
  rvector.push_back(m_hy2);
  rvector.push_back(m_hx3);
  rvector.push_back(m_hy3);

  return(rvector);
}


//-------------------------------------------------------------
// Procedure: get_spec()

string XYArrow::get_spec(unsigned int vertex_prec) const
{
  string spec;
  spec += "ctrx=" + doubleToStringX(m_ctr_x, vertex_prec);
  spec += ",ctry=" + doubleToStringX(m_ctr_y, vertex_prec);

  spec += ",ang=" + doubleToStringX(m_angle, vertex_prec);
  
  spec += ",bwid=" + doubleToStringX(m_base_wid, vertex_prec);
  spec += ",blen=" + doubleToStringX(m_base_len, vertex_prec);

  spec += ",hwid=" + doubleToStringX(m_head_wid, vertex_prec);
  spec += ",hlen=" + doubleToStringX(m_head_len, vertex_prec);
  
  string obj_spec = XYObject::get_spec();
  if(obj_spec != "") {
    if(spec != "")
      spec += ",";
    spec += obj_spec;
  }

  return(spec);
}


//-------------------------------------------------------------
// Procedure: set_param()

bool XYArrow::set_param(string param, string value)
{
  double dval = atof(value.c_str());
  bool isnum = isNumber(value);

  bool ok = true;
  if((param == "ctrx") && isnum)
    setBaseCenterX(dval);
  else if((param == "ctry") && isnum)
    setBaseCenterY(dval);
  else if((param == "ang") && isnum)
    setAngle(dval);
  else if((param == "bwid") && isnum)
    setBaseWid(dval);
  else if((param == "blen") && isnum)
    setBaseLen(dval);
  else if((param == "hwid") && isnum)
    setHeadWid(dval);
  else if((param == "hlen") && isnum)
    setHeadLen(dval);

  else
    ok = XYObject::set_param(param, value);

  return(ok);
}


//-------------------------------------------------------------
// Procedure: stringToArrow()

XYArrow stringToArrow(string str)
{
  XYArrow null_arrow;
  XYArrow arrow;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    bool ok = arrow.set_param(param, value);

    if(!ok)
      return(null_arrow);
  }

  return(arrow);
}



/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_BearingLine.cpp                                  */
/*    DATE: Feb 12th 2010                                        */
/*    NOTE: A simple behavior for testing dynamic spawning of    */
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

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "BHV_BearingLine.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "XYSegList.h"
#include "XYFormatUtilsPoint.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_BearingLine::BHV_BearingLine(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  m_descriptor = "bhv_bngline";  // variable at superclass level

  // configuration parameters
  m_bearing_pt.set_vertex(0, 0);
  m_line_pct = 50;   // Percentage [0,100]
  m_show_pt  = true;

  setParam("descriptor", "bhv_bngline");
  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_BearingLine::setParam(string param, string val) 
{
  if(param == "bearing_point") { 
    XYPoint bearing_pt = string2Point(val);
    if(bearing_pt.valid()) {
      m_bearing_pt = bearing_pt;
      string label = doubleToString(bearing_pt.x(),0) + ",";
      label += doubleToString(bearing_pt.y(),0);
      m_bearing_pt.set_label(label);
      return(true);
    }
  }
  else if((param == "line_pct") && isNumber(val)) {
    double dval = atof(val.c_str());
    m_line_pct = vclip(dval, 0, 100);
    return(true);
  }
  else if(param == "show_pt")
    return(setBooleanOnString(m_show_pt, val));
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: postErasableObjects

void BHV_BearingLine::postErasableObjects() 
{
  m_seglist.set_active(false);
  postMessage("VIEW_SEGLIST", m_seglist.get_spec());

  m_bearing_pt.set_active(false);
  postMessage("VIEW_POINT", m_bearing_pt.get_spec());
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_BearingLine::onRunState() 
{
  bool ok1, ok2;
  double nav_x, nav_y;
 
  nav_x = getBufferDoubleVal("NAV_X", ok1);
  nav_y = getBufferDoubleVal("NAV_Y", ok2);

  if(!ok1)
    postWMessage("No NAV_X info in the info_buffer");
  if(!ok2)
    postWMessage("No NAV_Y info in the info_buffer");

  double bng_x = m_bearing_pt.x();
  double bng_y = m_bearing_pt.y();
  double dist_to_target = hypot((nav_x-bng_x), (nav_y-bng_y));
  double rel_ang = relAng(nav_x, nav_y, bng_x, bng_y);

  double line_length = (m_line_pct/100.0) * dist_to_target;
  XYPoint end_pt = projectPoint(rel_ang, line_length, nav_x, nav_y);

  m_seglist.clear(); 
  m_seglist.set_active(true);
  m_seglist.add_vertex(nav_x, nav_y);
  m_seglist.add_vertex(end_pt);
  m_seglist.set_label(m_us_name + "_" + m_descriptor);

  postMessage("VIEW_SEGLIST", m_seglist.get_spec());

  if(m_show_pt) 
    m_bearing_pt.set_active(true);
  else
    m_bearing_pt.set_active(false);
  postMessage("VIEW_POINT", m_bearing_pt.get_spec());
  
  return(0);
}












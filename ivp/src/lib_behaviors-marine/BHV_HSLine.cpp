/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_HSLine.cpp                                       */
/*    DATE: Feb 8th 2009                                         */
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
#include "BHV_HSLine.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "XYSegList.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HSLine::BHV_HSLine(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  m_descriptor = "bhv_hsline";  // variable at superclass level

  // configuration parameters
  m_time_on_leg = 60;
  m_max_des_speed_observed = -1;

  setParam("descriptor", "bhv_hsline");
  addInfoVars("NAV_X, NAV_Y, DESIRED_HEADING, DESIRED_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HSLine::setParam(string param, string val) 
{
  if(param == "time_on_leg") { 
    double dval = atof(val.c_str());
    if((dval <= 0) || (!isNumber(val)))
      return(false);
    m_time_on_leg = dval;
    return(true);
  }

  return(false);
}


//-----------------------------------------------------------
// Procedure: postErasableSegList()

void BHV_HSLine::postErasableSegList()
{
  if(m_seglist.size() > 0)
    postMessage("VIEW_SEGLIST", m_seglist.get_spec("active=false"));
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HSLine::onRunState() 
{
  bool ok1, ok2, ok3, ok4;
  double des_hdg, des_spd, nav_x, nav_y;
 
  des_hdg = getBufferDoubleVal("DESIRED_HEADING",  ok1);
  des_spd = getBufferDoubleVal("DESIRED_SPEED",    ok2);
  nav_x   = getBufferDoubleVal("NAV_X",            ok3);
  nav_y   = getBufferDoubleVal("NAV_Y",            ok4);

  if(!ok1)
    postWMessage("No DESIRED_HEADING info in the info_buffer");
  if(!ok2)
    postWMessage("No DESIRED_SPEED info in the info_buffer");
  if(!ok3)
    postWMessage("No NAV_X info in the info_buffer");
  if(!ok4)
    postWMessage("No NAV_Y info in the info_buffer");

  // To dramatize the desired speed, keep track of the maximum
  // desired speed observed and determine the current desired
  // speed in relation. Then scale down the resported desired 
  // speed.
  if(des_spd > m_max_des_speed_observed)
    m_max_des_speed_observed = des_spd;
  
  double pct = 1.0;
  if(m_max_des_speed_observed > 0)
    pct = des_spd / m_max_des_speed_observed;

  des_spd *= pct; 
  
  double x1 = nav_x;
  double y1 = nav_y;
  double x2,y2;
  double dist = des_spd * m_time_on_leg;
  
  projectPoint(des_hdg, dist, x1, y1, x2, y2);

  m_seglist.clear();
  m_seglist.add_vertex(x1, y1);
  m_seglist.add_vertex(x2, y2);

  m_seglist.set_label(m_us_name + "_" + m_descriptor);
  string spec = m_seglist.get_spec("active=true");

  postMessage("VIEW_SEGLIST", spec);

  return(0);
}











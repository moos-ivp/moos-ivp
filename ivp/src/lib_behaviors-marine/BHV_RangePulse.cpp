/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_RangePulse.cpp                                   */
/*    DATE: Feb 7th 2011                                         */
/*    NOTE: A simple behavior for testing range sensor sims.     */
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

BHV_RangePulse::BHV_RangePulse(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  m_descriptor = "bhv_bngline";  // variable at superclass level

  // configuration parameters
  m_bearing_pt.set_vertex(0, 0);
  m_line_pct = 50;   // Percentage [0,100]
  m_show_pt  = true;

  setParam("descriptor", "bhv_rngpulse");
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("RANGE_PULSE"+m_us_name);
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_RangePulse::setParam(string param, string val) 
{
  return(true);
}

//-----------------------------------------------------------
// Procedure: setParamComplete

void BHV_RangePulse::onSetParamComplete()
{
  addInfoVars("RANGE_PULSE"+m_us_name);
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_RangePulse::onRunState() 
{
  bool okr;
  string varname = "RANGE_REPORT" + m_us_name;
  vector<string> rpts = getBufferStringVector(varname, okr);

  unsigned int i, vsize = rpts.size();
  if(vsize == 0)
    return(0);

  bool okx, oky;
  double nav_x, nav_y;
 
  nav_x = getBufferDoubleVal("NAV_X", okx);
  nav_y = getBufferDoubleVal("NAV_Y", oky);

  if(!okx)
    postWMessage("No NAV_X info in the info_buffer");
  if(!oky)
    postWMessage("No NAV_Y info in the info_buffer");

  for(i=0; i<vsize; i++) {
    string pulse = "x=" + doubleToString(nav_x,0);
    pulse += ",y=" + doubleToString(nav_y,0);
    pulse += ",radius=25,duration=20,label=one";
    postMessage("VIEW_RANGE_PULSE", pulse);
  }
  
  return(0);
}











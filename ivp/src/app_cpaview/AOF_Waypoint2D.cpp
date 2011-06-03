/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Waypoint2D.cpp                                   */
/*    DATE: Jan 4th 2006                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <cmath> 
#include <cassert> 
#include "AOF_Waypoint2D.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Waypoint2D::AOF_Waypoint2D(IvPDomain g_domain)
  : AOF(g_domain)
{
  int pts_course = m_domain.getVarPoints("course");
  int pts_speed  = m_domain.getVarPoints("speed");
  assert((pts_course > 0) && (pts_speed > 0));
	 
  osLAT = osLON = ptLAT = ptLON = desired_speed = current_angle = 0;
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Waypoint2D::evalBox(const IvPBox *b) const
{
  double evalCRS, evalSPD;

  m_domain.getVal("course", b->pt(0,0), evalCRS);
  m_domain.getVal("speed",  b->pt(1,0), evalSPD);

  double crs_diff = fabs(evalCRS - current_angle);
  if(crs_diff > 180)
    crs_diff = 360.0 - crs_diff;

  double spd_diff = fabs(desired_speed - evalSPD);

  return((180.0 - crs_diff) + (20*(5.0 - spd_diff)));
  return((180.0 - crs_diff) + (100*(5.0 - spd_diff)));
  return(180 - crs_diff);
}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Waypoint2D::setParam(const string& param, double param_val)
{
  if(param == "oslat")
    osLAT = param_val;
  else if(param == "oslon")
    osLON = param_val;
  else if(param == "ptlat")
    ptLAT = param_val;
  else if(param == "ptlon")
    ptLON = param_val;
  else if(param == "speed")
    desired_speed = param_val;
  else
    return(false);
  return(true);
}







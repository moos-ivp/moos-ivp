/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Waypoint2D.cpp                                   */
/*    DATE: Jan 4th 2006                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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








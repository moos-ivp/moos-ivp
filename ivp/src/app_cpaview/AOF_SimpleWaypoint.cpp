/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SimpleWaypoint.cpp                               */
/*    DATE: Feb 22th 2009                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "AOF_SimpleWaypoint.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_SimpleWaypoint::AOF_SimpleWaypoint(IvPDomain g_domain) : AOF(g_domain)
{
  // Unitialized cache values for later use in evalBox calls
  m_min_speed    = 0;
  m_max_speed    = 0;
  m_angle_to_wpt = 0;

  // Initialization parameters
  m_osx         = 0;  // ownship x-position 
  m_osy         = 0;  // ownship y-position
  m_ptx         = 0;  // waypoint x-position 
  m_pty         = 0;  // waypoint y-position
  m_desired_spd = 0;   

  // Initialization parameter flags
  m_osy_set         = false;
  m_osx_set         = false;
  m_pty_set         = false;
  m_ptx_set         = false;
  m_desired_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_SimpleWaypoint::setParam(const string& param, double param_val)
{
  if(param == "osy") {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if(param == "osx") {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if(param == "pty") {
    m_pty = param_val;
    m_pty_set = true;
    return(true);
  }
  else if(param == "ptx") {
    m_ptx = param_val;
    m_ptx_set = true;
    return(true);
  }
  else if(param == "desired_speed") {
    m_desired_spd = param_val;
    m_desired_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_SimpleWaypoint::initialize()
{
  // Check for failure conditions
  if(!m_osy_set || !m_osx_set || !m_pty_set || !m_ptx_set || !m_desired_spd_set)
    return(false);
  if(!m_domain.hasDomain("speed") || !m_domain.hasDomain("course"))
    return(false);

  // Initialize local variables to cache intermediate calculations 
  m_angle_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);;
  m_min_speed = m_domain.getVarLow("speed");
  m_max_speed = m_domain.getVarHigh("speed");

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalPoint
//   Purpose: Evaluate a candidate point in the decision space

double AOF_SimpleWaypoint::evalPoint(const vector<double>& point) const
{
  // Determine the course and speed being evaluated
  double eval_crs = extract("course", point);
  double eval_spd = extract("speed", point);

  // Calculate the first score, score_roc, based on rate of closure
  double angle_diff      = angle360(eval_crs - m_angle_to_wpt);
  double rad_diff        = degToRadians(angle_diff);
  double rate_of_closure = cos(rad_diff) * eval_spd;
  
  double roc_range = 2 * m_max_speed;
  double roc_diff = (m_desired_spd - rate_of_closure);
  if(roc_diff < 0) 
    roc_diff *= -0.5; // flip the sign, cut the penalty for being over
  if(roc_diff > roc_range)
    roc_diff = roc_range;
  
  double pct = (roc_diff / roc_range);
  double score_roc = (1.0 - pct) * 100;

  // Calculate the second score, score_rod, based on rate of detour
  double angle_180 = angle180(angle_diff);
  if(angle_180 < 0)
    angle_180 *= -1;
  if(eval_spd < 0)
    eval_spd = 0;
  double rate_of_detour  = (angle_180 * eval_spd);
  
  double rod_range = (m_max_speed * 180);
  double rod_pct   = (rate_of_detour / rod_range);
  double score_rod = (1.0 - rod_pct) * 100;

  // Calculate the third score, score_spd, based on ideal speed.
  double spd_range = m_max_speed - m_desired_spd;
  if((m_desired_spd - m_min_speed) > spd_range)
    spd_range = m_desired_spd - m_min_speed;
  double spd_diff = m_desired_spd - eval_spd;
  if(spd_diff < 0)
    spd_diff *= -1;
  if(spd_diff > spd_range)
    spd_diff = spd_range;
  double spd_pct   = (spd_diff / spd_range);
  double score_spd = (1.0 - spd_pct) * 100;

  // CALCULATE THE COMBINED SCORE 
  double combined_score = 0;
  combined_score += (0.75 * score_roc);
  combined_score += (0.2 * score_rod);
  combined_score += (0.05 * score_spd);
  
  return(combined_score);
}


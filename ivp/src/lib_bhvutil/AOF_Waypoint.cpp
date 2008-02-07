/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Waypoint.cpp                                     */
/*    DATE: Sep 28th 2006                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "AOF_Waypoint.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Waypoint::AOF_Waypoint(IvPDomain g_domain) 
  : AOF(g_domain)
{
  // Cached values for more efficient evalBox calls
  m_crs_ix = g_domain.getIndex("course");
  m_spd_ix = g_domain.getIndex("speed");

  // Unitialized cache values for later use in evalBox calls
  m_max_speed    = 0;
  m_angle_to_wpt = 0;

  // Initialization parameters
  m_osx         = 0;
  m_osy         = 0;
  m_ptx         = 0;
  m_pty         = 0;
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

bool AOF_Waypoint::setParam(const string& param, double param_val)
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

bool AOF_Waypoint::initialize()
{
  if((m_crs_ix==-1)||(m_spd_ix==-1))
    return(false);

  if(!m_osy_set || !m_osx_set || !m_pty_set || !m_ptx_set)
    return(false);

  if(!m_desired_spd_set)
    return(false);

  m_angle_to_wpt = relAng(m_osx, m_osy, m_ptx, m_pty);;

  m_max_speed = m_domain.getVarHigh(m_spd_ix);

  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//                                                                

double AOF_Waypoint::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);
  
  // CALCULATE THE FIRST SCORE - SCORE_ROC

  double angle_diff      = angle360(eval_crs - m_angle_to_wpt);
  double rad_diff        = degToRadians(angle_diff);
  double rate_of_closure = cos(rad_diff) * eval_spd;
  
  double roc_range = 2 * m_max_speed;
  double pct = (rate_of_closure - (-m_max_speed)) / roc_range;
  double score_ROC = pct * pct * 100;


  // CALCULATE THE SECOND SCORE - SCORE_SPD
  
  double diff_spd = eval_spd - m_desired_spd;
  
  if(diff_spd < 0)
    diff_spd = 0;

  double val = 2 - diff_spd;
  if(val < 0)
    val = 0;
  val *= 10;
  
  double score_SPD = val;

  //if(rate_of_closure < 0)
  // score_SPD = 0;

  return(score_ROC + score_SPD);

  double rate_of_detour  = sin(rad_diff) * eval_spd;
  if(rate_of_detour < 0)
    rate_of_detour *= -1;

  double diff = rate_of_closure - m_desired_spd;
  if(diff < 0) 
    diff *= -1;
  
  double mod_val1 = (100 - (diff*100));
#if 0
  if(mod_val1 < 0)
    mod_val1 = 0;
#endif

  double mod_val2 = 50 * rate_of_detour;

  double mod_val = mod_val1 - mod_val2;

  //  if(mod_val < 0)
  // mod_val = 0;
  
  
  return(mod_val);
}


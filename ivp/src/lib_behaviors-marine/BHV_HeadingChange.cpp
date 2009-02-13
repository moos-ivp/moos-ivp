/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HeadingChange.cpp                                */
/*    DATE: Feb 11th 2009                                        */
/*****************************************************************/

#include <iostream>
#include "BHV_HeadingChange.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HeadingChange::BHV_HeadingChange(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "hdg_change");
  m_domain = subDomain(m_domain, "course,speed");

  m_heading_delta = 20;
  m_turn_type     = "starboard";

  m_state           = "idle";
  m_start_heading   = 0;
  m_start_xpos      = 0;
  m_start_ypos      = 0;
  m_start_des_speed = 0;
  m_heading_goal    = 0;
  m_turn_speed      = -1;
  m_completions     = 0;

  addInfoVars("NAV_HEADING, NAV_X, NAV_Y, DESIRED_HEADING");
  addInfoVars("DESIRED_SPEED, NAV_SPEED");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HeadingChange::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "heading_delta") {
    double dval = atof(val.c_str());
    if((dval < 1) || (dval >= 360) || (!isNumber(val)))
      return(false);
    m_heading_delta = dval;
    return(true);
  }
  else if(param == "turn_speed") {
    if(tolower(val) == "current") {
      m_turn_speed = -1;
      return(true);
    }
    double dval = atof(val.c_str());
    if(!isNumber(val) || (dval <= 0))
      return(false);
    m_turn_speed = dval;
    return(true);
  }
  else if(param == "turn_type") {
    val = tolower(val);
    if((val == "port") || (val == "starboard"))
      m_turn_type = val;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_HeadingChange::onIdleState()
{
  m_state = "idle";
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HeadingChange::onRunState() 
{
  bool ok1, ok2, ok3, ok4, ok5;
  m_curr_time      = getBufferCurrTime();
  m_curr_heading   = getBufferDoubleVal("NAV_HEADING", ok1);
  m_curr_speed     = getBufferDoubleVal("NAV_SPEED", ok2);
  m_curr_des_speed = getBufferDoubleVal("DESIRED_SPEED", ok3);
  m_curr_xpos      = getBufferDoubleVal("NAV_X", ok4);
  m_curr_ypos      = getBufferDoubleVal("NAV_Y", ok5);
  
  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5) {
    postWMessage("Problem retrieving info from info_buffer.");
    return(0);
  }
  
  if(m_state == "idle") {
    if(m_turn_speed == -1)
      m_turn_speed = m_curr_speed;
    m_start_heading   = m_curr_heading;
    m_start_xpos      = m_curr_xpos;
    m_start_ypos      = m_curr_ypos;
    m_start_des_speed = m_curr_des_speed;
    m_state           = "running";
    m_heading_goal    = m_curr_heading + m_heading_delta;
    // Cannot infer the sign based on port/starboard due to wraparound
    m_sign_positive   = (m_curr_heading > m_heading_goal);
  }
  else {
    bool tcomplete = checkForTurnCompletion();
    if(tcomplete) {
      m_completions++;
      postMessage("TURNS", m_completions);
      setComplete();
      m_state = "idle";
      return(0);
    }
  }
  
  double peak_width = 30;
  double summit_delta = 50;

  ZAIC_PEAK spd_zaic(m_domain, "speed");
  spd_zaic.setSummit(m_turn_speed);
  spd_zaic.setValueWrap(false);
  spd_zaic.setPeakWidth(0.4);
  spd_zaic.setBaseWidth(0.6);
  spd_zaic.setSummitDelta(50);
  IvPFunction *spd_ipf = spd_zaic.extractOF();

  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_heading_goal);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(20);
  crs_zaic.setBaseWidth(160);
  crs_zaic.setSummitDelta(50);

  IvPFunction *crs_ipf = crs_zaic.extractOF();

  OF_Coupler coupler;
  IvPFunction *ipf = coupler.couple(crs_ipf, spd_ipf);

  postMessage("HEADING_GOAL", m_heading_goal);

  m_prev_heading = m_curr_heading;
  return(ipf);
}

//-----------------------------------------------------------
// Procedure: checkForTurnCompletion

bool BHV_HeadingChange::checkForTurnCompletion() 
{
#if 0
  if(m_turn_type == "starboard") {   // clockwise
    if(m_curr_heading < m_prev_heading)
      m_sign_positive = !m_sign_positive;
  }
  else if(m_turn_type == "port") {   // counter-clockwise
    if(m_curr_heading < m_prev_heading)
      m_sign_positive = !m_sign_positive;
  }
#endif

  bool   curr_sign_positive = (m_curr_heading > m_heading_goal);
  double curr_hdg_delta = (m_curr_heading - m_heading_goal);
  curr_hdg_delta = angle180(curr_hdg_delta);
  if(curr_hdg_delta < 0)
    curr_hdg_delta *= -1;

  cout << "HeadingDelta: " << curr_hdg_delta << endl;
  postMessage("CURR_HDG_DELTA", curr_hdg_delta);
  postBoolMessage("M_SIGN_POS", m_sign_positive);
  postBoolMessage("CURR_SIGN_POS", m_sign_positive);

  if(curr_sign_positive != m_sign_positive)
    return(true);
  else if(curr_hdg_delta <= 1.0)
    return(true);
  else
    return(false);
}

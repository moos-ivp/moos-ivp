/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_ZigZag.cpp                                       */
/*    DATE: Sep 16th 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "VarDataPairUtils.h"
#include "BHV_ZigZag.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "ZAIC_SPD.h"
#include "MacroUtils.h"
#include "OF_Coupler.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

BHV_ZigZag::BHV_ZigZag(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "zig");
  
  m_domain = subDomain(m_domain, "course,speed");
  
  // Initialize state variables
  m_state   = "stem";
  m_req_hdg = 0;
  m_set_hdg = 0;

  m_zig_count = 0;
  
  // Initialize config variables
  m_stem_hdg = 0;
  m_stem_spd = 0;

  m_hdg_thresh = 0;

  m_draw_set_hdg = true;
  m_draw_req_hdg = true;
  
  m_zig_first = "port";
  m_stale_nav_thresh = 5;

  m_stem_on_active = false;
  m_fierce_zigging = false;

  m_max_zig_legs = 2;
  
  m_hint_set_hdg_color = "white";
  m_hint_req_hdg_color = "yellow";
  
  m_zig_angle = 0;
  m_zig_angle_fierce = -1;

  
  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigZag::setParam(string param, string value) 
{
  if(IvPBehavior::setParam(param, value))
    return(true);
  
  bool handled = true;
  if(param == "speed")
    handled = setNonNegDoubleOnString(m_stem_spd, value);
  else if(param == "stem_hdg") {
    handled = setDoubleOnString(m_stem_hdg, value);
    if(handled)
      m_stem_hdg = angle360(m_stem_hdg);
  }  
  else if(param == "stem_on_active")
    handled = setBooleanOnString(m_stem_on_active, value);
  else if(param == "hdg_thresh")
    handled = setNonNegDoubleOnString(m_hdg_thresh, value);
  else if(param == "fierce_zigging")
    handled = setBooleanOnString(m_fierce_zigging, value);
  else if(param == "zig_angle")
    handled = handleConfigZigAngle(value);
  else if(param == "zig_first")
    handled = handleConfigZigFirst(value);
  else if(param == "max_zig_legs")
    handled = setUIntOnString(m_max_zig_legs, value);
  else if(param == "max_zig_zags")
    handled = handleConfigZigZags(value);
  else if(param == "delta_heading" || (param == "zig_angle_fierce"))
    handled = handleConfigZigAngleFierce(value);
  //else if(param == "end_solo_flag")
  //  handled = addVarDataPairOnString(m_end_solo_flag, value);

  else if(param == "visual_hints")  {
    vector<string> svector = parseStringQ(value, ',');
    for(unsigned int i=0; i<svector.size(); i++) 
      handled = handled && handleConfigVisualHint(svector[i]);
  }
  else
    handled = false;

  return(handled);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigZags()

bool BHV_ZigZag::handleConfigZigZags(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 1)
    return(false);

  unsigned int max_zig_zags = (unsigned int)(dval);

  m_max_zig_legs = 2 * max_zig_zags;

  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigFirst()
//      Note: Possible setting: port or star
//      Note: Posssble given values:
//            port, star, left, right, starboar

bool BHV_ZigZag::handleConfigZigFirst(string str)
{
  str = tolower(str);

  if(str == "starboard")
    str = "star";
  else if(str == "right")
    str = "star";
  else if(str == "left")
    str = "port";

  if((str == "port") || (str == "star"))
    m_zig_first = str;
  else 
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigAngle()

bool BHV_ZigZag::handleConfigZigAngle(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if((dval < 1) || (dval > 75))
    return(false);
  
  m_zig_angle = dval;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleConfigZigAngleFierce()

bool BHV_ZigZag::handleConfigZigAngleFierce(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if((dval < 1) || (dval > 75))
    return(false);
  
  m_zig_angle_fierce = dval;
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateSetHdg()

void BHV_ZigZag::updateSetHdg()
{
  // ========================================================
  // Part 1: If in stem state, pick a side and set a heading 
  // ========================================================
  if(m_state == "stem") {
    if(m_zig_first == "port") {
      setState("port");
      m_set_hdg = angle360(m_stem_hdg - m_zig_angle);
    }
    else {
      setState("star");
      m_set_hdg = angle360(m_stem_hdg + m_zig_angle);
    }
    return;
  }

  // ========================================================
  // Part 2: Check if turn is still in progress
  // ========================================================
  double diff = angleDiff(m_osh, m_set_hdg);
  bool turn_complete = false;
  if(m_state == "port") {
    if(diff < m_hdg_thresh)
      turn_complete = true;
    else if(!containsAngle(m_osh, m_osh-170, m_set_hdg))
      turn_complete = true;
  }
  else {
    if(diff < m_hdg_thresh)
      turn_complete = true;
    else if(!containsAngle(m_osh, m_osh+170, m_set_hdg))
      turn_complete = true;
  }

  if(!turn_complete)
    return;
  
  // ========================================================
  // Part 3: Achieved set_hdg so change state
  // ========================================================
  if(m_state == "port") {
    setState("star");
    m_set_hdg = angle360(m_stem_hdg + m_zig_angle);
    m_zig_count++;
  }
  else if(m_state == "star") {
    setState("port");
    m_set_hdg = angle360(m_stem_hdg - m_zig_angle);
    m_zig_count++;
  }
}

//-----------------------------------------------------------
// Procedure: updateReqHdg()

void BHV_ZigZag::updateReqHdg()
{
  if(!m_fierce_zigging)
    m_req_hdg = m_set_hdg;

  else {
    double delta_hdg = m_zig_angle;
    if(m_zig_angle_fierce > 0)
      delta_hdg = m_zig_angle_fierce;
    
    if(m_state == "port") 
      m_req_hdg = m_osh - delta_hdg;
    else if(m_state == "star") 
      m_req_hdg = m_osh + delta_hdg;
    else if(m_state == "stem") 
      m_req_hdg = m_stem_hdg;
  }

  m_req_hdg = angle360(m_req_hdg);
}

//-----------------------------------------------------------
// Procedure: buildOF()

IvPFunction *BHV_ZigZag::buildOF() 
{
  IvPFunction *ipf = 0;  
  // Sanity Check
  if(m_state == "off")
    return(0);
  
  //===================================================
  // Part 1: Build the Speed ZAIC
  //===================================================
  IvPFunction *spd_ipf = 0;  

  ZAIC_SPD spd_zaic(m_domain, "speed");
  spd_zaic.setParams(m_stem_spd, 0.1, m_stem_spd+0.4, 85, 20);  

  spd_ipf = spd_zaic.extractIvPFunction();
  if(!spd_ipf)
    postWMessage("Failure on the SPD ZAIC component");

  //===================================================
  // Part 2: Build the Heading ZAIC
  //===================================================
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(m_req_hdg);
  crs_zaic.setBaseWidth(180);
  crs_zaic.setValueWrap(true);
  
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction(false);  
  if(!crs_ipf) 
    postWMessage("Failure on the CRS ZAIC");

  OF_Coupler coupler;
  ipf = coupler.couple(crs_ipf, spd_ipf, 50, 50);
  if(!ipf)
    postWMessage("Failure on the CRS_SPD COUPLER");

  ipf->getPDMap()->normalize(0,100);
  ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunState()

IvPFunction *BHV_ZigZag::onRunState() 
{
  if(!updateOSPos() || !updateOSHdg() || !updateOSSpd())
    return(0);

  updateSetHdg();
  updateReqHdg();

  if(m_zig_count > m_max_zig_legs) {
    setComplete();
    if(m_perpetual) {
      m_zig_count = 0;
      m_state = "stem";
    }
    return(0);
  }
  
  // Part 2: Generate the IvP function 
  IvPFunction *ipf = buildOF();

  // Part 3: Apply the relevance and priority weight
  if(ipf) 
    ipf->setPWT(m_priority_wt);
  else 
    postMessage("ZIG_DEBUG", "Unable to build IvP Function");    

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState()

void BHV_ZigZag::onRunToIdleState()
{
  eraseSetHdgLine();
  eraseReqHdgLine();

  m_odometer.reset();  
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: onIdleToRunState()

void BHV_ZigZag::onIdleToRunState()
{
  if(m_stem_on_active) {
    if(updateOSHdg())
      m_stem_hdg = m_osh;
  }

  m_odometer.reset();  
  setState("stem");
}

//-----------------------------------------------------------
// Procedure: updateOSPos()
//   Returns: true if NAV_X/Y info is found and not stale
//            false otherwise

bool BHV_ZigZag::updateOSPos(string fail_action) 
{
  bool   ok_osx = true;
  bool   ok_osy = true;
  double new_osx = getBufferDoubleVal("NAV_X", ok_osx);
  double new_osy = getBufferDoubleVal("NAV_Y", ok_osy);
  double age_osx = getBufferTimeVal("NAV_X");
  double age_osy = getBufferTimeVal("NAV_Y");

  bool all_ok = true;
  if(!ok_osy || !ok_osy)
    all_ok = false;
  if((age_osx > m_stale_nav_thresh) ||
     (age_osy > m_stale_nav_thresh))
    all_ok = false;

  if(!all_ok) {
    if(fail_action == "err")
      postEMessage("ownship NAV_X/Y not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_X/Y not found or stale.");
    return(false);
  }

  m_osx = new_osx;
  m_osy = new_osy;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateOSHdg()
//   Returns: true if NAV_HEADING info is found and not stale
//            false otherwise

bool BHV_ZigZag::updateOSHdg(string fail_action) 
{
  bool   ok_osh = true;
  double new_osh = getBufferDoubleVal("NAV_HEADING", ok_osh);
  double age_osh = getBufferTimeVal("NAV_HEADING");

  if(!ok_osh || (age_osh > m_stale_nav_thresh)) {
    if(fail_action == "err")
      postEMessage("ownship NAV_HEADING not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_HEADING not found or stale.");
    return(false);
  }

  m_osh = new_osh;
  return(true);
}

//-----------------------------------------------------------
// Procedure: updateOSSpd()
//   Returns: true: if NAV_SPEED info is found and not stale
//            false: otherwise

bool BHV_ZigZag::updateOSSpd(string fail_action) 
{
  bool   ok_osv  = true;
  double new_osv = getBufferDoubleVal("NAV_SPEED", ok_osv);
  double age_osv = getBufferTimeVal("NAV_SPEED");
  
  if(!ok_osv || (age_osv > m_stale_nav_thresh)) {
    if(fail_action == "err")
      postEMessage("ownship NAV_SPEED not found or stale.");
    else if(fail_action == "warn")
      postWMessage("ownship NAV_SPEED not found or stale.");
    return(false);
  }

  m_osv = new_osv;
  return(true);

}


//-----------------------------------------------------------
// Procedure: updateOdometer()

bool BHV_ZigZag::updateOdometer()
{
  m_odometer.setX(m_osx);
  m_odometer.setY(m_osy);
   
  m_odometer.updateDistance();

  return(true);
}

//-----------------------------------------------------------
// Procedure: setState()

bool BHV_ZigZag::setState(std::string str)
{
  str = tolower(str);
  if((str != "stem") && (str != "port") && (str != "star"))
    return(false);
  
  m_state = str;

  return(true);
}

//------------------------------------------------------------
// Procedure: postSetHdgLine()
//   Purpose: Draw the heading of the desired zig leg

void BHV_ZigZag::postSetHdgLine()
{
  double x2,y2;
  projectPoint(m_set_hdg, 20, m_osx, m_osy, x2, y2);
  
  XYSegList segl;
  segl.add_vertex(m_osx, m_osy);
  segl.add_vertex(x2, y2);

  segl.set_label("zig_set_" + m_us_name);
  segl.set_label_color("off");
  segl.set_edge_color(m_hint_set_hdg_color);
  segl.set_edge_size(1);
  segl.set_duration(5);
  string spec = segl.get_spec();
  postMessage("VIEW_SEGLIST", spec);
}

//------------------------------------------------------------
// Procedure: postReqHdgLine()
//   Purpose: Draw the heading of the desired zig leg

void BHV_ZigZag::postReqHdgLine()
{
  double x2,y2;
  projectPoint(m_req_hdg, 20, m_osx, m_osy, x2, y2);
  
  XYSegList segl;
  segl.add_vertex(m_osx, m_osy);
  segl.add_vertex(x2, y2);

  segl.set_label("zig_req_" + m_us_name);
  segl.set_label_color("off");
  segl.set_edge_color(m_hint_req_hdg_color);
  segl.set_edge_size(1);
  segl.set_duration(5);
  string spec = segl.get_spec();
  postMessage("VIEW_SEGLIST", spec);
}

//------------------------------------------------------------
// Procedure: eraseSetHdgLine()
//   Purpose: Erase the heading of the desired zig leg

void BHV_ZigZag::eraseSetHdgLine()
{
  XYSegList segl;
  segl.set_label("zig_set_" + m_us_name);
  postMessage("VIEW_SEGLIST", segl.get_spec_inactive());
}

//------------------------------------------------------------
// Procedure: eraseReqHdgLine()
//   Purpose: Erase the heading of the desired zig leg

void BHV_ZigZag::eraseReqHdgLine()
{
  XYSegList segl;
  segl.set_label("zig_req_" + m_us_name);
  postMessage("VIEW_SEGLIST", segl.get_spec_inactive());
}

//-----------------------------------------------------------
// Procedure: handleConfigVisualHint()

bool BHV_ZigZag::handleConfigVisualHint(string hint)
{
  string param = tolower(biteStringX(hint, '='));
  string value = hint;

  bool handled = false;
  if(param == "draw_set_hdg")
    handled = setBooleanOnString(m_draw_set_hdg, value);
  else if(param == "draw_req_hdg")
    handled = setBooleanOnString(m_draw_req_hdg, value);
  else if(param == "set_hdg_color")
    handled = setColorOnString(m_hint_set_hdg_color, value);
  else if((param == "req_hdg_color") && isColor(value))
    handled = setColorOnString(m_hint_req_hdg_color, value);

  return(handled);  
}


//-----------------------------------------------------------
// Procedure: expandMacros() 

string BHV_ZigZag::expandMacros(string sdata)
{
#if 0
  sdata = IvPBehavior::expandMacros(sdata);

  sdata = macroExpand(sdata, "REGION", m_curr_region);
  sdata = macroExpand(sdata, "NEXT_REGION", m_next_region);
#endif
  
  return(sdata);
}


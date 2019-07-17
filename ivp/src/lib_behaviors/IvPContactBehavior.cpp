/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPContactBehavior.cpp                               */
/*    DATE: Apr 3rd 2010 Separated/generalized from indiv. bhvs  */
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

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "AOF_AvoidCollision.h"
#include "IvPContactBehavior.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "CPA_Utils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IvPContactBehavior::IvPContactBehavior(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  // Behavior will only post a warning - not error - if no contact info
  m_on_no_contact_ok  = true;

  m_extrapolate  = true;
  m_decay_start  = 15;
  m_decay_end    = 30;
  m_extrapolator.setDecay(m_decay_start, m_decay_end);
  m_time_on_leg  = 60;

  m_complete_after_retired = -1;  // seconds. -1 means don't use.
  
  // Initialize state variables
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;
  m_osv = 0;
  m_cnx = 0;
  m_cny = 0;
  m_cnh = 0;
  m_cnv = 0;
  m_cnutc = 0;

  m_cn_not_retired_tstamp = -1;
  m_cn_retired = false;
  
  m_contact_range = 0;
  m_range_gamma   = 0;
  m_relevance     = 0;

  m_bearing_rate  = 0;
  m_contact_rate  = 0;

  m_range_gamma   = 0;
  m_range_epsilon = 0;
  
  m_bearing_line_show = false;
  m_bearing_line_info = "relevance";

  m_os_fore_of_cn = false;
  m_os_aft_of_cn  = false;
  m_os_port_of_cn = false;
  m_os_starboard_of_cn = false;

  m_cn_fore_of_os = false;
  m_cn_aft_of_os  = false;
  m_cn_port_of_os = false;
  m_cn_starboard_of_os = false;
  
  m_cn_spd_in_os_pos = 0;

  m_os_cn_rel_bng = 0;
  m_cn_os_rel_bng = 0;

  m_os_cn_abs_bng = 0;
  
  m_rate_of_closure = 0;

  m_os_passes_cn = false;
  m_os_passes_cn_port = false;
  m_os_passes_cn_star = false;
  
  m_cn_passes_os = false;
  m_cn_passes_os_port = false;
  m_cn_passes_os_star = false;
  
  m_os_crosses_cn_stern = false;
  m_os_crosses_cn_bow   = false;
  m_os_crosses_cn_bow_or_stern = false;
  m_os_crosses_cn_bow_dist = 0;

  m_cn_crosses_os_bow = false;
  m_cn_crosses_os_stern = false;
  m_cn_crosses_os_bow_or_stern = false;
  m_cn_crosses_os_bow_dist = 0;

  m_os_curr_cpa_dist = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool IvPContactBehavior::setParam(string param, string param_val) 
{
  if(IvPBehavior::setParam(param, param_val))
    return(true);
  
  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));
  
  if((param == "contact") ||     // preferred
     (param == "them")) {        // deprecated 4/2010
    m_contact = toupper(param_val);
    return(true);
  }  
  else if((param == "complete_after_retired") && non_neg_number) {
    addInfoVars("CONTACTS_RETIRED", "nowarning");
    m_complete_after_retired = dval;
    return(true);
  }
  else if(param == "extrapolate")
    return(setBooleanOnString(m_extrapolate, param_val));
  else if(param == "match_contact_group") {
    if((m_match_contact_group != "") || (param_val == ""))
      return(false);
    m_match_contact_group = param_val;
    return(true);
  }
  else if(param == "ignore_contact_group") {
    if((m_ignore_contact_group != "") || (param_val == ""))
      return(false);
    m_ignore_contact_group = param_val;
    return(true);
  }
  else if((param == "match_group") ||
	  (param == "match_contact_group"))
    return(handleSetParamMatchGroup(param_val));
  else if((param == "ignore_group") ||
	  (param == "ignore_contact_group"))
    return(handleSetParamIgnoreGroup(param_val));
  else if(param == "match_type")
    return(handleSetParamMatchType(param_val));
  else if(param == "ignore_type")
    return(handleSetParamIgnoreType(param_val));
  else if(param == "decay") {
    string left  = biteStringX(param_val, ',');
    string right = param_val;
    if(isNumber(left) && isNumber(right)) {
      double start = atof(left.c_str());
      double end   = atof(right.c_str());
      if((start >= 0) && (start <= end)) {
	m_decay_start = start;
	m_decay_end   = end;
	m_extrapolator.setDecay(start,end);
	return(true);
      }
    }
  }  
  else if(param == "on_no_contact_ok") 
    return(setBooleanOnString(m_on_no_contact_ok, param_val));

  else if(param == "decay_end") {        // deprecated 4/2010
    if(isNumber(param_val)) {
      m_decay_end = dval;
      return(true);
    }
  }  
  else if(param == "time_on_leg") {
    if(!non_neg_number)
      return(false);
    m_time_on_leg = dval;
    return(true);
  }  
  // bearing_lines = white:0, green:0.65, yellow:0.8, red:1.0
  else if((param == "bearing_line_config") ||
	  (param == "bearing_lines")) {
    m_bearing_line_show = false;
    vector<string> svector = parseString(param_val, ',');
    unsigned int i, vsize = svector.size();
    bool valid_components = true;
    for(i=0; i<vsize; i++) {
      string left  = tolower(stripBlankEnds(biteString(svector[i],':')));
      string right = stripBlankEnds(svector[i]);
      if((left == "info") && (right == "range")) 
	m_bearing_line_info = "range";
      else if((left == "info") && (right == "relevance")) 
	m_bearing_line_info = "relevance";
      else if(isColor(left) && isNumber(right) && valid_components) {
	m_bearing_line_colors.push_back(left);
	double thresh_value = atof(right.c_str());
	m_bearing_line_thresh.push_back(thresh_value);
	m_bearing_line_show = true;
      }
      else
	valid_components = false;
    }
    return(true);
  }  
  return(false);
}

//-----------------------------------------------------------
// Procedure: updatePlatformInfo
//   Purpose: Update the following member variables:
//             m_osx: Current ownship x position (meters) 
//             m_osy: Current ownship y position (meters) 
//             m_osh: Current ownship heading (degrees 0-359)
//             m_osv: Current ownship speed (meters) 
//             m_cnx: Current contact x position (meters) 
//             m_cny: Current contact y position (meters) 
//             m_cnh: Current contact heading (degrees 0-359)
//             m_cnv: Current contact speed (meters) 
//             m_cnu: UTC time of last contact report

bool IvPContactBehavior::updatePlatformInfo()
{
  if(m_contact == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok = true;

  //==============================================================
  // Part 1: Update ownship and contact positions
  // Part 1A: Ascertain the current ownship position and trajectory.
  if(ok) m_osx = getBufferDoubleVal("NAV_X", ok);
  if(ok) m_osy = getBufferDoubleVal("NAV_Y", ok);
  if(ok) m_osh = getBufferDoubleVal("NAV_HEADING", ok);
  if(ok) m_osv = getBufferDoubleVal("NAV_SPEED", ok);
  if(!ok) {
    postEMessage("ownship x,y,heading, or speed info not found.");
    return(false);
  }
  m_osh = angle360(m_osh);
  m_cnh = angle360(m_cnh);

  // Part 1B: ascertain current contact position and trajectory.
  if(ok) m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok);
  if(ok) m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok);
  if(ok) m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok);
  if(ok) m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED",   ok);
  if(ok) m_cnutc = getBufferDoubleVal(m_contact+"_NAV_UTC", ok);
  if(!ok) {    
    string msg = m_contact + " x/y/heading/speed info not found";
    if(m_on_no_contact_ok)
      postWMessage(msg);
    else
      postEMessage(msg);
    return(false);
  }

  m_cn_group = getBufferStringVal(m_contact+"_NAV_GROUP");
  m_cn_vtype = getBufferStringVal(m_contact+"_NAV_TYPE");
  
  //==================================================================
  // Part 2: Extrapolate the contact position if extrapolation turn on

  // If extrapolation is turned off, we're done with the update.
  if(m_extrapolate) {
    // Even if mark_time is zero and thus "fresh", still derive the 
    // the contact position from the extrapolator since the UTC time
    // of the position in this report may still be substantially 
    // behind the current own-platform UTC time.
    
    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);
  
    double curr_time = getBufferCurrTime();
    double new_cnx, new_cny;
    ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);

    if(ok) {
      m_cnx = new_cnx;
      m_cny = new_cny;
    }
    else {
      string failure_reason = m_extrapolator.getFailureReason();
      postWMessage("Incomplete Linear Extrapolation: " + failure_reason);
    }
  }

  //==================================================================
  // Part 3: Monitor the retired contact list 

  if(m_complete_after_retired > 0) {
    string cns_retired = getBufferStringVal("CONTACTS_RETIRED");
    vector<string> svector = parseString(cns_retired, ',');
    double curr_time = getBufferCurrTime();
    if(!vectorContains(svector, tolower(m_contact))) {  // NOT on retlist
      m_cn_not_retired_tstamp = curr_time;
      m_cn_retired = false;
    }
    else {  // IS on the retired list, possibly flag for completion
      // Special case: if on ret list in first iter, just set tstamp
      if(m_cn_not_retired_tstamp < 0)
	m_cn_not_retired_tstamp = curr_time;     
      double elapsed = curr_time - m_cn_not_retired_tstamp;
      if(elapsed > m_complete_after_retired)
	m_cn_retired = true;
    }
  }

  //==================================================================
  // Part 4: Update the useful relative vehicle information
  
  m_cpa_engine.reset(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  m_rcpa_engine.reset(m_osy, m_osx, m_osh, m_osv, m_cny, m_cnx);    
    
  m_contact_range = hypot((m_osx-m_cnx), (m_osy-m_cny));

  m_os_cn_rel_bng = relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny);
  m_cn_os_rel_bng = relBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy);

  m_os_cn_abs_bng = m_cpa_engine.ownshipContactAbsBearing(); 
  m_cn_spd_in_os_pos = m_cpa_engine.getCNSpeedInOSPos();
  m_rate_of_closure = m_cpa_engine.evalROC(m_osh, m_osv);

  m_os_fore_of_cn  = m_cpa_engine.foreOfContact();
  m_os_aft_of_cn   = m_cpa_engine.aftOfContact();
  m_os_port_of_cn  = m_cpa_engine.portOfContact();
  m_os_starboard_of_cn = m_cpa_engine.starboardOfContact();

  m_rate_of_closure = m_cpa_engine.evalROC(m_osh, m_osv);

  m_cn_fore_of_os  = m_rcpa_engine.foreOfContact();
  m_cn_aft_of_os   = m_rcpa_engine.aftOfContact();
  m_cn_port_of_os  = m_rcpa_engine.portOfContact();
  m_cn_starboard_of_os = m_rcpa_engine.starboardOfContact();

  m_os_passes_cn         = m_cpa_engine.passesPortOrStar(m_osh, m_osv);
  m_os_passes_cn_port    = m_cpa_engine.passesPort(m_osh, m_osv);
  m_os_passes_cn_star    = m_cpa_engine.passesStar(m_osh, m_osv);
  
  m_cn_passes_os         = m_rcpa_engine.passesPortOrStar(m_cnh, m_cnv);
  m_cn_passes_os_port    = m_rcpa_engine.passesPort(m_cnh, m_cnv);
  m_cn_passes_os_star    = m_rcpa_engine.passesStar(m_cnh, m_cnv);
  
  m_os_crosses_cn_stern        = m_cpa_engine.crossesStern(m_osh, m_osv);
  m_os_crosses_cn_bow          = m_cpa_engine.crossesBow(m_osh, m_osv);
  m_os_crosses_cn_bow_or_stern = m_cpa_engine.crossesBowOrStern(m_osh, m_osv);
  m_os_crosses_cn_bow_dist     = m_cpa_engine.crossesBowDist(m_osh, m_osv);

  m_cn_crosses_os_stern        = m_rcpa_engine.crossesStern(m_cnh, m_cnv);
  m_cn_crosses_os_bow          = m_rcpa_engine.crossesBow(m_cnh, m_cnv);
  m_cn_crosses_os_bow_or_stern = m_rcpa_engine.crossesBowOrStern(m_cnh, m_cnv);
  m_cn_crosses_os_bow_dist     = m_rcpa_engine.crossesBowDist(m_cnh, m_cnv);

  m_range_gamma   = m_cpa_engine.getRangeGamma();
  m_range_epsilon = m_cpa_engine.getRangeEpsilon();

  m_os_curr_cpa_dist = m_cpa_engine.evalCPA(m_osh, m_osv, 120);

  return(ok);
}

//-----------------------------------------------------------
// Procedure: postViewableBearingLine

void IvPContactBehavior::postViewableBearingLine()
{
  if(!m_bearing_line_show)
    return;

  double index_value;
  if(m_bearing_line_info == "range")
    index_value = m_contact_range;
  else if(m_bearing_line_info == "relevance")
    index_value = m_relevance;
  else
    return;

  string color = "";
  unsigned int i, vsize = m_bearing_line_colors.size();
  for(i=0; (i<vsize)&&(color==""); i++) {
    if(index_value <= m_bearing_line_thresh[i])
      color = m_bearing_line_colors[i];
  }
  if(color == "") 
    color = "blank";

  m_bearing_line.clear(); 
  m_bearing_line.set_active(true);
  m_bearing_line.add_vertex(m_osx, m_osy);
  m_bearing_line.add_vertex(m_cnx, m_cny);
  m_bearing_line.set_label(m_us_name + "_" + m_descriptor);
  //m_bearing_line.set_color("label", "invisible");
  m_bearing_line.set_color("label", "white");
  m_bearing_line.set_color("edge", color);

  postMessage("VIEW_SEGLIST", m_bearing_line.get_spec());
}


//-----------------------------------------------------------
// Procedure: postErasableBearingLine

void IvPContactBehavior::postErasableBearingLine()
{
  if(m_bearing_line.size() == 0)
    return;

  m_bearing_line.set_active(false);
  postMessage("VIEW_SEGLIST", m_bearing_line.get_spec());
}


//-----------------------------------------------------------
// Procedure: handleSetParamMatchGroup()
//   Example: "blue" "blue,red,alpha"
//   Returns: false if provided no groups
//            false if a group is on the ignore list
//            true otherwise
//      Note: If a group is on the ignore list, it will not be
//            added to the match list, but if there are other
//            groups not on the ignore list, they will be added
//            to the match list.
//      Note: If a group is already on the match list it will
//            be ignored.

bool IvPContactBehavior::handleSetParamMatchGroup(string grpstr)
{
  bool all_ok = true;
  vector<string> svector = parseString(grpstr, ',');
  if(svector.size() == 0)
    all_ok = false;

  for(unsigned int i=0; i<svector.size(); i++) {
    string cn_group = stripBlankEnds(svector[i]);
    // Dont add a new match group if it is already an ignore group
    if(vectorContains(m_ignore_group, cn_group))
      all_ok = false;
    else if(!vectorContains(m_match_group, cn_group))
      m_match_group.push_back(cn_group);
  }
  return(all_ok);
}

//-----------------------------------------------------------
// Procedure: handleSetParamIgnoreGroup()
//   Example: "blue" "blue,red,alpha"
//   Returns: false if provided no groups
//            false if a group is on the match list
//            true otherwise
//      Note: If a group is on the match list, it will not be
//            added to the ignore list, but if there are other
//            groups not on the match list, they will be added
//            to the ignore list.
//      Note: If a group is already on the ignore list it will
//            be ignored.

bool IvPContactBehavior::handleSetParamIgnoreGroup(string grpstr)
{
  bool all_ok = true;
  vector<string> svector = parseString(grpstr, ',');
  if(svector.size() == 0)
    all_ok = false;

  for(unsigned int i=0; i<svector.size(); i++) {
    string cn_group = stripBlankEnds(svector[i]);
    // Dont add a new ignore group if it is already a match group
    if(vectorContains(m_match_group, cn_group))
      all_ok = false;
    else if(!vectorContains(m_ignore_group, cn_group))
      m_ignore_group.push_back(cn_group);
  }
  return(all_ok);
}

//-----------------------------------------------------------
// Procedure: handleSetParamMatchType()
//   Example: "kayak" "uuv,auv,ship"
//   Returns: false if provided no vehicle types
//            false if a vehicle type is on the ignore list
//            true otherwise
//      Note: If a type is on the ignore list, it will not be
//            added to the match list, but if there are other
//            types not on the ignore list, they will be added
//            to the match list.
//      Note: If a type is already on the match list it will
//            be ignored.

bool IvPContactBehavior::handleSetParamMatchType(string typestr)
{
  bool all_ok = true;
  vector<string> svector = parseString(typestr, ',');
  if(svector.size() == 0)
    all_ok = false;

  for(unsigned int i=0; i<svector.size(); i++) {
    string cn_vtype = stripBlankEnds(svector[i]);
    // Dont add a new match type if it is already an ignore type
    if(vectorContains(m_ignore_type, cn_vtype))
      all_ok = false;
    else if(!vectorContains(m_match_type, cn_vtype))
      m_match_type.push_back(cn_vtype);
  }
  return(all_ok);
}

//-----------------------------------------------------------
// Procedure: handleSetParamIgnoreType()
//   Example: "kayak" "uuv,auv,ship"
//   Returns: false if provided no vehicle types
//            false if a vehicle type is on the ignore list
//            true otherwise
//      Note: If a type is on the match list, it will not be
//            added to the ignore list, but if there are other
//            types not on the match list, they will be added
//            to the ignore list.
//      Note: If a type is already on the ignore list it will
//            be ignored.

bool IvPContactBehavior::handleSetParamIgnoreType(string typestr)
{
  bool all_ok = true;
  vector<string> svector = parseString(typestr, ',');
  if(svector.size() == 0)
    all_ok = false;

  for(unsigned int i=0; i<svector.size(); i++) {
    string cn_vtype = stripBlankEnds(svector[i]);
    // Dont add a new ignore type if it is already a match type
    if(vectorContains(m_match_type, cn_vtype))
      all_ok = false;
    else if(!vectorContains(m_ignore_type, cn_vtype))
      m_ignore_type.push_back(cn_vtype);
  }
  return(all_ok);
}

//-----------------------------------------------------------
// Procedure: getMatchGroupStr()

string IvPContactBehavior::getMatchGroupStr(string separator) const
{
  string str;
  for(unsigned int i=0; i<m_match_group.size(); i++) {
    if(str != "")
      str += separator;
    str += m_match_group[i];
  }
  return(str);
}

//-----------------------------------------------------------
// Procedure: getIgnoreGroupStr()

string IvPContactBehavior::getIgnoreGroupStr(string separator) const
{
  string str;
  for(unsigned int i=0; i<m_ignore_group.size(); i++) {
    if(str != "")
      str += separator;
    str += m_ignore_group[i];
  }
  return(str);
}

//-----------------------------------------------------------
// Procedure: getMatchTypeStr()

string IvPContactBehavior::getMatchTypeStr(string separator) const
{
  string str;
  for(unsigned int i=0; i<m_match_type.size(); i++) {
    if(str != "")
      str += separator;
    str += m_match_type[i];
  }
  return(str);
}

//-----------------------------------------------------------
// Procedure: getIgnoreTypeStr()

string IvPContactBehavior::getIgnoreTypeStr(string separator) const
{
  string str;
  for(unsigned int i=0; i<m_ignore_type.size(); i++) {
    if(str != "")
      str += separator;
    str += m_ignore_type[i];
  }
  return(str);
}


//-----------------------------------------------------------
// Procedure: getMatchIgnoreSummary()

string IvPContactBehavior::getMatchIgnoreSummary() const
{
  string summary;

  // Part 1: Add Match Groups if any
  string match_group_str = getMatchGroupStr();
  if(match_group_str != "") {
    if(summary != "")
      summary += ",";
    summary += "match_group=" + match_group_str;
  }

  // Part 2: Add Ignore Groups if any
  string ignore_group_str = getIgnoreGroupStr();
  if(ignore_group_str != "") {
    if(summary != "")
      summary += ",";
    summary += "ignore_group=" + ignore_group_str;
  }
  
  // Part 3: Add Match Types if any
  string match_type_str = getMatchTypeStr();
  if(match_type_str != "") {     
    if(summary != "") 
      summary += ",";
    summary += "match_type=" + match_type_str;
  }
  
  // Part 4: Add Ignore Types if any
  string ignore_type_str = getIgnoreTypeStr();
  if(ignore_type_str != "") {
    if(summary != "") 
      summary += ",";
    summary += "ignore_type=" + ignore_type_str;
  }
  
  return(summary);
}


//-----------------------------------------------------------
// Procedure: checkContactGroupRestrictions()
//      Note: Check here that the contact group is allowed.
//            Ideally group restrictions would have been conveyed
//            to a contact manager or similar. And if this 
//            behavior is a template, it would never have been
//            spawned, but we allow for checks here as well.

bool IvPContactBehavior::checkContactGroupRestrictions()
{
  // old style
  if((m_match_contact_group != "") &&
     (tolower(m_match_contact_group) != tolower(m_cn_group)))
    return(false);
  
  if((m_ignore_contact_group != "") &&
     (tolower(m_ignore_contact_group) == tolower(m_cn_group)))
    return(false);

  // Passing false to vectorContains indicates not case sensitive
  if(vectorContains(m_ignore_group, m_cn_group, false))
    return(false);

  if(m_match_group.size() != 0) {
    if(!vectorContains(m_match_group, m_cn_group, false))
      return(false);
  }
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: checkContactTypeRestrictions()
//      Note: Check here that the contact type is allowed.
//            Ideally type restrictions would have been conveyed
//            to a contact manager or similar. And if this 
//            behavior is a template, it would never have been
//            spawned, but we allow for checks here as well.

bool IvPContactBehavior::checkContactTypeRestrictions()
{
  // Passing false to vectorContains indicates not case sensitive
  if(vectorContains(m_ignore_type, m_cn_vtype, false))
    return(false);

  if(m_match_group.size() != 0) {
    if(!vectorContains(m_match_type, m_cn_vtype, false))
      return(false);
  }
  
  return(true);
}

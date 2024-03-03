/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

#include <cmath>
#include <cstdlib>
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "IvPContactBehavior.h"
#include "MBUtils.h"
#include "MacroUtils.h"
#include "VarDataPairUtils.h"

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

  m_post_per_contact_info = false;
  
  // Initialize in-mission exit criteria from filters
  m_exit_on_filter_vtype  = false;
  m_exit_on_filter_group  = false;
  m_exit_on_filter_region = false;


  // Initialize contact pose state variables
  m_cnx = 0;
  m_cny = 0;
  m_cnh = 0;
  m_cnv = 0;

  m_contact_range = 0;
  m_relevance     = 0;

  m_bearing_line_show = false;
  m_bearing_line_info = "relevance";
}

//-----------------------------------------------------------
// Procedure: setParamComplete()

void IvPContactBehavior::onSetParamComplete()
{
  IvPBehavior::onSetParamComplete();
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    m_logic_conditions[i].expandMacro("$[CONTACT]", toupper(m_contact));
    m_logic_conditions[i].expandMacro("$[contact]", tolower(m_contact));
    m_logic_conditions[i].expandMacro("$[Contact]", m_contact);
  }
}


//-----------------------------------------------------------
// Procedure: setParam()

bool IvPContactBehavior::setParam(string param, string param_val) 
{
  if(IvPBehavior::setParam(param, param_val))
    return(true);
  
  if(param == "contact") {
    string new_contact_name = toupper(param_val);
    if(m_contact != new_contact_name)
      onParamUpdate("contact");
    m_contact = new_contact_name;
    return(true);
  }  
  else if(param == "extrapolate")
    return(setBooleanOnString(m_extrapolate, param_val));
  else if(param == "post_per_contact_info")
    return(setBooleanOnString(m_post_per_contact_info, param_val));
  else if(param == "match_name")
    return(m_filter_set.addMatchName(param_val));
  else if(param == "ignore_name")
    return(m_filter_set.addIgnoreName(param_val));
  else if(param == "match_group")
    return(m_filter_set.addMatchGroup(param_val));
  else if(param == "ignore_group")
    return(m_filter_set.addIgnoreGroup(param_val));
  else if(param == "match_type")
    return(m_filter_set.addMatchType(param_val));
  else if(param == "ignore_type")
    return(m_filter_set.addIgnoreType(param_val));
  else if(param == "match_region")
    return(m_filter_set.addMatchRegion(param_val));
  else if(param == "ignore_region")
    return(m_filter_set.addIgnoreRegion(param_val));
  else if(param == "strict_ignore")
    return(m_filter_set.setStrictIgnore(param_val));
  else if(param == "exit_on_filter_vtype")
    return(setBooleanOnString(m_exit_on_filter_vtype, param_val));
  else if(param == "exit_on_filter_group")
    return(setBooleanOnString(m_exit_on_filter_group, param_val));
  else if(param == "exit_on_filter_region")
    return(setBooleanOnString(m_exit_on_filter_region, param_val));
      
  else if(param == "cnflag")
    return(addContactFlag(param_val));

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

  else if(param == "decay_end")         // deprecated 4/2010
    return(setDoubleOnString(m_decay_end, param_val));

  else if(param == "time_on_leg")
    return(setNonNegDoubleOnString(m_time_on_leg, param_val));
  
  else if(param == "bearing_line_label_show")
    return(setBooleanOnString(m_bearing_line_label_show, param_val));
  
  else if(param == "bearing_line_show")
    return(setBooleanOnString(m_bearing_line_show, param_val));
  
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
// Procedure: postFlag()

void IvPContactBehavior::postFlag(const VarDataPair& pair, bool repeat)
{
  // Handle data postings
  string var = pair.get_var();
  if(!pair.is_string()) {
    double ddata = pair.get_ddata();
    if(repeat)
      postRepeatableMessage(var, ddata);
    else
      postMessage(var, ddata);
    return;
  }

  // Handle pure data strings
  string sdata = stripBlankEnds(pair.get_sdata());
  if((sdata == "$[CPA]")      || (sdata == "$[OS_CN_REL_BNG]") ||
     (sdata == "$[RANGE]")    || (sdata == "$[CN_OS_REL_BNG]") ||
     (sdata == "$[BNG_RATE]")) {
    sdata = expandMacros(sdata);
    double dval = atof(sdata.c_str());
    if(repeat)
      postRepeatableMessage(var, dval);
    else
      postMessage(var, dval);
    return;
  }

  // Handle normal textual strings
  sdata = expandMacros(sdata);
  if(repeat)
    postRepeatableMessage(var, sdata);
  else
    postMessage(var, sdata);

}

//-----------------------------------------------------------
// Procedure: addContactFlag
//   Purpose: Before invoking the generic addVarDataPairOnString
//            function, check if the flag has a recognized tag.
//            Contact flags are posted on an event relative to a
//            contact, so a supported tag is mandatory.

bool IvPContactBehavior::addContactFlag(string str)
{
  str = stripBlankEnds(str);
  if(strBegins(str, "@<") || strBegins(str, "@>")) {
    string new_str = str.substr(2);
    string rng_str = biteStringX(new_str, ' ');
    double rng_dbl = atof(rng_str.c_str());
    if(isNumber(rng_str) && (rng_dbl >= 0))
      return(addVarDataPairOnString(m_cnflags, str));
  }
  else if(strBegins(str, "<") || strBegins(str, ">")) {
    string new_str = str.substr(1);
    string rng_str = biteStringX(new_str, ' ');
    double rng_dbl = atof(rng_str.c_str());
    if(isNumber(rng_str) && (rng_dbl >= 0))
      return(addVarDataPairOnString(m_cnflags, str));
  }
  else if(strBegins(str, "@cpa"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@os_passed_cn_port"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@os_passed_cn_star"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@os_passed_cn"))
    return(addVarDataPairOnString(m_cnflags, str));

  else if(strBegins(str, "@cn_passed_os_port"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@cn_passed_os_star"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@cn_passed_os"))
    return(addVarDataPairOnString(m_cnflags, str));

  else if(strBegins(str, "@os_crossed_cn"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@os_crossed_cn_bow"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@os_crossed_cn_stern"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@cn_crossed_os"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@cn_crossed_os_bow"))
    return(addVarDataPairOnString(m_cnflags, str));
  else if(strBegins(str, "@cn_crossed_os_stern"))
    return(addVarDataPairOnString(m_cnflags, str));
  
  return(false);    
}

//-----------------------------------------------------------
// Procedure: filterCheckHolds()
//   Purpose: If there are any applicable filters, check if
//            they still hold.
//    Return: false if any ONE filter has failed
//            true otherwise

bool IvPContactBehavior::filterCheckHolds() const
{
  if(m_exit_on_filter_vtype) {
    if(!m_filter_set.filterCheckVType(m_cn_vtype))
      return(false);
  }
  if(m_exit_on_filter_group) {
    if(!m_filter_set.filterCheckGroup(m_cn_group))
      return(false);
  }
  if(m_exit_on_filter_region) {
    if(!m_filter_set.filterCheckRegion(m_cnx, m_cny))
      return(false);
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: onEveryState()
//   Purpose: Invoked on every helm iteration regardless of the
//            behavior state (e.g., idle, active etc)

void IvPContactBehavior::onEveryState(string new_state)
{
  IvPBehavior::onEveryState(new_state);

  updatePlatformInfo();
  if(new_state == "running")
    handleContactFlags();
}

//-----------------------------------------------------------
// Procedure: handleContactFlags()
//   Purpose: Go through all the contact flags and send to
//            appropriate handler
//              Tag    Meaning
//            -------- --------------------------------------
//      Note: @< range posted when range transitions below val
//            @> range posted when range transitions above val
//            < range  posted constantly when range is below val
//            > range  posted constantly when range is above val
//            @cpa     posted when CPA is observed
//            @osxcn   posted when os changes port/star of cn
//            @ospcn   posted when os changes fore/aft of cn
//            @cnxos   posted when cn changes port/star of os
//            @ospcn   posted when cn changes fore/aft of os

void IvPContactBehavior::handleContactFlags()
{
  //==================================================
  // Handle Contact Flags
  //==================================================
  for(unsigned int i=0; i<m_cnflags.size(); i++) {
    string tag = m_cnflags[i].get_post_tag();
    
    if(strBegins(tag, "@<") || strBegins(tag, "<") ||
       strBegins(tag, "@>") || strBegins(tag, ">")) {
      handleContactFlagRange(m_cnflags[i]);
    }
    else if((tag == "@cpa") && m_cnos.cpa_reached())
      postFlag(m_cnflags[i], true);
    else if((tag == "@os_passed_cn") && m_cnos.os_passed_cn())
      postFlag(m_cnflags[i], true);
    else if((tag == "@os_passed_cn_port") && m_cnos.os_passed_cn_port())
      postFlag(m_cnflags[i], true);
    else if((tag == "@os_passed_cn_star") && m_cnos.os_passed_cn_star())
      postFlag(m_cnflags[i], true);

    else if((tag == "@cn_passed_os") && m_cnos.cn_passed_os())
      postFlag(m_cnflags[i], true);
    else if((tag == "@cn_passed_os_port") && m_cnos.cn_passed_os_port())
      postFlag(m_cnflags[i], true);
    else if((tag == "@cn_passed_os_star") && m_cnos.cn_passed_os_star())
      postFlag(m_cnflags[i], true);
  }
}

//-----------------------------------------------------------
// Procedure: handleContactFlagRange()

void IvPContactBehavior::handleContactFlagRange(VarDataPair flag)
{
  string post_tag   = flag.get_post_tag();
  double rng_thresh = 0;
  
  if(strBegins(post_tag, "@<") && (post_tag.length() > 2)) {
    bool ok = setNonNegDoubleOnString(rng_thresh, post_tag.substr(2));
    if(ok && m_cnos.rng_entered(rng_thresh))
      postFlag(flag, true);
  }
  else if(strBegins(post_tag, "@>") && (post_tag.length() > 2)) {
    bool ok = setNonNegDoubleOnString(rng_thresh, post_tag.substr(2));
    if(ok && m_cnos.rng_exited(rng_thresh))
      postFlag(flag, true);
  }
  else if(strBegins(post_tag, "<") && (post_tag.length() > 1)) {
    bool ok = setNonNegDoubleOnString(rng_thresh, post_tag.substr(1));
    if(ok && (m_contact_range <= rng_thresh))
      postFlag(flag, true);
  }
  else if(strBegins(post_tag, ">") && (post_tag.length() > 1)) {
    bool ok = setNonNegDoubleOnString(rng_thresh, post_tag.substr(1));
    if(ok && (m_contact_range >= rng_thresh))
      postFlag(flag, true);
  }
}

//-----------------------------------------------------------
// Procedure: expandMacros()

string IvPContactBehavior::expandMacros(string sdata)
{
  sdata = IvPBehavior::expandMacros(sdata);
  
  sdata = macroExpand(sdata, "CNX", m_cnx);
  sdata = macroExpand(sdata, "CNY", m_cny);
  sdata = macroExpand(sdata, "CNH", m_cnh);
  sdata = macroExpand(sdata, "CNV", m_cnv);

  sdata = macroExpand(sdata, "CN_NAME", m_contact);
  sdata = macroExpand(sdata, "CN_VTYPE", m_cn_vtype);
  sdata = macroExpand(sdata, "CN_GROUP", m_cn_group);
  sdata = macroExpand(sdata, "CN_RANGE", doubleToStringX(m_contact_range,1));
  sdata = macroExpand(sdata, "RANGE", doubleToStringX(m_contact_range,1));

  sdata = m_cnos.cnMacroExpand(sdata, "ROC");
  sdata = m_cnos.cnMacroExpand(sdata, "OS_CN_REL_BNG"); 
  sdata = m_cnos.cnMacroExpand(sdata, "CN_OS_REL_BNG"); // target angle
  sdata = m_cnos.cnMacroExpand(sdata, "BNG_RATE");
  sdata = m_cnos.cnMacroExpand(sdata, "CN_SPD_IN_OS_POS");
  sdata = m_cnos.cnMacroExpand(sdata, "OS_FORE_OF_CN");
  sdata = m_cnos.cnMacroExpand(sdata, "OS_AFT_OF_CN");
  sdata = m_cnos.cnMacroExpand(sdata, "OS_PORT_OF_CN");
  sdata = m_cnos.cnMacroExpand(sdata, "OS_STAR_OF_CN");
  sdata = m_cnos.cnMacroExpand(sdata, "CN_FORE_OF_OS");
  sdata = m_cnos.cnMacroExpand(sdata, "CN_AFT_OF_OS");
  sdata = m_cnos.cnMacroExpand(sdata, "CN_PORT_OF_OS");
  sdata = m_cnos.cnMacroExpand(sdata, "CN_STAR_OF_OS");
  
  return(sdata);
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
  // Sanity check - skip if update already successfully done
  if(m_cnos.helm_iter() == m_helm_iter)
    return(true);

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

  m_cnos.set_update_ok(false);
  if(m_contact == "") {
    if(m_on_no_contact_ok)
      return(true);
    postEMessage("contact IDX not set.");
    return(false);
  }
  

  if(tolower(m_contact) == "unset_ok")
    return(ok);
  
  // Part 1B: ascertain current contact position and trajectory.
  double cnutc = 0;
  if(ok) m_cnx = getBufferDoubleVal(m_contact+"_NAV_X", ok);
  if(ok) m_cny = getBufferDoubleVal(m_contact+"_NAV_Y", ok);
  if(ok) m_cnh = getBufferDoubleVal(m_contact+"_NAV_HEADING", ok);
  if(ok) m_cnv = getBufferDoubleVal(m_contact+"_NAV_SPEED",   ok);
  if(ok) cnutc = getBufferDoubleVal(m_contact+"_NAV_UTC", ok);
  if(!ok) {    
    string msg = m_contact + " x/y/heading/speed info not found";
    if(m_on_no_contact_ok)
      postWMessage(msg);
    else
      postEMessage(msg);
    return(false);
  }
  m_cnh = angle360(m_cnh);

  // Part 1C: At this point we know this update will be a success, i.e.,
  // it will return with true. So now is the time to invoke archive
  // on the ContactStateSet, moving the previously current cnos into the
  // cnos history and letting the newly current cnos hold current info.

  m_cnos.archiveCurrent();
  
  m_cnos.set_cnutc(cnutc);
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
    
    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, cnutc);
  
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
  // Part 3: Update the useful relative vehicle information
  
  m_cpa_engine.reset(m_cny, m_cnx, m_cnh, m_cnv, m_osy, m_osx);
  m_rcpa_engine.reset(m_osy, m_osx, m_osh, m_osv, m_cny, m_cnx);    
    
  m_contact_range = hypot((m_osx-m_cnx), (m_osy-m_cny));

  // range is stored in both m_contact_range and the cnos structure
  m_cnos.set_range(m_contact_range);
  
  m_cnos.set_os_fore_of_cn(m_cpa_engine.foreOfContact());
  m_cnos.set_os_aft_of_cn(m_cpa_engine.aftOfContact());
  m_cnos.set_os_port_of_cn(m_cpa_engine.portOfContact());
  m_cnos.set_os_star_of_cn(m_cpa_engine.starboardOfContact());

  m_cnos.set_cn_fore_of_os(m_rcpa_engine.foreOfContact());
  m_cnos.set_cn_aft_of_os(m_rcpa_engine.aftOfContact());
  m_cnos.set_cn_port_of_os(m_rcpa_engine.portOfContact());
  m_cnos.set_cn_star_of_os(m_rcpa_engine.starboardOfContact());

  m_cnos.set_cn_spd_in_os_pos(m_cpa_engine.getCNSpeedInOSPos());

  m_cnos.set_os_cn_rel_bng(relBearing(m_osx, m_osy, m_osh, m_cnx, m_cny));
  m_cnos.set_cn_os_rel_bng(relBearing(m_cnx, m_cny, m_cnh, m_osx, m_osy));
  m_cnos.set_os_cn_abs_bng(m_cpa_engine.ownshipContactAbsBearing());

  m_cnos.set_rate_of_closure(m_cpa_engine.evalROC(m_osh, m_osv));
  m_cnos.set_bearing_rate(m_cpa_engine.bearingRate(m_osh, m_osv));
  m_cnos.set_contact_rate(m_rcpa_engine.bearingRate(m_cnh, m_cnv));
  
  m_cnos.set_range_gamma(m_cpa_engine.getRangeGamma());
  m_cnos.set_range_epsilon(m_cpa_engine.getRangeEpsilon());

  m_cnos.set_os_passes_cn(m_cpa_engine.passesPortOrStar(m_osh, m_osv));
  m_cnos.set_os_passes_cn_port(m_cpa_engine.passesPort(m_osh, m_osv));
  m_cnos.set_os_passes_cn_star(m_cpa_engine.passesStar(m_osh, m_osv));
  
  m_cnos.set_cn_passes_os(m_rcpa_engine.passesPortOrStar(m_cnh, m_cnv));
  m_cnos.set_cn_passes_os_port(m_rcpa_engine.passesPort(m_cnh, m_cnv));
  m_cnos.set_cn_passes_os_star(m_rcpa_engine.passesStar(m_cnh, m_cnv));
  
  m_cnos.set_os_crosses_cn(m_cpa_engine.crossesBowOrStern(m_osh, m_osv));
  m_cnos.set_os_crosses_cn_stern(m_cpa_engine.crossesStern(m_osh, m_osv));
  m_cnos.set_os_crosses_cn_bow(m_cpa_engine.crossesBow(m_osh, m_osv));
  m_cnos.set_os_crosses_cn_bow_dist(m_cpa_engine.crossesBowDist(m_osh, m_osv));

  m_cnos.set_cn_crosses_os(m_rcpa_engine.crossesBowOrStern(m_cnh, m_cnv));
  m_cnos.set_cn_crosses_os_stern(m_rcpa_engine.crossesStern(m_cnh, m_cnv));
  m_cnos.set_cn_crosses_os_bow(m_rcpa_engine.crossesBow(m_cnh, m_cnv));
  m_cnos.set_cn_crosses_os_bow_dist(m_rcpa_engine.crossesBowDist(m_cnh, m_cnv));

  m_cnos.set_os_curr_cpa_dist(m_cpa_engine.evalCPA(m_osh, m_osv, 120));
  
  m_cnos.set_update_ok(true);

  // Mark helm_iteration to further indicate the current ContactState
  // has been successfully updated for this iteration.
  m_cnos.set_helm_iter(m_helm_iter);

  return(ok);
}

//-----------------------------------------------------------
// Procedure: postViewableBearingLine()

void IvPContactBehavior::postViewableBearingLine(bool active)
{
  if(!m_bearing_line_show)
    return;

  if(commsPolicy() != "open")
    return;

  double index_value;
  if(m_bearing_line_info == "range")
    index_value = m_contact_range;
  else if(m_bearing_line_info == "relevance")
    index_value = m_relevance;
  else
    return;

  string color = "";
  unsigned vsize = m_bearing_line_colors.size();
  for(unsigned int i=0; (i<vsize)&&(color==""); i++) {
    if(index_value <= m_bearing_line_thresh[i])
      color = m_bearing_line_colors[i];
  }
  if(color == "") 
    color = "blank";

  XYSegList m_bearing_line;
  m_bearing_line.set_active(true);
  m_bearing_line.add_vertex(m_osx, m_osy);
  m_bearing_line.add_vertex(m_cnx, m_cny);
  m_bearing_line.set_label(m_us_name + "_" + m_descriptor);
  if(m_bearing_line_label_show)
    m_bearing_line.set_color("label", "white");
  else
    m_bearing_line.set_color("label", "invisible");
  m_bearing_line.set_color("edge", color);
  m_bearing_line.set_duration(1);
  m_bearing_line.set_time(getBufferCurrTime());

  string segl_spec = m_bearing_line.get_spec();
  if(!active)
    segl_spec = m_bearing_line.get_spec_inactive();
  
  postMessage("VIEW_SEGLIST", segl_spec);
}


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollisionDetector.cpp                                */
/*    DATE: Dec 21st 2015                                        */
/*    DATE: Dec 30st 2024 Added ContactLedger in CPAMonitor      */
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

#include "CollisionDetector.h"
#include "XYRangePulse.h"
#include "XYCircle.h"
#include "MBUtils.h"
#include "LogicUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

CollisionDetector::CollisionDetector()
{
  m_encounter_dist = 20;
  m_near_miss_dist = 6;
  m_collision_dist = 3;

  // pulse configuration variables
  m_pulse_render = true;
  m_pulse_duration = 10;
  m_pulse_range = 20;

  // By default encounter rings are rendered
  m_encounter_rings = true;
  m_verbose = false;
  
  // state variables -- counters of collision types
  m_total_collisions  = 0;
  m_total_near_misses = 0;
  m_total_encounters  = 0;
  m_total_node_reports = 0;
  
  m_conditions_ok = true;

  m_post_closest_range = false;
  m_post_closest_range_ever = false;

  m_report_all_encounters = false;

  m_info_buffer = new InfoBuffer;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool CollisionDetector::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

    bool   handled = true;
    string whynot;

    if(key == "NODE_REPORT") 
      handled = handleMailNodeReport(sval, whynot);
    if(key == "UCD_RESET") 
      m_cpa_monitor.resetClosestRange();
    else 
      updateInfoBuffer(msg);

    if(!handled) {
      string warning = "Bad Mail: " + key;
      if(whynot != "")
	warning += " " + whynot;
      reportRunWarning(warning);
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool CollisionDetector::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool CollisionDetector::Iterate()
{
  AppCastingMOOSApp::Iterate();

  m_cpa_monitor.examineAndReport();
  m_cpa_monitor.setIteration(m_iteration);

  if(m_post_closest_range) {
    double closest_range = m_cpa_monitor.getClosestRange();
    if(closest_range > 0)
      Notify("UCD_CLOSEST_RANGE", closest_range);
  }

  cout << "closest_range:" << m_cpa_monitor.getClosestRange() << endl;
  
  if(m_post_closest_range_ever) {
    double closest_range_ever = m_cpa_monitor.getClosestRangeEver();
    if(closest_range_ever > 0)
      Notify("UCD_CLOSEST_RANGE_EVER", closest_range_ever);
  }
  
  m_conditions_ok = checkConditions();
  if(m_conditions_ok) {
    unsigned int events = m_cpa_monitor.getEventCount();
    for(unsigned int ix=0; ix<events; ix++) {
      CPAEvent event = m_cpa_monitor.getEvent(ix);
      handleCPAEvent(event);
    }
  }

  m_cpa_monitor.clear();
  if(m_encounter_rings)
    postRings();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: postRings()

void CollisionDetector::postRings()
{
  vector<string> vnames = m_cpa_monitor.getVNames();
  
  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];
    NodeRecord record = m_cpa_monitor.getVRecord(vname);
    if(record.valid() && (vname != "badguy")) {
      double x = record.getX();
      double y = record.getY();
      XYCircle circle(x,y,m_encounter_dist);
      circle.set_edge_color("white");
      circle.set_label(vname + "rng");
      circle.set_color("label", "off");
      string spec = circle.get_spec();
      Notify("VIEW_CIRCLE", spec);
    }
  }
}

//---------------------------------------------------------
// Procedure: handleCPAEvent()

void CollisionDetector::handleCPAEvent(CPAEvent event)
{
  //===========================================================
  // Part 1: Get the event structure information
  //===========================================================
  string v1   = event.getVName1();
  string v2   = event.getVName2();
  double cpa  = event.getCPA();
  double midx = event.getX();
  double midy = event.getY();
  string cpas = doubleToStringX(cpa,2);
  double alpha = event.getAlpha();
  double beta  = event.getBeta();
  
  m_total_encounters++;
  m_map_vname_encounters[v1]++;
  m_map_vname_encounters[v2]++;

  postFlags(m_encounter_flags, event);

  Notify("ENCOUNTER_TOTAL", m_total_encounters);

  string rank = "clear";
  if(cpa <= m_collision_dist) {
    rank = "collision";
    m_total_collisions++;
    m_map_vname_collisions[v1]++;
    m_map_vname_collisions[v2]++;
    postFlags(m_collision_flags, event);
    Notify("COLLISION_TOTAL", m_total_collisions);
  }
  else if(cpa <= m_near_miss_dist) {
    rank = "near_miss";
    m_total_near_misses++;
    m_map_vname_near_misses[v1]++;
    m_map_vname_near_misses[v2]++;
    postFlags(m_near_miss_flags, event);
    Notify("NEAR_MISS_TOTAL", m_total_near_misses);
  }
  
  string event_str = "Encounter: " + v1 + " and " + v2 + ":";
  event_str += "rank=" + rank + ", cpa=" + cpas;
  reportEvent(event_str);
  
  // Perhaps done if clear encounter and minimal reporting
  if((rank == "clear") && !m_report_all_encounters)
    return;
  
  //===========================================================
  // Part2: Build the Collision Detection Report UCD_REPORT
  //===========================================================

  // Part 2A: Build the basic report, names, cpa, rank.
  string report = "cpa=" + cpas + ",vname1=" + v1 + ",vname2=" + v2;
  report += ",rank=" + rank;

  // Part 2B: If we have relative bearing info, then characterized
  //          the type of encounter.
  if(alpha > 0)
    report += ", alpha=" + doubleToStringX(alpha,3);
  if(beta > 0)
    report += ", beta=" + doubleToStringX(beta,3);

  if((alpha > 0) && (beta > 0)) {
    bool cn_port_of_os = (beta > 180);
    bool os_port_of_cn = (alpha > 180);
    bool cn_fore_of_os = ((beta <= 90) || (beta >= 270));
    bool os_fore_of_cn = ((alpha <= 90) || (alpha >= 270));

    string ptype_os = "star";
    if(cn_port_of_os)
      ptype_os = "port";
    string ptype_cn = "star";
    if(os_port_of_cn)
      ptype_cn = "port";
    string ptype = ptype_os + ":" + ptype_cn;

    string xtype_os = "aft";
    if(cn_fore_of_os)
      xtype_os = "fore";
    string xtype_cn = "aft";
    if(os_fore_of_cn)
      xtype_cn = "fore";
    string xtype = xtype_os + ":" + xtype_cn;

    report += ", ptype=" + ptype;
    report += ", xtype=" + xtype;
  }
  
  // Part 2C: Post the final report
  Notify("UCD_REPORT", report);

  //===========================================================
  // Part 3: Build and post the Appcasting Event
  //===========================================================
  if(rank != "clear") {
    string ac_event = v1 + "::" + v2 + ", " + "cpa=" + cpas;
    ac_event += ", rank=" + rank;
    reportEvent(ac_event);
  }
  
  //===========================================================
  // Part 4: Build the RANGE_PULSE if rendering turned on
  //===========================================================
  if((rank != "clear") && m_pulse_render) {
    string pulse_color = "red";
    if(rank == "near_miss") 
      pulse_color = "yellow";    
    XYRangePulse pulse(midx, midy);
    pulse.set_rad(m_pulse_range);
    pulse.set_duration(m_pulse_duration);
    pulse.set_time(MOOSTime());
    pulse.set_color("edge", pulse_color);
    pulse.set_color("fill", pulse_color);
    string spec = pulse.get_spec();
    Notify("VIEW_RANGE_PULSE", spec);
  }  
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool CollisionDetector::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

   // Look for latitude, longitude initial datum
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    reportConfigWarning("Lat or Lon Origin not set in *.moos file.");

  bool ok_init = m_cpa_monitor.setGeodesy(lat_origin, lon_origin);
  if(!ok_init)
    reportConfigWarning("Geodesy failed to initialize");
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string line  = *p;
    string orig  = line;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "verbose") 
      handled = setBooleanOnString(m_verbose, value);
    else if(param == "collision_range") 
      handled = setNonNegDoubleOnString(m_collision_dist, value);
    else if(param == "near_miss_range") 
      handled = setNonNegDoubleOnString(m_near_miss_dist, value);
    else if(param == "encounter_range") 
      handled = setNonNegDoubleOnString(m_encounter_dist, value);
    else if(param == "pulse_render") 
      handled = setBooleanOnString(m_pulse_render, value);
    else if(param == "encounter_rings") 
      handled = setBooleanOnString(m_encounter_rings, value);
    else if(param == "pulse_range") 
      handled = setNonNegDoubleOnString(m_pulse_range, value);
    else if(param == "pulse_duration") 
      handled = setNonNegDoubleOnString(m_pulse_duration, value);
    else if(param == "collision_flag") 
      handled = handleConfigFlag("collision", value);
    else if(param == "near_miss_flag") 
      handled = handleConfigFlag("near_miss", value);
    else if(param == "encounter_flag") 
      handled = handleConfigFlag("encounter", value);
    else if(param == "post_closest_range") 
      handled = setBooleanOnString(m_post_closest_range, value);
    else if(param == "post_closest_range_ever") 
      handled = setBooleanOnString(m_post_closest_range_ever, value);
    else if(param == "report_all_encounters") 
      handled = setBooleanOnString(m_report_all_encounters, value);
    else if(param == "ignore_group") 
      handled = m_cpa_monitor.addIgnoreGroup(value);
    else if(param == "reject_group") 
      handled = m_cpa_monitor.addRejectGroup(value);
    else if(param == "condition") {
      LogicCondition new_condition;
      handled = new_condition.setCondition(value);
      if(handled)
	m_logic_conditions.push_back(new_condition);
      m_conditions_ok = false; // Assume false until shown otherwise
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  m_cpa_monitor.setVerbose(m_verbose);
  
  if(m_near_miss_dist < m_collision_dist) {
    m_near_miss_dist = m_collision_dist;
    reportConfigWarning("near_miss_dist set smaller than collision_dist");
  }
  if(m_encounter_dist < m_near_miss_dist) {
    m_encounter_dist = m_near_miss_dist;
    reportConfigWarning("encounter_dist set smaller than near_miss_dist");
  }
  
  m_cpa_monitor.setReportRange(m_encounter_dist);
  m_cpa_monitor.setIgnoreRange(m_encounter_dist * 1.5);

  m_param_summary = "collision_range=" + doubleToStringX(m_collision_dist);
  m_param_summary += ", near_miss_range=" + doubleToStringX(m_near_miss_dist);
  m_param_summary += ", encounter_range=" + doubleToStringX(m_encounter_dist);
  Notify("COLLISION_DETECT_PARAMS", m_param_summary);

  Notify("ENCOUNTER_TOTAL", (double)0);
  Notify("NEAR_MISS_TOTAL", (double)0);
  Notify("COLLISION_TOTAL", (double)0);
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void CollisionDetector::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("UCD_RESET", 0);
  
  //=======================================================
  // Register for variables used in the logic conditions
  //=======================================================
  // First get all the variable names from all present conditions.
  vector<string> all_vars;
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);
  
  // Register for all variables found in all conditions.
  for(unsigned int i=0; i<all_vars.size(); i++)
    Register(all_vars[i], 0);
}


//------------------------------------------------------------
// Procedure: handleConfigFlag()

bool CollisionDetector::handleConfigFlag(string flag_type, string str)
{
  string moosvar = biteStringX(str, '=');
  string moosval = str;

  if((moosvar == "") || (moosval == ""))
    return(false);
  
  VarDataPair pair(moosvar, moosval, "auto");
  if(flag_type == "collision")
    m_collision_flags.push_back(pair);
  else if(flag_type == "near_miss")
    m_near_miss_flags.push_back(pair);
  else if(flag_type == "encounter")
    m_encounter_flags.push_back(pair);
  else
    return(false);

  return(true);
}


//------------------------------------------------------------
// Procedure: expandMacroCD()
//   Purpose: Examine the string and expand macros of the type
//            $[vname@range]. The vname could be an known vehicle
//            name, or just V1 or V2 involved in an encounter.
//  Examples: "foo=$[nelson@40], bar=$[V2@50]"
//            "$[abe,20]
//      Note: The numerical component specifies a range. The macro
//            will expand to an unsigned int string, indicating
//            the number of contacts presently within that range,
//            the Contact Density (CD) of the moment.

string CollisionDetector::expandMacroCD(string orig, string vname1,
					string vname2)
{
  string result = orig;

  vector<string> svector = parseString(orig, '$');
  for(unsigned int i=0; i<svector.size(); i++) {
    string str = svector[i];
    if(strBegins(str, "[") && strContains(str, "]")) {
      biteString(str, '[');
      rbiteString(str, ']');
      string macro = "$[" + str + "]";
      vector<string> jvector = parseString(str, '@');
      if(jvector.size() == 2) {
	string vname = jvector[0];
	string range = jvector[1];
	if(isNumber(range)) {
	  double drng = atof(range.c_str());
	  if(drng >= 0) {
	    if((vname == "V1") || (vname == "v1"))
	      vname = vname1;
	    else if((vname == "V2") || (vname == "v2"))
	      vname = vname2;
	    unsigned int cd = m_cpa_monitor.getContactDensity(vname, drng);
	    string cd_str = uintToString(cd);
	    result = findReplace(result, macro, cd_str);
	  }
	}
      }
    }
  }
  return(result);
}

//------------------------------------------------------------
// Procedure: postFlags()

void CollisionDetector::postFlags(const vector<VarDataPair>& flags,
				  const CPAEvent& event)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();

    // Handle Variable macro expansion for $V1 and $V2 
    string vname1  = event.getVName1();
    string vname2  = event.getVName2();
    moosvar = findReplace(moosvar, "$V1", vname1);
    moosvar = findReplace(moosvar, "$V2", vname2);
    moosvar = findReplace(moosvar, "$UP_V1", toupper(vname1));
    moosvar = findReplace(moosvar, "$UP_V2", toupper(vname2));
    moosvar = findReplace(moosvar, "$IDX", uintToString(m_total_encounters));

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();
      double cpa_dbl = event.getCPA();
      string cpa_str = doubleToStringX(cpa_dbl, 4);

      sval = expandMacroCD(sval, vname1, vname2);
      if(isNumber(sval)) {
	double dval = atof(sval.c_str());
	Notify(moosvar, dval);
      }
      // If the string is just $CPA or $IDX interpret as a double posting
      else if(sval == "$CPA")
	Notify(moosvar, cpa_dbl);
      else if(sval == "$IDX") 
	Notify(moosvar, m_total_encounters);
      else {
	sval = findReplace(sval, "$V1", vname1);
	sval = findReplace(sval, "$V2", vname2);
	sval = findReplace(sval, "$UP_V1", toupper(vname1));
	sval = findReplace(sval, "$UP_V2", toupper(vname2));
	sval = findReplace(sval, "$CPA", cpa_str);
	sval = findReplace(sval, "$IDX", uintToString(m_total_encounters));
	Notify(moosvar, sval);
      }
    }
  }
}

//-----------------------------------------------------------
// Procedure: checkconditions()
//   Purpose: Determine if all the logic conditions in the vector
//            of conditions is met, given the snapshot of variable
//            values in the info_buffer.

bool CollisionDetector::checkConditions()
{
  if(!m_info_buffer) 
    return(false);

  // Phase 1: get all the variable names from all present conditions.
  vector<string> condition_vars = getUniqueVars(m_logic_conditions);
  
  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  for(unsigned int i=0; i<condition_vars.size(); i++) {
    string varname = condition_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);

    if(ok_s) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
	m_logic_conditions[j].setVarVal(varname, s_result);
    }

    if(ok_d) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
      m_logic_conditions[j].setVarVal(varname, d_result);
    }
  }

    
  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    bool satisfied = m_logic_conditions[i].eval();
    if(!satisfied)
      return(false);
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool CollisionDetector::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.GetKey();
  string sdata = msg.GetString();
  double ddata = msg.GetDouble();

  if(msg.IsDouble()) 
    return(m_info_buffer->setValue(key, ddata));
  else if(msg.IsString())
    return(m_info_buffer->setValue(key, sdata));

  return(false);
}

//------------------------------------------------------------
// Procedure: handleMailNodeReport()

bool CollisionDetector::handleMailNodeReport(string report,
					     string& whynot)
{
  // Part 1: inject the node report into the CPAMonitor
  m_total_node_reports++;
  bool ok = m_cpa_monitor.handleNodeReport(report, whynot);
  if(!ok) 
    return(false);

  // Part 2: The first time we deal with a particular vehicle, we 
  // post the parameter summary to be sent to that vehicle. We do
  // this now rather than at startup at this app, since we can't 
  // be sure that the other vehicle is online when this app starts
  // up. But now that we have received a node report from that 
  // vehicle, we can be pretty sure it will get this one-time msg.
  string vname = tokStringParse(report, "NAME", ',', '=');
  if(!vectorContains(m_notified_vehicles, vname)) {
    m_notified_vehicles.push_back(vname);
    vname = toupper(vname);
    Notify("COLLISION_DETECT_PARAMS_" + vname, m_param_summary);
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: buildReport()

//  ============================================
//  uFldCollisionDetect
//  ============================================
//  Configuration:
//  ============================================
//         encounter_dist:
//         collision_dist:
//         near_miss_dist:
//          ignore_groups:
//          reject_groups:
//     range_pulse_render:
//   range_pulse_duration:
//      range_pulse_range:
//   
//  ============================================
//  State Overall:
//  ============================================
//                 Active: true
//       Total Encounters:  121
//       Total Collisions:    2
//      Total Near Misses:   11
//                                            
//  ============================================
//  State By Vehicle:
//  ============================================
//  Vehicle  Encounters  Collisions  Near Misses
//  -------  ----------  ----------  -----------
//      abe         129           2           12
//      ben         111           3            8
//      cal          80           5           32


bool CollisionDetector::buildReport() 
{
  string encounter_str = doubleToString(m_encounter_dist,2);
  string coll_dist_str = doubleToString(m_collision_dist,2);
  string near_miss_str = doubleToString(m_near_miss_dist,2);
  string pulse_ren_str = boolToString(m_pulse_render);
  string pulse_dur_str = doubleToString(m_pulse_duration,2);
  string pulse_rng_str = doubleToString(m_pulse_range,2);
  string ignore_grps_str = m_cpa_monitor.getIgnoreGroups();
  string reject_grps_str = m_cpa_monitor.getRejectGroups();
  string post_cr_str  = boolToString(m_post_closest_range);
  string post_cre_str = boolToString(m_post_closest_range_ever);
  
  m_msgs << "Configuration:                               " << endl;
  m_msgs << "============================================ " << endl;
  m_msgs << "         encounter_dist: " << encounter_str << endl;
  m_msgs << "         collision_dist: " << coll_dist_str << endl;
  m_msgs << "         near_miss_dist: " << near_miss_str << endl;
  m_msgs << "          ignore_groups: " << ignore_grps_str << endl;
  m_msgs << "          reject_groups: " << reject_grps_str << endl;
  m_msgs << "     range_pulse_render: " << pulse_ren_str << endl;
  m_msgs << "   range_pulse_duration: " << pulse_dur_str << endl;
  m_msgs << "      range_pulse_range: " << pulse_rng_str << endl;
  m_msgs << "     post_closest_range: " << post_cr_str << endl;
  m_msgs << "post_closest_range_ever: " << post_cre_str << endl << endl;
  
  string conditions_ok_str  = boolToString(m_conditions_ok);
  string tot_encounters_str = uintToString(m_total_encounters);
  string tot_near_miss_str = uintToString(m_total_near_misses);
  string tot_collision_str = uintToString(m_total_collisions);

  double closest_range = m_cpa_monitor.getClosestRange();
  double closest_range_ever = m_cpa_monitor.getClosestRangeEver();
  string cr_str  = doubleToStringX(closest_range,2);
  string cre_str = doubleToStringX(closest_range_ever,2);

  m_msgs << endl;
  m_msgs << "Ledger: " << m_cpa_monitor.getLedgerSummary() << endl;
  m_msgs << endl;

  m_msgs << "============================================" << endl;
  m_msgs << "State Overall:                              " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "             Active: " << conditions_ok_str   << endl;
  m_msgs << "       Node Reports: " << m_total_node_reports << endl;
  m_msgs << "      Closest Range: " << cr_str              << endl;
  m_msgs << " Closest Range Ever: " << cre_str             << endl;
  m_msgs << "   Total Encounters: " << tot_encounters_str  << endl;
  m_msgs << "  Total Near Misses: " << tot_near_miss_str   << endl;
  m_msgs << "   Total Collisions: " << tot_collision_str   << endl << endl;

  m_msgs << "============================================" << endl;
  m_msgs << "State By Vehicle:                           " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);

  actab << "Vehicle | Encounters | Near Misses | Collisions";
  actab.addHeaderLines();

  map<string, unsigned int>::iterator p;
  for(p=m_map_vname_encounters.begin(); p!=m_map_vname_encounters.end(); p++) {
    string vname = p->first;
    unsigned int encounters  = p->second;
    unsigned int near_misses = m_map_vname_near_misses[vname];
    unsigned int collisions  = m_map_vname_collisions[vname];

    string s_encounters  = uintToString(encounters);
    string s_near_misses = uintToString(near_misses);
    string s_collisions  = uintToString(collisions);

    actab << vname << s_encounters << s_near_misses << s_collisions;
  }
  m_msgs << actab.getFormattedString();
  return(true);
}

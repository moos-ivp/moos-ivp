/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FldNodeComms.cpp                                     */
/*    DATE: Dec 4th 2011                                         */
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

#include <cmath>
#include <set>
#include <iterator>
#include "FldNodeComms.h"
#include "MBUtils.h"
#include "NodeRecordUtils.h"
#include "NodeMessageUtils.h"
#include "XYCommsPulse.h"
#include "ColorParse.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

FldNodeComms::FldNodeComms()
{
  // The default range within which reports are sent between nodes
  m_comms_range      = 100;

  // A range with which node reports are sent between nodes regardless
  // of other criteria (in the spirit of collision avoidance).
  m_critical_range   = 30;

  m_default_stealth  = 1.0;
  m_default_earange  = 1.0;
  m_min_stealth      = 0.1;  
  m_max_earange      = 10.0;

  m_stale_time       = 5.0;
  m_stale_forget     = m_stale_time * 20;
  m_min_msg_interval = 30.0;
  m_min_rpt_interval = -1;
  m_max_msg_length   = 1000;    // zero means unlimited length
  
  m_stale_dropped    = 0;
  
  m_drop_pct = 0; // percentage of messages that should be dropped
  srand((int)time(0));// seed the random number generator

  m_msg_color = "white";
  m_msg_repeat_color = "light_green";
  
  m_min_share_interval = 0.1;
  
  m_pulse_duration   = 10;      // zero means no pulses posted.
  m_view_node_rpt_pulses = true;

  // If true then comms between vehicles only happens if they are
  // part of the same group. (unless range is within critical).
  m_apply_groups      = false;
  m_apply_groups_msgs = false;

  m_total_reports_sent  = 0;
  m_total_messages_rcvd = 0;
  m_total_messages_sent = 0;
  m_total_ack_messages_rcvd = 0;
  m_total_ack_messages_sent = 0;

  m_blk_msg_invalid  = 0;
  m_blk_msg_toostale = 0;
  m_blk_msg_tooquick = 0;
  m_blk_msg_toolong  = 0;
  m_blk_msg_toofar   = 0;
  m_blk_msg_noinfo   = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool FldNodeComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  m_ledger.setCurrTimeUTC(m_curr_time);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 
    string msrc   = msg.GetSource(); 
    double dval   = msg.GetDouble(); 
    
    bool   handled = false;
    string whynot;

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) 
      handled = handleMailNodeReport(sval, whynot);
    else if((key == "NODE_MESSAGE") || (key == "MEDIATED_MESSAGE"))
      handled = handleMailNodeMessage(sval, msrc);
    else if(key == "ACK_MESSAGE") 
      handled = handleMailAckMessage(sval);
    else if(key == "UNC_SHARED_NODE_REPORTS") 
      handled = handleEnableSharedNodeReports(sval);
    else if(key == "UNC_STEALTH") 
      handled = handleStealth(sval);
    else if(key == "UNC_COMMS_RANGE") 
      handled = handleMailCommsRange(dval);
    else if(key == "UNC_VIEW_NODE_RPT_PULSES") 
      handled = setBooleanOnString(m_view_node_rpt_pulses, sval);
    else if(key == "UNC_EARANGE") 
      handled = handleEarange(sval);
    else if(key == "UNC_DROP_PCT") {
      handled = false;
      if((dval >= 0) && (dval <= 100)) {
	m_drop_pct = dval;
	handled = true;
      }
    }      
    
    if(!handled) {
      string warning = "Unhandled Mail: " + key;
      if(whynot != "")
	warning += " " + whynot;
      reportRunWarning(warning);
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool FldNodeComms::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool FldNodeComms::Iterate()
{
  AppCastingMOOSApp::Iterate();

  m_ledger.setCurrTimeUTC(m_curr_time);
  
  // Part 2: Distribute Node reports
  map<string, bool>::iterator p;
  for(p=m_map_newrecord.begin(); p!=m_map_newrecord.end(); p++) {
    string uname   = p->first;    
    bool   newinfo = p->second;
    if(newinfo) {
      distributeNodeReportInfo(uname);
      localShareNodeReportInfo(uname);
    }
  }

  // Part 2: Distribute NodeMessages (and MediatedMessages)
  map<string, bool>::iterator p2;
  for(p2=m_map_newmessage.begin(); p2!=m_map_newmessage.end(); p2++) {
    string src_name = p2->first;    
    bool   newinfo  = p2->second;
    // If the message has changed, consider sending it out
    if(newinfo) 
      distributeNodeMessageInfo(src_name);
  }

  // Part 3: Distribute AckMessages
  map<string, bool>::iterator p3;
  for(p3=m_map_newackmessage.begin(); p3!=m_map_newackmessage.end(); p3++) {
    string src_name = p3->first;    
    bool   newinfo  = p3->second;
    // If the message has changed, consider sending it out
    if(newinfo) 
      distributeAckMessageInfo(src_name);
  }

  m_map_newrecord.clear();

  m_map_newmessage.clear();
  m_map_newackmessage.clear();

  m_map_message.clear();
  m_map_ack_message.clear();

  clearStaleNodes();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool FldNodeComms::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

   // Look for latitude, longitude initial datum
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    reportConfigWarning("Lat or Lon Origin not set in *.moos file.");

  bool ok_init = m_ledger.setGeodesy(lat_origin, lon_origin);
  if(!ok_init)
    reportConfigWarning("Geodesy failed to initialize");
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if((param == "comms_range") && (value == "nolimit")) {
      // A negative comms range means all comms goes through
      // A zero comms range means nothing goes through
      m_comms_range = -1;
      handled = true;
    }
    else if(param == "comms_range")
      // A negative comms range means all comms goes through
      // A zero comms range means nothing goes through
      handled = setDoubleOnString(m_comms_range, value);
    else if(param == "critical_range")
      handled = setNonNegDoubleOnString(m_critical_range, value);
    else if(param == "stale_time") {
      // A negative stale time means staleness never applies
      handled = setDoubleOnString(m_stale_time, value);
      m_stale_forget = m_stale_time * 20;
    }
    else if(param == "min_msg_interval")
      handled = setNonNegDoubleOnString(m_min_msg_interval, value);
    else if(param == "min_rpt_interval") 
      handled = setNonNegDoubleOnString(m_min_rpt_interval, value);
    else if(param == "max_msg_length")
      handled = setUIntOnString(m_max_msg_length, value);
    else if(param == "stealth") 
      handled = handleStealth(value);
    else if(param == "earange") 
      handled = handleEarange(value);
    else if(param == "groups") 
      handled  = setBooleanOnString(m_apply_groups, value);
    else if(param == "msg_groups") 
      handled  = setBooleanOnString(m_apply_groups_msgs, value);
    else if(param == "view_node_rpt_pulses") 
      handled = setBooleanOnString(m_view_node_rpt_pulses, value);

    else if(param == "drop_percentage") 
      handled = setPosDoubleOnString(m_drop_pct, value);
    else if(param == "pulse_duration") 
      handled = setDoubleOnString(m_pulse_duration, value);
    else if(param == "shared_node_reports") 
      handled = handleEnableSharedNodeReports(value);
    else if(param == "msg_color") 
      handled = setColorOnString(m_msg_color, value);
    else if(param == "msg_repeat_color") 
      handled = setColorOnString(m_msg_repeat_color, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables()

void FldNodeComms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("NODE_MESSAGE", 0);
  Register("MEDIATED_MESSAGE", 0);
  Register("ACK_MESSAGE", 0);
  Register("UNC_COMMS_RANGE", 0);
  Register("UNC_STEALTH", 0);
  Register("UNC_EARANGE", 0);
  Register("UNC_DROP_PCT", 0);
  Register("UNC_VIEW_NODE_RPT_PULSES", 0);
}

//------------------------------------------------------------
// Procedure: handleMailNodeReport()

bool FldNodeComms::handleMailNodeReport(const string& str, string& whynot)
{
  string vname = m_ledger.processNodeReport(str, whynot);
  if(whynot != "")
    return(false);

  string upp_name = toupper(vname);
  m_map_newrecord[upp_name] = true;

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailNodeMessage()
//   Example: NODE_MESSAGE = src_node=henry,dest_node=ike,
//                           var_name=FOO, string_val=bar   

bool FldNodeComms::handleMailNodeMessage(const string& msg,
					 const string& msg_src)
{
  NodeMessage new_message = string2NodeMessage(msg);
  
  // Part 1: List of "last" messages store solely for user
  // debug viewing at the console window.
  m_last_messages.push_back(msg);
  if(m_last_messages.size() > 5) 
    m_last_messages.pop_front();

  // Part 2: Check that the message is valid
  if(!new_message.valid())
    return(false);

  // Part 3: If the source app is not named in the message,
  // then add it here. Added Mar 30, 2022.
  if(new_message.getSourceApp() == "")
    new_message.setSourceApp(msg_src);

  // Part 4: 
  string upp_src_node = toupper(new_message.getSourceNode());

  m_map_message[upp_src_node].push_back(new_message);
  m_map_newmessage[upp_src_node] = true;

  if(m_map_messages_rcvd.count(upp_src_node) == 0)
    m_map_messages_rcvd[upp_src_node] = 1;
  else
    m_map_messages_rcvd[upp_src_node]++;
  m_total_messages_rcvd++;

  reportEvent("Msg rec'd: " + msg);

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailAckMessage()
//   Example: ACK_MESSAGE = src=henry, dest=ike, id=ike_21

bool FldNodeComms::handleMailAckMessage(const string& msg)
{
  AckMessage new_message = string2AckMessage(msg);

  // #1 List of "last" messages store solely for user debug 
  //    viewing at the console window.
  m_last_messages.push_back(msg);
  if(m_last_messages.size() > 5) 
    m_last_messages.pop_front();

  // #2 Check that the message is valid
  if(!new_message.valid())
    return(false);

  string upp_src_node = toupper(new_message.getSourceNode());

  m_map_ack_message[upp_src_node].push_back(new_message);
  m_map_newackmessage[upp_src_node] = true;

  if(m_map_ack_messages_rcvd.count(upp_src_node) == 0)
    m_map_ack_messages_rcvd[upp_src_node] = 1;
  else
    m_map_ack_messages_rcvd[upp_src_node]++;

  m_total_ack_messages_rcvd++;

  reportEvent("Msg rec'd: " + msg);

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailCommsRange()
//   Example: 300

bool FldNodeComms::handleMailCommsRange(double new_range)
{
  m_comms_range = new_range;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleStealth()
//   Example: vname=alpha,stealth=0.4"
//      Note: Max value is 1.0. Min value set by m_min_stealth

bool FldNodeComms::handleStealth(const string& str)
{
  string vname, stealth;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = toupper(value);
    else if(param == "stealth")
      stealth = value;
  }

  if(!isNumber(stealth))
    return(false);

  double dbl_stealth = atof(stealth.c_str());
  if(dbl_stealth < m_min_stealth)
    dbl_stealth = m_min_stealth;
  if(dbl_stealth > 1)
    dbl_stealth = 1;

  m_map_stealth[vname] = dbl_stealth;
  return(true);
}


//------------------------------------------------------------
// Procedure: handleEarange()
//   Example: vname=alpha,earange=0.5
//      Note: Min value is 1.0. Max value set by m_max_earange

bool FldNodeComms::handleEarange(const string& str)
{
  string vname, earange;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = toupper(value);
    else if((param == "earange") || (param == "earrange"))
      earange = value;
  }

  if(!isNumber(earange))
    return(false);

  double dbl_earange = atof(earange.c_str());
  if(dbl_earange > m_max_earange)
    dbl_earange = m_max_earange;
  if(dbl_earange < 1)
    dbl_earange = 1;

  m_map_earange[vname] = dbl_earange;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleEnableShareNodeReports()
//   Purpose: When shared node reports are enabled, uFldNodeComms
//            re-publishes the node reports locally, presumably for
//            pMarineViewer. This allows the viewer to have a more
//            controlled flow of node reports and lets uFldNodeComms
//            share in the burden of pruning.
//            uFldNodeComms needs to handle node reports out to the
//            vehicles at a rate that allows the vehicles to do things
//            like collision avoidance.  The viewer however only needs
//            to make things look smooth. When there is a high time
//            warp, the viewer needs far fewer than every node
//            report. uFldNodeComms can handle this pruning.

bool FldNodeComms::handleEnableSharedNodeReports(string str)
{
  bool handled = true;
  if(tolower(str) == "true")
    m_min_share_interval = 0.1;
  else if(tolower(str) == "false")
    m_min_share_interval = -1;
  else if(isNumber(str))
    handled = setNonNegDoubleOnString(m_min_share_interval, str);
  else
    handled = false;

  return(handled);
}

//------------------------------------------------------------
// Procedure: distributeNodeReportInfo()
//   Purpose: Post the node report for vehicle <uname> to all 
//            other vehicles as NODE_REPORT_VNAME where <uname>
//            is not equal to VNAME (no need to report to self).
//     Notes: Inter-vehicle node reports must pass certain criteria
//            based on inter-vehicle range, group, and the respective
//            stealth and earange of the senting and receiving nodes.

void FldNodeComms::distributeNodeReportInfo(const string& us_vname)
{
  if(!m_ledger.isValid(us_vname))
    return;

  // We'll need the same node report sent out to all vehicles.
  string node_report = m_ledger.getSpec(us_vname);

  vector<string> vnames = m_ledger.getVNames();
  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];

    // Criteria #1: Skip and do not send to self
    if(vname == us_vname)
      continue;

    // NOTE: If nodes within "critical" range, send report to be safe!
    if(meetsCriticalRangeThresh(us_vname, vname)) {
      postNodeReport(us_vname, vname, node_report);
      continue;
    }

    // Criteria #2: Skip if receiving vehicle not heard from recently.
    if(m_ledger.isStale(vname, m_stale_time))
      continue;

    // Criteria #3: Skip if range between vehicles is too large.
    if(!meetsRangeThresh(us_vname, vname)) 
      continue;

    // Criteria #4: Skip if groups considered, and not same group
    if(m_apply_groups && !m_ledger.groupMatch(us_vname, vname))
      continue;

    // Criteria #5: Skip randomly sometimes to replicate dropped msgs
    if(meetsDropPercentage())
      continue;

    // Criteria #6: Skip if already sent too recently
    if(!meetsReportRateThresh(us_vname, vname))
      continue;

    postNodeReport(us_vname, vname, node_report);
  }  
}


//------------------------------------------------------------
// Procedure: postNodeReport()

void FldNodeComms::postNodeReport(string us_vname, string vname,
				  string node_report)
{
  Notify("NODE_REPORT_" + vname, node_report);
  if(m_view_node_rpt_pulses)
    postViewCommsPulse(us_vname, vname);
  m_total_reports_sent++;
  m_map_reports_sent[vname]++;
  
  string key = tolower(us_vname) + ":" + tolower(vname);
  m_map_last_rpt_tstamp[key] = m_curr_time;
  
}

//------------------------------------------------------------
// Procedure: localShareNodeReportInfo()
//   Purpose: Post node report for vehicle <us_vname> to a variable 
//            designed to be used on shoreside to feed pMarineViewer.
//            This allows a viewing app to receive node reports only
//            as fast as makes sense for viewing. Node reports sent
//            to vehicles otherwise need it as fast as generated.

void FldNodeComms::localShareNodeReportInfo(const string& us_vname)
{
  // First check if the latest record for the given vehicle is valid.
  if(!m_ledger.isValid(us_vname))
    return;
  if(m_min_share_interval < 0)
    return;

  bool ok_to_post = true;
  if(m_map_lshare_tstamp.count(us_vname)) {
    double elapsed = m_curr_time - m_map_lshare_tstamp[us_vname];
    double real_time_elapsed = elapsed / m_time_warp;
    Notify("RTE", real_time_elapsed);
    if(real_time_elapsed < m_min_share_interval)
      ok_to_post = false;
  }
  
  if(!ok_to_post)
    return;

  // Post the report and note the time stamp
  m_map_lshare_tstamp[us_vname] = m_curr_time;
  string node_report = m_ledger.getSpec(us_vname);
  Notify("NODE_REPORT_UNC", node_report);
}


//------------------------------------------------------------
// Procedure: distributeNodeMessageInfo()
//      Note: Mod Sep 20th, 2015 (mikerb) to allow uFldNodeComms to
//            keep all messages received since the previous Iteration.
//            Previously messages sent together in time, and all
//            received in one iteration here, would drop all but the
//            latest message.

void FldNodeComms::distributeNodeMessageInfo(const string& src_name)
{
  if(m_map_message.count(src_name) == 0)
    return;

  for(unsigned int i=0; i<m_map_message[src_name].size(); i++) {
    double elapsed = m_curr_time - m_map_time_nmessage[src_name];
    if(elapsed >= m_min_msg_interval) {
      NodeMessage message = m_map_message[src_name][i];
      distributeNodeMessageInfo(src_name, message);
      m_map_time_nmessage[src_name] = m_curr_time;
    }
    else
      m_blk_msg_tooquick++;
      
  }
}


//------------------------------------------------------------
// Procedure: distributeNodeMessageInfo()
//   Purpose: Post the node message for vehicle <src_name> to the  
//            recipients specified in the message. 
//     Notes: The recipient may be specified by the name of the recipient
//            node, or the vehicle/node group. Group membership is 
//            determined by nodes reporting their group membership as
//            part of their node report.
//     Notes: Inter-vehicle node reports must pass certain criteria
//            based on inter-vehicle range, group, and the respective
//            stealth and earange of the senting and receiving nodes.

void FldNodeComms::distributeNodeMessageInfo(string src_name,
					     NodeMessage message)
{
  // Check 1: If the latest message for the given vehicle is valid.
  if(!message.valid()) {
    m_blk_msg_invalid++;
    return;
  }
  
  // Check 2: If max_msg_length is limited (!=0), check len of this msg
  if((m_max_msg_length > 0) && (message.length() > m_max_msg_length)) {
    m_blk_msg_toolong++;
    return;
  }

  // Check 3: Ensure src vehicle info is valid and recent
  if(!m_ledger.hasVNameValidNotStale(src_name))
    return;

  
  string msg_color = m_msg_color;
  if(message.getColor() != "")
    msg_color = message.getColor();
  
  // Part 1: Begin determining the list of destinations
  // Examples: dest=ben, dest=abe:ben, dest=all, 
  set<string> dest_names;

  // If destination name(s) given add each one in the colon-separated list
  bool all = false;
  string dest_name  = toupper(message.getDestNode());
  string dest_group = toupper(message.getDestGroup());
  if((dest_name == "ALL") || (dest_group == "ALL"))
    all = true;

  if(all)
    dest_names = m_ledger.getVNamesByGroup("all");
  else {
    // Add names on the dest=vnames list
    vector<string> svector = parseString(dest_name, ':');
    for(unsigned int i=0; i<svector.size(); i++) {
      string vname = stripBlankEnds(svector[i]);
      if(m_ledger.hasVName(vname))
	dest_names.insert(vname);
    }

    // Add names by group
    set<string> grp_members = m_ledger.getVNamesByGroup(dest_group);
    dest_names = mergeSets(dest_names, grp_members);
  }

  dest_names.erase(src_name);
  // End determining the list of destinations
    
  // Part 2: Begin handling all the destinations
  set<string>::iterator p;
  for(p=dest_names.begin(); p!=dest_names.end(); p++) {
    string a_dest_name = *p;

    // Criteria #N: vehicles in same group if enforced
    if(m_apply_groups_msgs) {
      if(!m_ledger.groupMatch(src_name, a_dest_name))
	continue;
    }

    // Criteria #1B: Must have node records (position info) for both vehicles
    if(!m_ledger.hasVNameValidNotStale(a_dest_name, m_stale_time)) {
      m_blk_msg_noinfo++;
      continue;
    }

    // Criteria #3: the range between vehicles is not too large.
    if(!meetsRangeThresh(src_name, a_dest_name)) {
      m_blk_msg_toofar++;
      continue;
    }

    // Criteria #4: randomly drop some messages based on config
    if(meetsDropPercentage())
      continue;

    string moos_var = "NODE_MESSAGE_" + a_dest_name;
    if(message.getAckRequested())
      moos_var = "MEDIATED_MESSAGE_" + a_dest_name;
    
    string node_message = message.getSpec();
    Notify(moos_var, node_message);
    
    string id = message.getMessageID();
    if((id != "") && listContains(m_recent_ackids,id))
      postViewCommsPulse(src_name, a_dest_name, "msg",
			 m_msg_repeat_color, 0.6);
    else
      postViewCommsPulse(src_name, a_dest_name, "msg",
			 msg_color, 0.6);
    
    if(id != "") {
      m_recent_ackids.push_front(id);
      if(m_recent_ackids.size() > 100)
	m_recent_ackids.pop_back();
    }
      
    m_total_messages_sent++;
    m_map_messages_sent[a_dest_name]++;
  }
}


//------------------------------------------------------------
// Procedure: distributeAckMessageInfo()
//      Note: Added March 2022

void FldNodeComms::distributeAckMessageInfo(const string& src_name)
{
  if(m_map_ack_message.count(src_name) == 0)
    return;

  for(unsigned int i=0; i<m_map_ack_message[src_name].size(); i++) {
    double elapsed = m_curr_time - m_map_time_amessage[src_name];
    if(elapsed >= m_min_msg_interval) {
      AckMessage message = m_map_ack_message[src_name][i];
      distributeAckMessageInfo(src_name, message);
      m_map_time_amessage[src_name] = m_curr_time;
    }
    else
      m_blk_msg_tooquick++;
  }
}


//------------------------------------------------------------
// Procedure: distributeAckMessageInfo()
//      Note: Added March 2022

void FldNodeComms::distributeAckMessageInfo(string src_name,
					    AckMessage message)
{
  // Check 1: Confirm validity of message
  if(!message.valid()) {
    m_blk_msg_invalid++;
    return;
  }
  
  // Check 2: Must have node records for both vehicles
  string dest_name = toupper(message.getDestNode());
  if(!m_ledger.hasVNameValid(src_name) ||
     !m_ledger.hasVNameValid(dest_name)) {
    m_blk_msg_noinfo++;
    return;
  }
  
  // Check 3: Both node records must not be stale
  if(!m_ledger.hasVNameValidNotStale(src_name, m_stale_time) ||
     !m_ledger.hasVNameValidNotStale(dest_name, m_stale_time)) {
    m_blk_msg_toostale++;
    return;
  }
    
  // Check 4: the range between vehicles is not too large.
  if(!meetsRangeThresh(src_name, dest_name)) {
    m_blk_msg_toofar++;
    return;
  }

  // Check 5:: randomly drop some messages
  if(meetsDropPercentage())
    return;
  
  string moos_var = "ACK_MESSAGE_" + dest_name;
  string ack_message = message.getSpec();
  Notify(moos_var, ack_message);
  //postViewCommsPulse(src_name, dest_name, "msg", msg_color, 0.6);
  m_total_ack_messages_sent++;
  m_map_ack_messages_sent[dest_name]++;
}


//------------------------------------------------------------
// Procedure: clearStaleNodes()
//            For info associated with a vehicle that is not cleared on
//            each iteration, clear after a period of time when no new
//            node report info has been received.

void FldNodeComms::clearStaleNodes()
{
  if(m_stale_forget < 0)
    return;

  vector<string> stales = m_ledger.getVNamesStale(m_stale_forget);
  m_stale_dropped += stales.size();
  
  for(unsigned int i=0; i<stales.size(); i++) {
    string vname = stales[i];
    m_ledger.clearNode(vname); 

    m_map_message.erase(vname);
    m_map_newrecord.erase(vname);
    m_map_time_nmessage.erase(vname);
    m_map_lshare_tstamp.erase(vname);
    m_map_reports_sent.erase(vname);
    m_map_messages_rcvd.erase(vname);
    m_map_messages_sent.erase(vname);
  }
}
  
//------------------------------------------------------------
// Procedure: meetsDropPercentage()
//   Purpose: Determine if message should be dropped based on
//            drop percentage (returning true means drop)

bool FldNodeComms::meetsDropPercentage()
{
  // if drop percentage > 0, randomly drop messages,
  // to probabilistically let the all but drop percentage through
  if(m_drop_pct > 0) {

    // generate random number between 0 and 100:
    size_t rand_nr = rand() % 100;

    // drop message if random number is within drop percentage range
    // probabilistically we should thus drop the percentage amount
    if(rand_nr < m_drop_pct)
      return(true); // drop msg
  }
  return(false);
}


//------------------------------------------------------------
// Procedure: meetsRangeThresh()
//   Purpose: Determine if Vehicle2 should hear the node report
//            of Vehicle1, given the raw range, earange and 
//            stealth factors.

bool FldNodeComms::meetsRangeThresh(string vname1, string vname2)
{
  if(m_comms_range < 0)
    return(true);
  if(m_comms_range == 0)
    return(false);

  vname1 = toupper(vname1);
  vname2 = toupper(vname2);
  
  if(!m_ledger.hasVNameValidNotStale(vname1))
    return(false);
  if(!m_ledger.hasVNameValidNotStale(vname2))
    return(false);
  
  double x1 = m_ledger.getX(vname1);
  double y1 = m_ledger.getY(vname1);
  double x2 = m_ledger.getX(vname2);
  double y2 = m_ledger.getY(vname2);
  double range = hypot((x1-x2), (y1-y2));

  // See if source vehicle has a modified stealth value 
  double stealth = 1.0;
  if(m_map_stealth.count(vname1))
    stealth = m_map_stealth[vname1];

  // See if receiving vehicle has a modified earange value
  double earange = 1.0;
  if(m_map_earange.count(vname2))
    earange = m_map_earange[vname2];

  // Comms range thresh is baseline + stealth, earange.
  double comms_range = m_comms_range * stealth * earange;
  if(range > comms_range)
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: meetsReportRateThresh()
//   Purpose: Determine if Vehicle2 should receive the node report
//            of Vehicle1, given the amount of time since the last
//            report sent from vehicle 1 to 2.

bool FldNodeComms::meetsReportRateThresh(string vname1, string vname2)
{
  // Sanity check: If no rate limits enabled, just return true
  if(m_min_rpt_interval < 0)
    return(true);

  string key = tolower(vname1) + ":" + tolower(vname2);

  if(m_map_last_rpt_tstamp.count(key) == 0) {
    //int rand_delay = rand() % (int)(m_min_rpt_interval);
    int rand_delay = 0;
    m_map_last_rpt_tstamp[key] = m_curr_time + rand_delay;
    return(true);
  }
    
  double elapsed = m_curr_time - m_map_last_rpt_tstamp[key];
  if(elapsed < m_min_rpt_interval)
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: meetsCriticalRangeThresh()
//   Purpose: Determine if Vehicle2 should hear the node report
//            of Vehicle1, given the raw range and critical range.

bool FldNodeComms::meetsCriticalRangeThresh(string vname1,
					    string vname2)
{
  vname1 = toupper(vname1);
  vname2 = toupper(vname2);
  
  if(!m_ledger.hasVNameValidNotStale(vname1))
    return(false);
  if(!m_ledger.hasVNameValidNotStale(vname2))
    return(false);
  
  double x1 = m_ledger.getX(vname1);
  double y1 = m_ledger.getY(vname1);
  double x2 = m_ledger.getX(vname2);
  double y2 = m_ledger.getY(vname2);
  double range = hypot((x1-x2), (y1-y2));

  if(range <= m_critical_range)
    return(true);
  return(false);
}


//------------------------------------------------------------
// Procedure: postViewCommsPulse()

void FldNodeComms::postViewCommsPulse(string vname1,
				      string vname2,
				      string pulse_type,
				      string pcolor,
				      double fill_opaqueness)
{
  if(m_pulse_duration <= 0)
    return;

  string lpcolor = tolower(pcolor);
  if((lpcolor == "invisible") || (lpcolor == "off"))
    return;

  vname1 = toupper(vname1);
  vname2 = toupper(vname2);
  if(vname1 == vname2)
    return;

  double x1 = m_ledger.getX(vname1);
  double y1 = m_ledger.getY(vname1);
  double x2 = m_ledger.getX(vname2);
  double y2 = m_ledger.getY(vname2);

  double beam_length = hypot(x1-x2, y1-y2);
  double beam_width = 0.1 * beam_length;
  if(beam_width < 5)
    beam_width = 5;
  
  XYCommsPulse pulse(x1, y1, x2, y2);
  
  string label = vname1 + "2" + vname2;

  if(pcolor != "auto")
    label += pcolor;

  pulse.set_duration(m_pulse_duration);
  pulse.set_label(label);
  pulse.set_time(m_curr_time);
  pulse.set_beam_width(beam_width);
  pulse.set_fill(fill_opaqueness);
  pulse.set_pulse_type(pulse_type);
  
  string pulse_color = pcolor;
  if(pcolor == "auto")
    pulse_color = "orange";

  pulse.set_color("fill", pulse_color);

  string pulse_spec = pulse.get_spec();
  Notify("VIEW_COMMS_PULSE", pulse_spec);
}

//------------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of AppCastingMOOSApp superclass, conditionally 
//            invoked if either a terminal or appcast report is needed.

bool FldNodeComms::buildReport()
{
  bool shared_rpts_enabled = (m_min_rpt_interval > 0);
  string share_rpt_string = boolToString(shared_rpts_enabled);
  if(shared_rpts_enabled)
    share_rpt_string += " (" + doubleToStringX(m_min_share_interval,2) + ")";
  
  m_msgs << "Configuration:  " << endl;
  m_msgs << "       Comms Range: " << doubleToStringX(m_comms_range)    << endl;
  m_msgs << "    Critical Range: " << doubleToStringX(m_critical_range) << endl;
  m_msgs << "        Stale Drop: " << doubleToStringX(m_stale_time)     << endl;
  m_msgs << "       Stale Foget: " << doubleToStringX(m_stale_forget)   << endl;
  m_msgs << "   Max Message Len: " << doubleToStringX(m_max_msg_length) << endl;
  m_msgs << "   Min Message Int: " << doubleToStringX(m_min_msg_interval) << endl;
  m_msgs << "   Min Report  Int: " << doubleToStringX(m_min_rpt_interval,2) << endl;
  m_msgs << "          Drop Pct: " << doubleToStringX(m_drop_pct,2)     << endl;
  m_msgs << "Apply Group (reps): " << boolToString(m_apply_groups)      << endl;
  m_msgs << "Apply Group (msgs): " << boolToString(m_apply_groups_msgs) << endl;
  m_msgs << "     Share Reports: " << share_rpt_string << endl;
  m_msgs << endl;

  double elapsed_app = (m_curr_time - m_start_time);
  double total_reps_rcvd = m_ledger.totalReports();
  string tot_rcvd = padString(uintToString(total_reps_rcvd),10,false);
  
  m_msgs << "Node Report Summary"                    << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "     Total Received: " << tot_rcvd << "Elapsed      Rate" << endl;
  vector<string> vnames = m_ledger.getVNames();
  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];
    unsigned int total = m_ledger.totalReports(vname);
    string total_str = padString(uintToString(total),7,false);

    string pad_vname  = padString(vname, 17);
    m_msgs << "  " << pad_vname << ": " << total_str;

    string elapsed_str = "n/a";
    double elapsed_time = m_ledger.getUTCAgeReceived(vname);
    elapsed_str = "(" + doubleToString(elapsed_time,1) + ")";
    elapsed_str = padString(elapsed_str,8);

    string rate_str = "n/a";
    if(elapsed_app > 0) {
      double rate = (double)(total) / elapsed_app;
      rate_str = doubleToString(rate,2);
    }
    rate_str = "[" + rate_str + "]";
    rate_str = padString(rate_str, 12);
    
    m_msgs << elapsed_str << rate_str << endl;
  }    
    

  m_msgs << "     --------------- " << endl;
  m_msgs << "         Total Sent: " << m_total_reports_sent << endl;
  map<string, unsigned int>::iterator p2;
  for(p2=m_map_reports_sent.begin(); p2!=m_map_reports_sent.end(); p2++) {
    string vname = p2->first;
    unsigned int total = p2->second;
    vname  = padString(vname, 17);
    m_msgs << "  " << vname << ": " << total << endl;
  }
  m_msgs << "     --------------- " << endl;
  m_msgs << "     Stale Vehicles: " << m_stale_dropped << endl;

  
  m_msgs << endl;
  m_msgs << "Node Message Summary"                   << endl;
  m_msgs << "======================================" << endl;

  m_msgs << "    Total Msgs Received: " << m_total_messages_rcvd << endl;
  map<string, unsigned int>::iterator q;
  for(q=m_map_messages_rcvd.begin(); q!=m_map_messages_rcvd.end(); q++) {
    string vname = q->first;
    unsigned int total = q->second;
    string pad_vname  = padString(vname, 20);
    m_msgs << "  " << pad_vname << ": " << total;

    double elapsed_time = m_curr_time - m_map_time_nmessage[vname];
    string stime = "(" + doubleToString(elapsed_time,1) + ")";
    stime = padString(stime,12);
    m_msgs << stime << endl;
  }

  m_msgs << "     ------------------ " << endl;
  m_msgs << "            Total Sent: " << m_total_messages_sent << endl;
  map<string, unsigned int>::iterator q2;
  for(q2=m_map_messages_sent.begin(); q2!=m_map_messages_sent.end(); q2++) {
    string vname = q2->first;
    unsigned int total = q2->second;
    vname  = padString(vname, 20);
    m_msgs << "  " << vname << ": " << total << endl;
  }
  
  unsigned int total_blk_msgs = m_blk_msg_invalid + m_blk_msg_toostale + 
    m_blk_msg_tooquick + m_blk_msg_toolong + m_blk_msg_toofar + m_blk_msg_noinfo;
  string blk_msg_invalid  = uintToString(m_blk_msg_invalid);
  string blk_msg_toostale = uintToString(m_blk_msg_toostale);
  string blk_msg_tooquick = uintToString(m_blk_msg_tooquick);
  string blk_msg_toolong  = uintToString(m_blk_msg_toolong);
  string blk_msg_toofar   = uintToString(m_blk_msg_toofar);
  string blk_msg_noinfo   = uintToString(m_blk_msg_noinfo);
  
  m_msgs << "     ------------------ " << endl;
  m_msgs << "    Total Blocked Msgs: " << total_blk_msgs << endl;
  m_msgs << "               Invalid: " << blk_msg_invalid << endl;
  m_msgs << "          Missing Info: " << blk_msg_noinfo << endl;
  m_msgs << "        Stale Receiver: " << blk_msg_toostale << endl;
  m_msgs << "            Too Recent: " << blk_msg_tooquick<< endl;
  m_msgs << "          Msg Too Long: " << blk_msg_toolong << endl;
  m_msgs << "         Range Too Far: " << blk_msg_toofar << endl << endl;

  return(true);
}

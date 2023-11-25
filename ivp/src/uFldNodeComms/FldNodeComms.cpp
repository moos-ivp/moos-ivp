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
  m_min_share_interval = 0.1;
  
  m_stale_dropped    = 0;
  
  m_drop_pct = 0; // percentage of messages that should be dropped
  srand((int)time(0));// seed the random number generator

  m_verbose          = false;
  m_debug            = false;

  m_msg_color = "white";
  m_msg_repeat_color = "light_green";
  
  m_min_share_interval = 0.1;
  
  m_pulse_duration   = 10;      // zero means no pulses posted.
  m_view_node_rpt_pulses = true;

  // If true then comms between vehicles only happens if they are
  // part of the same group. (unless range is within critical).
  m_apply_groups      = false;
  m_apply_groups_msgs = false;

  m_total_reports_rcvd  = 0;
  m_total_reports_sent  = 0;
  m_total_messages_rcvd = 0;
  m_total_messages_sent = 0;
  m_total_ack_messages_rcvd = 0;
  m_total_ack_messages_sent = 0;

  m_blk_msg_invalid   = 0;
  m_blk_msg_toostale  = 0;
  m_blk_msg_tooquick  = 0;
  m_blk_msg_toolong   = 0;
  m_blk_msg_toofar    = 0;
  m_blk_msg_noinfo    = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool FldNodeComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

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
    else if(key == "UNC_FULL_REPORT_REQ")
      handled = handleMailFullReportReq(sval);
    
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

  cout << "Iterating..." << endl;
  
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
  cout << "OnStartup Starting" << endl;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  cout << "OnStartup 111" << endl;
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    cout << "param: " << param << endl;
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
    else if(param == "verbose") 
      handled = setBooleanOnString(m_verbose, value);
    else if(param == "view_node_rpt_pulses") 
      handled = setBooleanOnString(m_view_node_rpt_pulses, value);
    else if(param == "debug") 
      handled = setBooleanOnString(m_debug, value);
    else if(param == "drop_percentage") 
      handled = setPosDoubleOnString(m_drop_pct, value);
    else if(param == "pulse_duration") 
      handled = setDoubleOnString(m_pulse_duration, value);
    else if(param == "shared_node_reports") 
      handled = handleEnableSharedNodeReports(value);
    else if(param == "ignore_group") 
      handled = setNonWhiteVarOnString(m_ignore_group, value);
    else if(param == "msg_color") 
      handled = setColorOnString(m_msg_color, value);
    else if(param == "msg_repeat_color") 
      handled = setColorOnString(m_msg_repeat_color, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }


  cout << "OnStartupCompleted" << endl;
  registerVariables();
  return(true);
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
  map<string,double>::iterator p;

  vector<string> stale_vnames;
  
  for(p=m_map_time_nreport.begin(); p!=m_map_time_nreport.end(); p++) {
    string vname = p->first;
    double tstamp = p->second;
    if((m_curr_time - tstamp) > (m_stale_forget)) {
      stale_vnames.push_back(vname);
      m_stale_dropped++;
    }    
  }

  for(unsigned int i=0; i<stale_vnames.size(); i++) {
    string vname = stale_vnames[i];
    m_map_record.erase(vname);
    m_map_message.erase(vname);
    m_map_newrecord.erase(vname);
    m_map_time_nreport.erase(vname);
    m_map_time_nmessage.erase(vname);
    m_map_vindex.erase(vname);
    m_map_vgroup.erase(vname);
    m_map_lshare_tstamp.erase(vname);
    m_map_reports_rcvd.erase(vname);
    m_map_reports_sent.erase(vname);
    m_map_messages_rcvd.erase(vname);
    m_map_messages_sent.erase(vname);
  }
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
  Register("UNC_FULL_REPORT_REQ", 0);
  Register("UNC_VIEW_NODE_RPT_PULSES", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport()

bool FldNodeComms::handleMailNodeReport(const string& str, string& whynot)
{
  NodeRecord new_record = string2NodeRecord(str);
  if(!new_record.valid("x,y,name", whynot))
    return(false);
  
  string upp_name = toupper(new_record.getName());
  string grp_name = toupper(new_record.getGroup());

  if(m_ignore_group != "") {
    if(toupper(m_ignore_group) == grp_name)
      return(true);
  }
  
  m_map_record[upp_name] = new_record;
  m_map_newrecord[upp_name] = true;
  m_map_time_nreport[upp_name] = m_curr_time;
  m_map_vgroup[upp_name]  = grp_name;

  unsigned int vindex_size = m_map_vindex.size();
  if(m_map_vindex.count(upp_name) == 0)
    m_map_vindex[upp_name] = vindex_size;

  if(m_map_reports_rcvd.count(upp_name) == 0)
    m_map_reports_rcvd[upp_name] = 1;
  else
    m_map_reports_rcvd[upp_name]++;
  m_total_reports_rcvd++;

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

  unsigned int vindex_size = m_map_vindex.size();
  if(m_map_vindex.count(upp_src_node) == 0)
    m_map_vindex[upp_src_node] = vindex_size;

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

  unsigned int vindex_size = m_map_vindex.size();
  if(m_map_vindex.count(upp_src_node) == 0)
    m_map_vindex[upp_src_node] = vindex_size;

  if(m_map_ack_messages_rcvd.count(upp_src_node) == 0)
    m_map_ack_messages_rcvd[upp_src_node] = 1;
  else
    m_map_ack_messages_rcvd[upp_src_node]++;

  m_total_ack_messages_rcvd++;

  reportEvent("Msg rec'd: " + msg);

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
// Procedure: handleMailCommsRange()
//   Example: 300

bool FldNodeComms::handleMailCommsRange(double new_range)
{
  m_comms_range = new_range;
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

void FldNodeComms::distributeNodeReportInfo(const string& uname)
{
  // First check if the latest record for the given vehicle is valid.
  NodeRecord record = m_map_record[uname];
  if(!record.valid())
    return;

  // We'll need the same node report sent out to all vehicles.
  string node_report = record.getSpec();

  map<string, NodeRecord>::iterator p;
  for(p=m_map_record.begin(); p!=m_map_record.end(); p++) {
    string vname = p->first;

    // Criteria #1: vehicles different
    if(vname == uname)
      continue;

    if(m_verbose) 
      cout << uname << "--->" << vname << ": " << endl;

    bool msg_send = true;
    
    // Criteria #2: receiving vehicle has been heard from recently.
    // Freshness is enforced to disallow messages to a vehicle that may in
    // fact be much farther away than a stale node report would indicate
    double vname_time_since_update = m_curr_time - m_map_time_nreport[vname];
    if(vname_time_since_update > m_stale_time)
      msg_send = false;
    
    // Criteria #3: the range between vehicles is not too large.
    if(msg_send && !meetsRangeThresh(uname, vname))
      msg_send = false;
    
    
    // Criteria #4: if groups considered, check for same group
    if(msg_send && m_apply_groups) {
      if(m_verbose) {
	cout << "  uname group:" << m_map_vgroup[uname] << endl;
	cout << "  vname group:" << m_map_vgroup[vname] << endl;
      }
      if(m_map_vgroup[uname] != m_map_vgroup[vname])
	msg_send = false;
    }
    
    // Criteria #5: randomly drop messages, to probabilistically let 
    //              all but drop percentage through
    if(msg_send && meetsDropPercentage())
      msg_send = false;


    if(msg_send && !meetsReportRateThresh(uname, vname))
      msg_send = false;
    
    // Extra Criteria: If otherwise not sending, check to see if nodes
    // are within "critical" range. If so send report regardless of 
    // anything else - in the spirit of safety!
    if(!msg_send && meetsCriticalRangeThresh(uname, vname))
      msg_send = true;

    if(m_full_rpt_vname == vname) {
      if(m_full_rpt_waiting.count(uname)) {
	msg_send = true;
	m_full_rpt_waiting.erase(uname);
	if(m_full_rpt_waiting.size() == 0)
	  m_full_rpt_vname = "";
      }
    }

    if(msg_send) {
      string moos_var = "NODE_REPORT_" + vname;
      Notify(moos_var, node_report);
      if(m_view_node_rpt_pulses)
	postViewCommsPulse(uname, vname);
      m_total_reports_sent++;
      m_map_reports_sent[vname]++;

      string key = tolower(uname) + ":" + tolower(vname);
      m_map_last_rpt_tstamp[key] = m_curr_time;
    }

    if(m_verbose) {
      if(msg_send)
	cout << "NODE_REPORT Sent!" << endl;
      else
	cout << "NODE_REPORT Held!" << endl;
    }
  }  
}


//------------------------------------------------------------
// Procedure: localShareNodeReportInfo()
//   Purpose: Post the node report for vehicle <uname> to a variable 
//            designed to be used on shoreside to feed pMarineViewer.
//            This allows a viewing app to receive node reports only
//            as fast as makes sense for viewing. Node reports sent
//            to vehicles otherwise need it as fast as generated.

void FldNodeComms::localShareNodeReportInfo(const string& uname)
{
  // First check if the latest record for the given vehicle is valid.
  NodeRecord record = m_map_record[uname];
  if(!record.valid())
    return;

  bool ok_to_post = true;
  if(m_map_lshare_tstamp.count(uname)) {
    double elapsed = m_curr_time - m_map_lshare_tstamp[uname];
    double real_time_elapsed = elapsed / m_time_warp;
    if(real_time_elapsed < m_min_share_interval)
      ok_to_post = false;
  }
  
  if(!ok_to_post)
    return;

  // Post the report and note the time stamp
  m_map_lshare_tstamp[uname] = m_curr_time;
  string node_report = record.getSpec();
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
  // First check if the latest message for the given vehicle is valid.
  if(!message.valid()) {
    m_blk_msg_invalid++;
    return;
  }
  
  // If max_msg_length is limited (nonzero), check length of this msg
  if((m_max_msg_length > 0) && (message.length() > m_max_msg_length)) {
    m_blk_msg_toolong++;
    return;
  }

  string msg_color = m_msg_color;
  if(message.getColor() != "")
    msg_color = message.getColor();
  
  // Begin determining the list of destinations
  set<string> dest_names;

  string dest_name  = toupper(message.getDestNode());
  string dest_group = toupper(message.getDestGroup());
  
  // If destination name(s) given add each one in the colon-separated list
  vector<string> svector = parseString(dest_name, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string a_destination_name = stripBlankEnds(svector[i]);
    if(toupper(a_destination_name) != "ALL")
      dest_names.insert(a_destination_name);
  }

  // If a group name is specified, add all vehicles in that group to 
  // the list of destinations for this message.
  if((dest_group != "") || (dest_name == "ALL")) {
    map<string, string>::iterator p;
    for(p=m_map_vgroup.begin(); p!=m_map_vgroup.end(); p++) {
      string vname = p->first;
      string group = toupper(p->second);

      bool match = true;
      if(toupper(vname) == toupper(src_name))
	match = false;
      if((dest_group != "") && (group != dest_group))
	match = false;
      if(vname == "ALL")
	match = false;

      if(match)
	dest_names.insert(vname);
    }
  }
  // End determining the list of destinations
    
  // Begin handling all the destinations
  set<string>::iterator p;
  for(p=dest_names.begin(); p!=dest_names.end(); p++) {
    string a_dest_name = *p;

    bool msg_send = true;

    if(m_debug) 
      cout << src_name << "--->" << a_dest_name << ": " << endl;
  
    // Criteria #N: vehicles in same group if enforced
    if(m_apply_groups_msgs) {
      string src_grp  = m_map_record[src_name].getGroup();
      string dest_grp = m_map_record[a_dest_name].getGroup();
      if((src_grp == "") || (dest_grp == ""))
	msg_send = false;
      if(src_grp != dest_grp)
	msg_send = false;
    }

    // Criteria #1: vehicles different
    if(a_dest_name == src_name)
      msg_send = false;

    // Criteria #1B: Must have node records (position info) for both vehicles
    if(!m_map_record.count(toupper(src_name)) ||
       !m_map_record.count(toupper(a_dest_name))) {
      m_blk_msg_noinfo++;
      msg_send = false;
    }
    
    // Criteria #2: receiving vehicle has been heard from recently.
    // Freshness is enforced to disallow messages to a vehicle that may in
    // fact be much farther away than a stale node report would indicate
    // 2a - check if receiving vehicle has never been heard from
    if(msg_send && (m_map_time_nreport.count(a_dest_name) == 0))
      msg_send = false;
    
    // 2b - receiving vehicle has not been heard from recently
    if(msg_send) {
      double dest_name_time_since_nreport = m_curr_time;
      dest_name_time_since_nreport -= m_map_time_nreport[a_dest_name];
      if(dest_name_time_since_nreport > m_stale_time) {
	m_blk_msg_toostale++;
	msg_send = false;
      }
    }
    
    // Criteria #3: the range between vehicles is not too large.
    if(msg_send && !meetsRangeThresh(src_name, a_dest_name)) {
      m_blk_msg_toofar++;
      msg_send = false;
    }

    // Criteria #4: randomly drop messages, to probabilistically let 
    //              all but drop percentage through
    if(msg_send && meetsDropPercentage())
      msg_send = false;

    if(msg_send) {
      string moos_var = "NODE_MESSAGE_" + a_dest_name;
      if(message.getAckRequested())
	moos_var = "MEDIATED_MESSAGE_" + a_dest_name;

      string node_message = message.getSpec();
      Notify(moos_var, node_message);

      string id = message.getMessageID();
      if((id != "") && listContains(m_recent_ackids,id))
	postViewCommsPulse(src_name, a_dest_name, "msg", m_msg_repeat_color, 0.6);
      else
	postViewCommsPulse(src_name, a_dest_name, "msg", msg_color, 0.6);

      if(id != "") {
	m_recent_ackids.push_front(id);
	if(m_recent_ackids.size() > 100)
	  m_recent_ackids.pop_back();
      }
      

      m_total_messages_sent++;
      m_map_messages_sent[a_dest_name]++;
    }
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
  // First check if the latest message for the given vehicle is valid.
  if(!message.valid()) {
    m_blk_msg_invalid++;
    return;
  }
  
  // Begin determining the list of destinations
  string dest_name  = toupper(message.getDestNode());

  // Begin handling all the destinations
  bool msg_send = true;
  
  if(m_debug) 
    cout << src_name << "--->" << dest_name << ": " << endl;
  
  // Criteria #1B: Must have node records (position info) for both vehicles
  if(!m_map_record.count(toupper(src_name)) ||
     !m_map_record.count(toupper(dest_name))) {
    m_blk_msg_noinfo++;
    msg_send = false;
  }
    
  // Criteria #2: receiving vehicle has been heard from recently.
  // Freshness is enforced to disallow messages to a vehicle that may in
  // fact be much farther away than a stale node report would indicate
  // 2a - check if receiving vehicle has never been heard from
  if(msg_send && (m_map_time_nreport.count(dest_name) == 0))
    msg_send = false;
    
  // 2b - receiving vehicle has not been heard from recently
  if(msg_send) {
    double dest_name_time_since_nreport = m_curr_time;
    dest_name_time_since_nreport -= m_map_time_nreport[dest_name];
    if(dest_name_time_since_nreport > m_stale_time) {
      m_blk_msg_toostale++;
      msg_send = false;
    }
  }
    
  // Criteria #3: the range between vehicles is not too large.
  if(msg_send && !meetsRangeThresh(src_name, dest_name)) {
    m_blk_msg_toofar++;
    msg_send = false;
  }

  // Criteria #4: randomly drop messages, to probabilistically let 
  //              all but drop percentage through
  if(msg_send && meetsDropPercentage())
    msg_send = false;
  
  if(msg_send) {
    string moos_var = "ACK_MESSAGE_" + dest_name;

    string ack_message = message.getSpec();
    Notify(moos_var, ack_message);
    //postViewCommsPulse(src_name, dest_name, "msg", msg_color, 0.6);
    m_total_ack_messages_sent++;
    m_map_ack_messages_sent[dest_name]++;
  }
}


//------------------------------------------------------------
// Procedure: meetsRangeThresh
//   Purpose: Determine if Vehicle2 should hear the node report
//            of Vehicle1, given the raw range, earange and 
//            stealth factors.

bool FldNodeComms::meetsRangeThresh(const string& uname1,
				    const string& uname2)
{
  if(m_comms_range < 0)
    return(true);
  if(m_comms_range == 0)
    return(false);
  
  NodeRecord record1 = m_map_record[uname1];
  NodeRecord record2 = m_map_record[uname2];

  if(!record1.valid() || !record2.valid())
    return(false);

  double x1 = record1.getX();
  double y1 = record1.getY();
  double x2 = record2.getX();
  double y2 = record2.getY();
  double range = hypot((x1-x2), (y1-y2));

  // See if source vehicle has a modified stealth value 
  double stealth = 1.0;
  if(m_map_stealth.count(uname1))
    stealth = m_map_stealth[uname1];

  // See if receiving vehicle has a modified earange value
  double earange = 1.0;
  if(m_map_earange.count(uname2))
    earange = m_map_earange[uname2];

  // Compute the comms range threshold from baseline plus
  // stealth and earange factors.

  double comms_range = m_comms_range * stealth * earange;

  if(m_verbose) {
    cout << termColor("blue") << "  raw_range:" << range << endl; 
    cout << "  mod_rng:" << doubleToString(comms_range);
    cout << "  stealth:" << doubleToString(stealth);
    cout << "  earange:" << doubleToString(earange);
    cout << "  def_rng:" << doubleToString(m_comms_range);
    cout << termColor() << endl;
  }
    
  if(range > comms_range)
    return(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailFullReportReq()
//   Purpose: 

bool FldNodeComms::handleMailFullReportReq(string vname)
{
  string full_rpt_vname;
  set<string> full_rpt_waiting;

  vname = toupper(vname);
  
  map<string, NodeRecord>::iterator p;
  for(p=m_map_record.begin(); p!=m_map_record.end(); p++) {
    string this_name = toupper(p->first);
    if(this_name == vname)
      full_rpt_vname = vname;
    else
      full_rpt_waiting.insert(this_name);
  }

  if(full_rpt_vname == "")
    return(false);
  
  m_full_rpt_vname = full_rpt_vname;
  m_full_rpt_waiting = full_rpt_waiting;
  return(true);
}

//------------------------------------------------------------
// Procedure: meetsReportRateThresh()
//   Purpose: Determine if Vehicle2 should receive the node report
//            of Vehicle1, given the amount of time since the last
//            report sent from vehicle 1 to 2.

bool FldNodeComms::meetsReportRateThresh(const string& uname1,
					 const string& uname2)
{
  // Sanity check: If no rate limits enabled, just return true
  if(m_min_rpt_interval < 0)
    return(true);

  string key = tolower(uname1) + ":" + tolower(uname2);

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

bool FldNodeComms::meetsCriticalRangeThresh(const string& uname1,
					    const string& uname2)
{
  NodeRecord record1 = m_map_record[uname1];
  NodeRecord record2 = m_map_record[uname2];

  if(!record1.valid() || !record2.valid())
    return(false);

  double x1 = record1.getX();
  double y1 = record1.getY();
  double x2 = record2.getX();
  double y2 = record2.getY();
  double range = hypot((x1-x2), (y1-y2));

  if(range <= m_critical_range)
    return(true);
  return(false);
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
// Procedure: postViewCommsPulse()

void FldNodeComms::postViewCommsPulse(const string& uname1,
				      const string& uname2,
				      const string& pulse_type,
				      const string& pcolor,
				      double fill_opaqueness)
{
  if(m_pulse_duration <= 0)
    return;

  string lpcolor = tolower(pcolor);
  if((lpcolor == "invisible") || (lpcolor == "off"))
    return;
  
  if(uname1 == uname2)
    return;

  NodeRecord record1 = m_map_record[uname1];
  NodeRecord record2 = m_map_record[uname2];

  double x1 = record1.getX();
  double y1 = record1.getY();
  double x2 = record2.getX();
  double y2 = record2.getY();

  double beam_length = hypot(x1-x2, y1-y2);
  double beam_width = 0.1 * beam_length;
  if(beam_width < 5)
    beam_width = 5;
  
  XYCommsPulse pulse(x1, y1, x2, y2);
  
  string label = uname1 + "2" + uname2;

  if(pcolor != "auto")
    label += pcolor;

  pulse.set_duration(m_pulse_duration);
  pulse.set_label(label);
  pulse.set_time(m_curr_time);
  //pulse.set_beam_width(7);
  pulse.set_beam_width(beam_width);
  pulse.set_fill(fill_opaqueness);
  pulse.set_pulse_type(pulse_type);
  
  string pulse_color = pcolor;
  if(pcolor == "auto") {
    unsigned int color_index = m_map_vindex[uname1];
    if(color_index == 0)
      pulse_color = "orange";
    else if(color_index == 1)
      pulse_color = "green";
    else if(color_index == 2)
      pulse_color = "blue";
    else if(color_index == 3)
      pulse_color = "red";
    else 
      pulse_color = "purple";
  }

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
  m_msgs << "       Apply Group: " << boolToString(m_apply_groups)      << endl;
  m_msgs << "     Share Reports: " << share_rpt_string << endl;
  m_msgs << "      Ignore Group: " << m_ignore_group << endl;
  m_msgs << endl;

  double elapsed_app = (m_curr_time - m_start_time);
  string tot_rcvd = padString(uintToString(m_total_reports_rcvd),10,false);
  
  m_msgs << "Node Report Summary"                    << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "     Total Received: " << tot_rcvd << "Elapsed      Rate" << endl;
  map<string, unsigned int>::iterator p;
  for(p=m_map_reports_rcvd.begin(); p!=m_map_reports_rcvd.end(); p++) {
    string vname = p->first;
    unsigned int total = p->second;
    string total_str = padString(uintToString(total),7,false);

    string pad_vname  = padString(vname, 17);
    m_msgs << "  " << pad_vname << ": " << total_str;

    string elapsed_str = "n/a";
    if(m_map_time_nreport.count(vname) != 0) {
      double elapsed_time = m_curr_time - m_map_time_nreport[vname];
      elapsed_str = "(" + doubleToString(elapsed_time,1) + ")";
    }
    elapsed_str = padString(elapsed_str,8);

    string rate_str = "n/a";
    if(elapsed_app > 0) {
      double rate = (double)(total) / elapsed_app;
      rate_str = doubleToString(rate,2);
    }
    rate_str = "[" + rate_str + "]";
    rate_str = padString(rate_str, 14);
    
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



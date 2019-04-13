/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
// Constructor

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
  m_min_msg_interval = 30.0;
  m_max_msg_length   = 1000;    // zero means unlimited length

  m_stale_dropped    = 0;
  
  m_drop_pct = 0; // percentage of messages that should be dropped
  srand((int)time(0));// seed the random number generator

  m_verbose          = false;
  m_debug            = false;

  m_pulse_duration   = 10;      // zero means no pulses posted.
  m_view_node_rpt_pulses = true;

  // If true then comms between vehicles only happens if they are
  // part of the same group. (unless range is within critical).
  m_apply_groups     = false;

  m_total_reports_rcvd  = 0;
  m_total_reports_sent  = 0;
  m_total_messages_rcvd = 0;
  m_total_messages_sent = 0;

  m_blk_msg_invalid   = 0;
  m_blk_msg_toostale  = 0;
  m_blk_msg_tooquick  = 0;
  m_blk_msg_toolong   = 0;
  m_blk_msg_toofar    = 0;
  m_blk_msg_noinfo    = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool FldNodeComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

    bool   handled = false;
    string whynot;

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) 
      handled = handleMailNodeReport(sval, whynot);
    else if(key == "NODE_MESSAGE") 
      handled = handleMailNodeMessage(sval);
    else if(key == "UNC_STEALTH") 
      handled = handleStealth(sval);
    else if(key == "UNC_VIEW_NODE_RPT_PULSES") 
      handled = setBooleanOnString(m_view_node_rpt_pulses, sval);
    else if(key == "UNC_EARANGE") 
      handled = handleEarange(sval);

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
// Procedure: OnConnectToServer

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

  map<string, bool>::iterator p;
  for(p=m_map_newrecord.begin(); p!=m_map_newrecord.end(); p++) {
    string uname   = p->first;    
    bool   newinfo = p->second;
    if(newinfo)
      distributeNodeReportInfo(uname);
  }

  map<string, bool>::iterator p2;
  for(p2=m_map_newmessage.begin(); p2!=m_map_newmessage.end(); p2++) {
    string src_name = p2->first;    
    bool   newinfo  = p2->second;
    // If the message has changed, consider sending it out
    if(newinfo) 
      distributeNodeMessageInfo(src_name);
  }

  m_map_newrecord.clear();
  m_map_newmessage.clear();
  m_map_message.clear();

  clearStaleNodes();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            Happens before connection is open

bool FldNodeComms::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if((param == "COMMS_RANGE") && isNumber(value)) {
      // A negative comms range means all comms goes through
      // A zero comms range means nothing goes through
      m_comms_range = atof(value.c_str());
      handled = true;
    }
    else if((param == "CRITICAL_RANGE") && isNumber(value)) {
      m_critical_range = atof(value.c_str());
      handled = true;
    }
    else if((param == "STALE_TIME") && isNumber(value)) {
      // A negative stale time means staleness never applies
      m_stale_time = atof(value.c_str());
      handled = true;
    }
    else if((param == "MIN_MSG_INTERVAL") && isNumber(value)) {
      m_min_msg_interval = atof(value.c_str());
      handled = true;
    }
    else if((param == "MAX_MSG_LENGTH") && isNumber(value)) {
      m_max_msg_length = atoi(value.c_str());
      handled = true;
    }
    else if(param == "STEALTH") 
      handled = handleStealth(value);
    else if(param == "EARANGE") 
      handled = handleEarange(value);
    else if(param == "GROUPS") 
      handled  = setBooleanOnString(m_apply_groups, value);
    else if(param == "VERBOSE") 
      handled = setBooleanOnString(m_verbose, value);
    else if(param == "VIEW_NODE_RPT_PULSES") 
      handled = setBooleanOnString(m_view_node_rpt_pulses, value);
    else if(param == "DEBUG") 
      handled = setBooleanOnString(m_debug, value);
    else if(param == "DROP_PERCENTAGE") 
      handled = setPosDoubleOnString(m_drop_pct, value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

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
  map<string,double>::iterator p;

  vector<string> stale_vnames;
  
  for(p=m_map_time_nreport.begin(); p!=m_map_time_nreport.end(); p++) {
    string vname = p->first;
    double tstamp = p->second;
    if((m_curr_time - tstamp) > (m_stale_time*2)) {
      stale_vnames.push_back(vname);
      m_stale_dropped++;
    }    
  }

  for(unsigned int i=0; i<stale_vnames.size(); i++) {
    string vname = stale_vnames[i];
    m_map_record.erase(vname);
    m_map_time_nreport.erase(vname);
    m_map_time_nmessage.erase(vname);
    m_map_vindex.erase(vname);
    m_map_vgroup.erase(vname);
  }
}
  


//------------------------------------------------------------
// Procedure: registerVariables

void FldNodeComms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("NODE_MESSAGE", 0);
  Register("UNC_STEALTH", 0);
  Register("UNC_EARANGE", 0);
  Register("UNC_VIEW_NODE_RPT_PULSES", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport

bool FldNodeComms::handleMailNodeReport(const string& str, string& whynot)
{
  NodeRecord new_record = string2NodeRecord(str);
  if(!new_record.valid("x,y,name", whynot))
    return(false);
  
  string upp_name = toupper(new_record.getName());
  string grp_name = toupper(new_record.getGroup());

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
// Procedure: handleMailNodeMessage
//   Example: NODE_MESSAGE = src_node=henry,dest_node=ike,
//                           var_name=FOO, string_val=bar   

bool FldNodeComms::handleMailNodeMessage(const string& msg)
{
  NodeMessage new_message = string2NodeMessage(msg);

  // #1 List of "last" messages store solely for user debug 
  //    viewing at the console window.
  m_last_messages.push_back(msg);
  if(m_last_messages.size() > 5) 
    m_last_messages.pop_front();

  // #2 Check that the message is valid
  if(!new_message.valid())
    return(false);

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
// Procedure: handleStealth
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
// Procedure: handleEarange
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
// Procedure: distributeNodeReportInfo
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

    // Extra Criteria: If otherwise not sending, check to see if nodes
    // are within "critical" range. If so send report regardless of 
    // anything else - in the spirit of safety!
    if(!msg_send && meetsCriticalRangeThresh(uname, vname))
      msg_send = true;

      
    if(msg_send) {
      string moos_var = "NODE_REPORT_" + vname;
      Notify(moos_var, node_report);
      if(m_view_node_rpt_pulses)
	postViewCommsPulse(uname, vname);
      m_total_reports_sent++;
      m_map_reports_sent[vname]++;
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
// Procedure: distributeNodeMessageInfo
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
// Procedure: distributeNodeMessageInfo
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

  string msg_color = "white";
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
      string node_message = message.getSpec();
      Notify(moos_var, node_message);
      postViewCommsPulse(src_name, a_dest_name, "msg", msg_color, 0.6);
      m_total_messages_sent++;
      m_map_messages_sent[a_dest_name]++;
    }
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

  if(uname1 == uname2)
    return;

  NodeRecord record1 = m_map_record[uname1];
  NodeRecord record2 = m_map_record[uname2];

  double x1 = record1.getX();
  double y1 = record1.getY();
  double x2 = record2.getX();
  double y2 = record2.getY();

  XYCommsPulse pulse(x1, y1, x2, y2);
  
  string label = uname1 + "2" + uname2;

  if(pcolor != "auto")
    label += pcolor;

  pulse.set_duration(m_pulse_duration);
  pulse.set_label(label);
  pulse.set_time(m_curr_time);
  pulse.set_beam_width(7);
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
// Procedure: buildReport
//      Note: A virtual function of AppCastingMOOSApp superclass, conditionally 
//            invoked if either a terminal or appcast report is needed.

bool FldNodeComms::buildReport()
{
  m_msgs << "Configuration:                        " << endl;
  m_msgs << "       Comms Range: " << doubleToStringX(m_comms_range)    << endl;
  m_msgs << "    Critical Range: " << doubleToStringX(m_critical_range) << endl;
  m_msgs << "   Max Message Len: " << doubleToStringX(m_max_msg_length) << endl;
  m_msgs << "   Min Message Int: " << doubleToStringX(m_min_msg_interval) << endl;
  m_msgs << "       Apply Group: " << boolToString(m_apply_groups) << endl;
  m_msgs << endl;

  m_msgs << "Node Report Summary"                    << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "        Total Received: " << m_total_reports_rcvd << endl;
  map<string, unsigned int>::iterator p;
  for(p=m_map_reports_rcvd.begin(); p!=m_map_reports_rcvd.end(); p++) {
    string vname = p->first;
    unsigned int total = p->second;
    string pad_vname  = padString(vname, 20);
    m_msgs << "  " << pad_vname << ": " << total;

    double elapsed_time = m_curr_time - m_map_time_nreport[vname];
    string stime = "(" + doubleToString(elapsed_time,1) + ")";
    stime = padString(stime,12);
    m_msgs << stime << endl;
  }

  m_msgs << "     ------------------ " << endl;
  m_msgs << "            Total Sent: " << m_total_reports_sent << endl;
  map<string, unsigned int>::iterator p2;
  for(p2=m_map_reports_sent.begin(); p2!=m_map_reports_sent.end(); p2++) {
    string vname = p2->first;
    unsigned int total = p2->second;
    vname  = padString(vname, 20);
    m_msgs << "  " << vname << ": " << total << endl;
  }

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





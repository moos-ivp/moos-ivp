/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: FldNodeComms.cpp                                     */
/*   DATE: Dec 4th 2011                                         */
/****************************************************************/

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

  m_verbose          = true;
  m_debug            = false;

  m_pulse_duration   = 10;      // zero means no pulses posted.

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
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool FldNodeComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) {
      bool ok = handleMailNodeReport(sval);
      if(!ok && m_debug)
	MOOSTrace("Unhandled NODE_REPORT: %s \n", sval.c_str());
    }
    else if(key == "NODE_MESSAGE") {
      bool ok = handleMailNodeMessage(sval);
      if(m_debug) {
	if(!ok )
	  MOOSTrace("Unhandled NODE_MESSAGE: %s \n", sval.c_str());
	else
	  MOOSTrace("Handled NODE_MESSAGE: %s \n", sval.c_str());
      }
    }
    else if(key == "UNC_STEALTH") {
      bool ok = handleStealth(sval);
      if(!ok && m_debug)
	MOOSTrace("Unhandled UNC_STEALTH: %s \n", sval.c_str());
    }
    else if(key == "UNC_EARANGE") {
      bool ok = handleEarange(sval);
      if(!ok && m_debug)
	MOOSTrace("Unhandled UNC_EARANGE: %s \n", sval.c_str());
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
  m_curr_time = MOOSTime();

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
    if(newinfo) {
      // Check if message interval is sufficiently long
      double elapsed = m_curr_time - m_map_time_nmessage[src_name];
      if(elapsed >= m_min_msg_interval) {
	distributeNodeMessageInfo(src_name);
	m_map_time_nmessage[src_name] = m_curr_time;
      }
      else
	m_blk_msg_tooquick++;
    }
  }

  m_map_newrecord.clear();
  m_map_newmessage.clear();

  printReport();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            Happens before connection is open

bool FldNodeComms::OnStartUp()
{
  m_curr_time = MOOSTime();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "COMMS_RANGE") {
      // A negative comms range means all comms goes through
      // A zero comms range means nothing goes through
      if(isNumber(value))
	m_comms_range = atof(value.c_str());
    }
    else if(param == "CRITICAL_RANGE") {
      if(isNumber(value))
	m_critical_range = atof(value.c_str());
    }
    else if(param == "STALE_TIME") {
      // A negative stale time means staleness never applies
      if(isNumber(value))
	m_stale_time = atof(value.c_str());
    }
    else if(param == "MIN_MSG_INTERVAL") {
      if(isNumber(value))
	m_min_msg_interval = atof(value.c_str());
    }
    else if(param == "MAX_MSG_LENGTH") {
      if(isNumber(value))
	m_max_msg_length = atoi(value.c_str());
    }
    else if(param == "STEALTH") {
      bool ok = handleStealth(value);
      if(!ok)
	cout << termColor("red") << "Improper Node Stealth config: " << value;
    }
    else if(param == "EARANGE") {
      bool ok = handleEarange(value);
      if(!ok)
	cout << termColor("red") << "Improper Node Earange config: " << value;
    }
    else if(param == "GROUPS") {
      bool ok = setBooleanOnString(m_apply_groups, value);
      if(!ok) {
	cout << termColor("red");
	cout << "Improper config: GROUPS should be \"true\" or \"false\".";
      }
    }
    else if(param == "VERBOSE") {
      bool ok = setBooleanOnString(m_verbose, value);
      if(!ok) {
	cout << termColor("red");
	cout << "Improper config: VERBOSE should be \"true\" or \"false\".";
      }
    }
    else if(param == "DEBUG") {
      bool ok = setBooleanOnString(m_debug, value);
      if(!ok) {
	cout << termColor("red");
	cout << "Improper config: DEBUG should be \"true\" or \"false\".";
      }
    }
    cout << termColor() << endl;
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void FldNodeComms::registerVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("NODE_MESSAGE", 0);
  m_Comms.Register("UNC_STEALTH", 0);
  m_Comms.Register("UNC_EARANGE", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport

bool FldNodeComms::handleMailNodeReport(const string& str)
{
  NodeRecord new_record = string2NodeRecord(str);
  if(!new_record.valid())
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

bool FldNodeComms::handleMailNodeMessage(const string& msg)
{
  NodeMessage new_message = string2NodeMessage(msg);

  cout << "Handling new Node Message: " << new_message.getSpec() << endl;

  // List of "last" messages store solely for user debug viewing at
  // the console window.
  m_last_messages.push_back(msg);
  if(m_last_messages.size() > 5) 
    m_last_messages.pop_front();

  if(!new_message.valid())
    return(false);

  string upp_src_node = toupper(new_message.getSourceNode());

  m_map_message[upp_src_node]    = new_message;
  m_map_newmessage[upp_src_node] = true;

  unsigned int vindex_size = m_map_vindex.size();
  if(m_map_vindex.count(upp_src_node) == 0)
    m_map_vindex[upp_src_node] = vindex_size;

  if(m_map_messages_rcvd.count(upp_src_node) == 0)
    m_map_messages_rcvd[upp_src_node] = 1;
  else
    m_map_messages_rcvd[upp_src_node]++;
  m_total_messages_rcvd++;

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
    
    // Extra Criteria: If otherwise not sending, check to see if nodes
    // are within "critical" range. If so send report regardless of 
    // anything else - in the spirit of safety!
    if(!msg_send && meetsCriticalRangeThresh(uname, vname))
      msg_send = true;

      
    if(msg_send) {
      string moos_var = "NODE_REPORT_" + vname;
      m_Comms.Notify(moos_var, node_report);
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
//   Purpose: Post the node message for vehicle <uname> to the  
//            recipients specified in the message. 
//     Notes: The recipient may be specified by the name of the recipient
//            node, or the vehicle/node group. Group membership is 
//            determined by nodes reporting their group membership as
//            part of their node report.
//     Notes: Inter-vehicle node reports must pass certain criteria
//            based on inter-vehicle range, group, and the respective
//            stealth and earange of the senting and receiving nodes.

void FldNodeComms::distributeNodeMessageInfo(const string& src_name)
{
  // First check if the latest message for the given vehicle is valid.
  NodeMessage message = m_map_message[src_name];
  if(!message.valid()) {
    m_blk_msg_invalid++;
    return;
  }
  
  // If max_msg_length is limited (nonzero), check length of this msg
  if((m_max_msg_length > 0) && (message.length() > m_max_msg_length)) {
    m_blk_msg_toolong++;
    return;
  }

  // Begin determining the list of destinations
  set<string> dest_names;

  string dest_name  = toupper(message.getDestNode());
  string dest_group = message.getDestGroup();
  
  // If destination name(s) given add each one in the colon-separated list
  vector<string> svector = parseString(dest_name, ':');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string a_destination_name = stripBlankEnds(svector[i]);
    dest_names.insert(a_destination_name);
  }

  // If a group name is specified, add all vehicles in that group to 
  // the list of destinations for this message. 
  if((dest_group != "") || (dest_name == "ALL")) {
    map<string, string>::iterator p;
    for(p=m_map_vgroup.begin(); p!=m_map_vgroup.end(); p++) {
      string vname = p->first;
      string group = p->second;
      if((dest_group == group) || (dest_group == "all") || (dest_name=="ALL"))
	dest_names.insert(vname);
    }
  }
  // End determining the list of destinations
    
  // Begin handling all the destinations
  set<string>::iterator p;
  for(p=dest_names.begin(); p!=dest_names.end(); p++) {
    string a_dest_name = *p;

    if(m_debug) 
      cout << src_name << "--->" << a_dest_name << ": " << endl;
  
    bool msg_send = true;

    // Criteria #1: vehicles different
    if(a_dest_name == src_name)
      msg_send = false;
    
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

    if(msg_send) {
      string moos_var = "NODE_MESSAGE_" + a_dest_name;
      string node_message = message.getSpec();
      m_Comms.Notify(moos_var, node_message);
      postViewCommsPulse(src_name, a_dest_name, "white");
      m_total_messages_sent++;
      m_map_messages_sent[a_dest_name]++;
    }

    if(m_verbose) {
      if(msg_send)
	cout << "NODE_MESSAGE Sent!" << endl;
      else
	cout << "NODE_MESSAGE Held!" << endl;
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
// Procedure: meetsCriticalRangeThresh
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
// Procedure: postViewCommsPulse
//   Purpose: 

void FldNodeComms::postViewCommsPulse(const string& uname1,
				      const string& uname2,
				      const string& pcolor)
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

  if(m_verbose)
    cout << "Pulse: From: " << uname1 << "   TO: " << uname2 << endl;

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
  m_Comms.Notify("VIEW_COMMS_PULSE", pulse_spec);
}


//------------------------------------------------------------
// Procedure: printReport
//   Purpose: 

void FldNodeComms::printReport()
{
  if(!m_verbose) {
    cout << "*" << flush;
    return;
  }

  cout << endl << endl << endl;
  cout << "*****************************************" << endl;
  cout << "uFldNodeCommsSummary:                    " << endl;
  cout << "*****************************************" << endl;
  cout << "                                         " << endl;
  cout << "Node Report Summary                      " << endl;
  cout << "======================================   " << endl;

  cout << "        Total Received: " << m_total_reports_rcvd << endl;
  map<string, unsigned int>::iterator p;
  for(p=m_map_reports_rcvd.begin(); p!=m_map_reports_rcvd.end(); p++) {
    string vname = p->first;
    unsigned int total = p->second;
    string pad_vname  = padString(vname, 20);
    cout << "  " << pad_vname << ": " << total;

    double elapsed_time = m_curr_time - m_map_time_nreport[vname];
    string stime = "(" + doubleToString(elapsed_time,1) + ")";
    stime = padString(stime,12);
    cout << stime << endl;
  }

  cout << "     ------------------ " << endl;
  cout << "            Total Sent: " << m_total_reports_sent << endl;
  map<string, unsigned int>::iterator p2;
  for(p2=m_map_reports_sent.begin(); p2!=m_map_reports_sent.end(); p2++) {
    string vname = p2->first;
    unsigned int total = p2->second;
    vname  = padString(vname, 20);
    cout << "  " << vname << ": " << total << endl;
  }


  cout << endl << endl;
  cout << "Node Message Summary                      " << endl;
  cout << "======================================   " << endl;

  cout << "    Total Msgs Received: " << m_total_messages_rcvd << endl;
  map<string, unsigned int>::iterator q;
  for(q=m_map_messages_rcvd.begin(); q!=m_map_messages_rcvd.end(); q++) {
    string vname = q->first;
    unsigned int total = q->second;
    string pad_vname  = padString(vname, 20);
    cout << "  " << pad_vname << ": " << total;

    double elapsed_time = m_curr_time - m_map_time_nmessage[vname];
    string stime = "(" + doubleToString(elapsed_time,1) + ")";
    stime = padString(stime,12);
    cout << stime << endl;
  }

  cout << "     ------------------ " << endl;
  cout << "            Total Sent: " << m_total_messages_sent << endl;
  map<string, unsigned int>::iterator q2;
  for(q2=m_map_messages_sent.begin(); q2!=m_map_messages_sent.end(); q2++) {
    string vname = q2->first;
    unsigned int total = q2->second;
    vname  = padString(vname, 20);
    cout << "  " << vname << ": " << total << endl;
  }
  
  unsigned int total_blk_msgs = m_blk_msg_invalid + m_blk_msg_toostale + 
    m_blk_msg_tooquick + m_blk_msg_toolong + m_blk_msg_toofar;
  string blk_msg_invalid  = uintToString(m_blk_msg_invalid);
  string blk_msg_toostale = uintToString(m_blk_msg_toostale);
  string blk_msg_tooquick = uintToString(m_blk_msg_tooquick);
  string blk_msg_toolong  = uintToString(m_blk_msg_toolong);
  string blk_msg_toofar   = uintToString(m_blk_msg_toofar);
  
  cout << "     ------------------ " << endl;
  cout << "    Total Blocked Msgs: " << total_blk_msgs << endl;
  cout << "               Invalid: " << blk_msg_invalid << endl;
  cout << "        Stale Receiver: " << blk_msg_toostale << endl;
  cout << "            Too Recent: " << blk_msg_tooquick<< endl;
  cout << "          Msg Too Long: " << blk_msg_toolong << endl;
  cout << "         Range Too Far: " << blk_msg_toofar << endl;

  cout << "     ------------------ " << endl;
  cout << "             Last Msgs: " << endl;
  list<string>::iterator p3;
  for(p3 = m_last_messages.begin(); p3 != m_last_messages.end(); p3++) {
    string msg = *p3;
    cout << msg << endl;
  }
}

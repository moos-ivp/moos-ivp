/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: FldNodeComms.cpp                                     */
/*   DATE: Dec 4th 2011                                         */
/****************************************************************/

#include <cmath>
#include <iterator>
#include "FldNodeComms.h"
#include "MBUtils.h"
#include "NodeRecordUtils.h"
#include "XYCommsPulse.h"
#include "ColorParse.h"

using namespace std;

//---------------------------------------------------------
// Constructor

FldNodeComms::FldNodeComms()
{
  // The default range within which reports are sent between nodes
  m_comms_range     = 100;

  // A range with which reports will be sent between nodes regardless
  // of all other criteria (in the spirit of collision avoidance).
  m_critical_range  = 30;

  m_default_stealth = 1.0;
  m_default_earange = 1.0;
  m_min_stealth     = 0.1;  
  m_max_earange     = 10.0;

  m_stale_time      = 5.0;
  m_delete_time     = 10.0;
  m_verbose         = true;

  // If true then comms between vehicles only happens if they are
  // part of the same group. (unless range is within critical).
  m_apply_groups    = false;
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
      bool ok = handleNodeReport(sval);
      if(!ok)
	MOOSTrace("Unhandled NODE_REPORT: %s \n", sval.c_str());
    }
    else if(key == "UNC_STEALTH") {
      bool ok = handleStealth(sval);
      if(!ok)
	MOOSTrace("Unhandled UNC_STEALTH: %s \n", sval.c_str());
    }
    else if(key == "UNC_EARANGE") {
      bool ok = handleEarange(sval);
      if(!ok)
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
  for(p=m_map_newinfo.begin(); p!=m_map_newinfo.end(); p++) {
    string uname   = p->first;    
    bool   newinfo = p->second;
    if(newinfo)
      distributeNodeInfo(uname);
  }

  m_map_newinfo.clear();
  checkForDeletableNodes();
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
    else if(param == "STALE_TIME") {
      // A negative stale time means staleness never applies
      if(isNumber(value))
	m_stale_time = atof(value.c_str());
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
  m_Comms.Register("UNC_STEALTH", 0);
  m_Comms.Register("UNC_EARANGE", 0);
}


//------------------------------------------------------------
// Procedure: checkForDeletableNodes

void FldNodeComms::checkForDeletableNodes()
{
  vector<string> deletable_nodes;

  map<string, double>::iterator p;
  for(p=m_map_timestamp.begin(); p!=m_map_timestamp.end(); p++) {
    string vname = p->first;
    double time  = p->second;
    if((m_curr_time - time) > m_delete_time)
      deletable_nodes.push_back(vname);
  }

  unsigned int i, vsize = deletable_nodes.size();
  for(i=0; i<vsize; i++) {
    string vname = deletable_nodes[i];
    m_map_records.erase(vname);
    m_map_newinfo.erase(vname);
    m_map_timestamp.erase(vname);
    m_map_vindex.erase(vname);
    m_map_vgroup.erase(vname);
  }
}


//------------------------------------------------------------
// Procedure: handleNodeReport

bool FldNodeComms::handleNodeReport(const string& str)
{
  NodeRecord new_record = string2NodeRecord(str);
  if(!new_record.valid())
    return(false);

  string upp_name = toupper(new_record.getName());
  string grp_name = toupper(new_record.getGroup());

  m_map_records[upp_name] = new_record;
  m_map_newinfo[upp_name] = true;
  m_map_timestamp[upp_name] = m_curr_time;
  m_map_vgroup[upp_name]  = grp_name;

  unsigned int vindex_size = m_map_vindex.size();
  if(m_map_vindex.count(upp_name) == 0)
    m_map_vindex[upp_name] = vindex_size;

  return(true);
}

//------------------------------------------------------------
// Procedure: handleStealth
//   Example: "alpha, 0.4"
//            "alpha = 0.4"
//      Note: Max value is 1.0. Min value set by m_min_stealth

bool FldNodeComms::handleStealth(const string& str)
{
  string nstr = findReplace(str, '=', ',');
  vector<string> svector = parseString(nstr, ',');

  if(svector.size() != 2)
    return(false);

  if(!isNumber(svector[1]))
    return(false);

  string uname   = toupper(svector[0]);
  double stealth = atof(svector[1].c_str());
  if(stealth < m_min_stealth)
    stealth = m_min_stealth;
  if(stealth > 1)
    stealth = 1;

  m_map_stealth[uname] = stealth;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleEarange
//   Example: "alpha, 1.4"
//            "alpha = 1.4"
//      Note: Min value is 1.0. Max value set by m_max_earange

bool FldNodeComms::handleEarange(const string& str)
{
  string nstr = findReplace(str, '=', ',');
  vector<string> svector = parseString(nstr, ',');

  if(svector.size() != 2)
    return(false);

  if(!isNumber(svector[1]))
    return(false);

  string uname   = toupper(svector[0]);
  double earange = atof(svector[1].c_str());
  if(earange > m_max_earange)
    earange = m_max_earange;
  if(earange < 1)
    earange = 1;

  m_map_earange[uname] = earange;
  return(true);
}


//------------------------------------------------------------
// Procedure: distributeNodeInfo
//   Purpose: Post the node report for vehicle <uname> to all 
//            other vehicles as NODE_REPORT_VNAME where <uname>
//            is not equal to VNAME (no need to report to self).

void FldNodeComms::distributeNodeInfo(const string& uname)
{
  // First check if the latest record for the given vehicle is valid.
  NodeRecord record = m_map_records[uname];
  if(!record.valid())
    return;

  // We'll need the same node report sent out to all vehicles.
  string node_report = record.getSpec();

  map<string, NodeRecord>::iterator p;
  for(p=m_map_records.begin(); p!=m_map_records.end(); p++) {
    string vname = p->first;

    // Criteria #1: vehicles different
    if(vname == uname)
      continue;

    if(m_verbose) 
      cout << uname << "--->" << vname << ": " << endl;

    bool msg_send = true;
    
    // Criteria #2: receiving vehicle has been heard from recently.
    double vname_time_since_update = m_curr_time - m_map_timestamp[vname];
    if(vname_time_since_update > m_stale_time)
      msg_send = false;
    
    // Criteria #3: the range between vehicles is not too large.
    if(msg_send && !meetsRangeThresh(uname, vname))
      msg_send = false;
    
    
    // Criteria #4: if groups considered, check for same group
    if(msg_send && m_apply_groups) {
      cout << "  uname group:" << m_map_vgroup[uname] << endl;
      cout << "  vname group:" << m_map_vgroup[vname] << endl;
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
    }

    if(m_verbose) {
      if(msg_send)
	cout << "Sent!" << endl;
      else
	cout << "Held!" << endl;
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
  NodeRecord record1 = m_map_records[uname1];
  NodeRecord record2 = m_map_records[uname2];

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
  NodeRecord record1 = m_map_records[uname1];
  NodeRecord record2 = m_map_records[uname2];

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
				      const string& uname2)
{
  if(uname1 == uname2)
    return;

  NodeRecord record1 = m_map_records[uname1];
  NodeRecord record2 = m_map_records[uname2];

  double x1 = record1.getX();
  double y1 = record1.getY();
  double x2 = record2.getX();
  double y2 = record2.getY();

  XYCommsPulse pulse(x1, y1, x2, y2);
  
  string label = uname1 + "2" + uname2;
  pulse.set_label(label);
  pulse.set_time(m_curr_time);
  pulse.set_beam_width(7);

  cout << "Pulse: From: " << uname1 << "   TO: " << uname2 << endl;

  unsigned int color_index = m_map_vindex[uname1];
  if(color_index == 0)
    pulse.set_color("fill", "orange");
  else if(color_index == 1)
    pulse.set_color("fill", "green");
  else if(color_index == 2)
    pulse.set_color("fill", "blue");
  else if(color_index == 3)
    pulse.set_color("fill", "red");
  else 
    pulse.set_color("fill", "purple");

  string pulse_spec = pulse.get_spec();
  m_Comms.Notify("VIEW_COMMS_PULSE", pulse_spec);
}

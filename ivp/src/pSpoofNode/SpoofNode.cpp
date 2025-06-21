/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: SpoofNode.cpp                                   */
/*    DATE: May 16th, 2022                                  */
/*    DATE: June 17th, 2025 Mods for supporting speed/adv   */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SpoofNode.h"
#include "ColorParse.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

SpoofNode::SpoofNode()
{
  // Config variables
  m_refresh_interval = 0.5;
  m_default_vtype = "kayak";
  m_default_group = "";
  m_default_color = "purple";
  m_default_length = 5;
  m_default_vsource = "";

  m_default_hdg = 45;
  m_default_spd = 0;
  m_default_duration = 0;
  
  // State variables
  m_last_posting = 0;
  m_total_postings = 0;
}

//-----------------------------------------------------------
// Procedure: OnNewMail()

bool SpoofNode::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 
    
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = false;
    if(key == "SPOOF") 
      handled = handleSpoofRequest(sval);
    else if(key == "SPOOF_CANCEL") 
      handled = handleMailSpoofCancel(sval);
    
    if(!handled && (key != "APPCAST_REQ")) // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: OnConnectToServer()

bool SpoofNode::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-----------------------------------------------------------
// Procedure: Iterate()

bool SpoofNode::Iterate()
{
  AppCastingMOOSApp::Iterate();

  double elapsed = m_curr_time - m_last_posting;
  if(elapsed > m_refresh_interval) {  
    advancePositions(elapsed);
    postNodeReports();
    m_last_posting = m_curr_time;
  }

  checkForExpiredNodes();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//-----------------------------------------------------------
// Procedure: OnStartUp()

bool SpoofNode::OnStartUp()
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
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "refresh_interval") 
      handled = setNonNegDoubleOnString(m_refresh_interval, value);
    else if(param == "default_length") 
      handled = setPosDoubleOnString(m_default_length, value);
    else if(param == "default_vtype") 
      handled = handleConfigDefaultVType(value);
    else if(param == "default_group") 
      handled = setNonWhiteVarOnString(m_default_group, value);
    else if(param == "default_vsource") 
      handled = setNonWhiteVarOnString(m_default_vsource, value);
    else if(param == "default_color")
      handled = setColorOnString(m_default_color, value);

    else if(param == "default_hdg") 
      handled = setDoubleOnString(m_default_hdg, value);
    else if(param == "default_spd") 
      handled = setPosDoubleOnString(m_default_spd, value);
    else if(param == "default_duration") 
      handled = setPosDoubleOnString(m_default_duration, value);

    if(!handled && (param != "spoof"))
      reportUnhandledConfigWarning(orig);
  }

  // Handle the spoof requests AFTER all the default values are set
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "spoof") 
      handled = handleSpoofRequest(value);
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();	
  return(true);
}

//-----------------------------------------------------------
// Procedure: registerVariables()

void SpoofNode::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SPOOF", 0);
  Register("SPOOF_CANCEL", 0);
}


//-----------------------------------------------------------
// Procedure: advancePositions()

void SpoofNode::advancePositions(double elapsed)
{
  map<string, NodeRecord> new_map;
  
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    NodeRecord record = p->second;

    string vname = record.getName();
    double dist = record.getSpeed() * elapsed;
    if(dist != 0) {
      double osx = record.getX();
      double osy = record.getY();
      double osh = record.getHeading();
      double new_osx, new_osy;
      projectPoint(osh, dist, osx, osy, new_osx, new_osy);
      record.setX(new_osx);
      record.setY(new_osy);
    }
    new_map[vname] = record;
  }

  m_map_node_records = new_map;
}


//-----------------------------------------------------------
// Procedure: postNodeReports()

void SpoofNode::postNodeReports()
{
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    NodeRecord record = p->second;
    record.setTimeStamp(m_curr_time);
    Notify("NODE_REPORT", record.getSpec());
    m_total_postings++;
  }
}


//-----------------------------------------------------------
// Procedure: handleConfigDefaultType()
//   Options: kayak, auv, ship, heron, glider, wamv, longship etc

bool SpoofNode::handleConfigDefaultVType(string str)
{
  if(!isKnownVehicleType(str))
    return(false);

  m_default_vtype = str;
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleSpoofRequest()
//   Example: x=23, y=34
//   Example: x=23, y=34, name=val, group=blue, type=heron, color=blue

bool SpoofNode::handleSpoofRequest(string str)
{
  str = tolower(str);
  
  string xval  = tokStringParse(str, "x");
  string yval  = tokStringParse(str, "y");
  string hdg   = tokStringParse(str, "hdg");
  string spd   = tokStringParse(str, "spd");
  string len   = tokStringParse(str, "len");
  string vname = tokStringParse(str, "name");
  string group = tokStringParse(str, "group");
  string vtype = tokStringParse(str, "type");
  string color = tokStringParse(str, "color");
  string vsource = tokStringParse(str, "vsource");
  string duration = tokStringParse(str, "dur");

  if(!isNumber(xval) || !isNumber(yval))
    return(false);

  if((vname != "") && !isAlphaNum(vname))
    return(false);
  if((group != "") && !isAlphaNum(group))
    return(false);
  if((vtype != "") && !isAlphaNum(vtype))
    return(false);
  if((vsource != "") && !isAlphaNum(vsource))
    return(false);

  if((hdg != "") && !isNumber(hdg))
    return(false);
  if((spd != "") && !isNumber(spd))
    return(false);
  if((len != "") && !isNumber(len))
    return(false);

  if((duration != "") && !isNumber(duration))
    return(false);

  double dbl_hdg = m_default_hdg;
  if(isNumber(hdg))
    dbl_hdg = atof(hdg.c_str());
  double dbl_spd = m_default_spd;
  if(isNumber(spd))
    dbl_spd = atof(spd.c_str());
  double dbl_duration = m_default_duration;
  if(isNumber(duration))
    dbl_duration = atof(duration.c_str());

  NodeRecord record;
  record.setX(atof(xval.c_str()));
  record.setY(atof(yval.c_str()));
  record.setHeading(dbl_hdg);
  record.setSpeed(dbl_spd);
  record.setLength(m_default_length);
  
  if(vname == "")
    vname = "C" + uintToString(m_total_spoof_reqs, 4);
  if(vtype == "")
    vtype = m_default_vtype;
  if(color == "")
    color = m_default_color;
  if(color == "")
    color = m_default_color;

  //double utc = m_curr_time;
  //if(time != "")
  //  utc = atof(time.c_str());
  //record.setTimeStamp(m_curr_time);
  
  record.setName(vname);
  record.setColor(color);
  record.setType(vtype);

  if(group != "")
    record.setGroup(group);
  if(vsource != "")
    record.setVSource(vsource);
  
  m_map_node_records[vname]   = record;
  m_map_node_durations[vname] = dbl_duration;
  m_map_spoof_start[vname]    = m_curr_time;

  m_total_spoof_reqs++; 
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: handleMailSpoofCancel()
//   Example: vname=alpha
//   Example: group=redteam

bool SpoofNode::handleMailSpoofCancel(string str)
{
  str = tolower(str);

  string vname = tokStringParse(str, "vname");
  string group = tokStringParse(str, "group");

  if(vname == "")
    vname = tokStringParse(str, "contact");
      
  if(vname != "") {
    m_map_node_records.erase(vname);
    m_map_node_durations.erase(vname);
    m_map_spoof_start.erase(vname);
  }

  if(group != "") {
    map<string, NodeRecord>::iterator p;
    for(p=m_map_node_records.begin(); p!= m_map_node_records.end(); ) {
      string pname = p->first;
      NodeRecord precord = p->second;
      string pgroup = precord.getGroup();
      if(pgroup == group) {
	p = m_map_node_records.erase(p);
	m_map_node_durations.erase(pname);
	m_map_spoof_start.erase(pname);
      }
      else
	++p;
    }
  }
  
  return(true);
}


//-----------------------------------------------------------
// Procedure: checkForExpiredNodes()

void SpoofNode::checkForExpiredNodes()
{
  set<string> expired_nodes;
  
  map<string,double>::iterator p;
  for(p=m_map_spoof_start.begin(); p!=m_map_spoof_start.end(); p++) {
    string vname = p->first;
    double start = p->second;

    double duration = m_map_node_durations[vname];
    if(duration <= 0)
      continue;

    double elapsed = m_curr_time - start;
    if(elapsed > duration)
      expired_nodes.insert(vname);
  }

  set<string>::iterator q;
  for(q=expired_nodes.begin(); q!=expired_nodes.end(); q++) {
    string vname = *q;
    m_map_node_records.erase(vname);
    m_map_node_durations.erase(vname);
    m_map_spoof_start.erase(vname);
  }

}
    

  

//-----------------------------------------------------------
// Procedure: buildReport()

bool SpoofNode::buildReport() 
{
  m_msgs << "Config:                                  " << endl;
  m_msgs << "  Default Type:    " << m_default_vtype     << endl;
  m_msgs << "  Default Color:   " << m_default_color     << endl;
  m_msgs << "  Default Group:   " << m_default_group     << endl;
  m_msgs << "  Default VSource: " << m_default_vsource   << endl;
  m_msgs << "  refresh Interv:  " << doubleToStringX(m_refresh_interval,2) << endl;
  m_msgs << endl;

  m_msgs << "Postings: " << uintToString(m_total_postings) << endl;
  m_msgs << endl;
  
  ACTable actab(6);
  actab << "VName | XPos | YPos | Hdg | Spd | Age ";
  actab.addHeaderLines();

  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;
    NodeRecord record = p->second;
    string xpos = doubleToStringX(record.getX(),2);
    string ypos = doubleToStringX(record.getY(),2);
    string hdg  = doubleToStringX(record.getHeading(),2);
    string spd  = doubleToStringX(record.getSpeed(),2);
    double dbl_age = m_curr_time - record.getTimeStamp();
    string str_age = doubleToStringX(dbl_age, 2);

    actab << vname << xpos << ypos << hdg << spd << str_age;
  }
  
  m_msgs << actab.getFormattedString();

  return(true);
}

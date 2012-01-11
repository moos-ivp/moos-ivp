/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PathCheck_MOOSApp.cpp                                */
/*   DATE: Nov 22th 2011                                        */
/****************************************************************/

#include <cmath>
#include <iterator>
#include "PathCheck_MOOSApp.h"
#include "MBUtils.h"
#include "NodeRecordUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PathCheck_MOOSApp::PathCheck_MOOSApp()
{
  m_history_size = 10;
}

//---------------------------------------------------------
// Destructor

PathCheck_MOOSApp::~PathCheck_MOOSApp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PathCheck_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key   = msg.GetKey();

    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();
    
    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) {
      string sval  = msg.GetString(); 
      NodeRecord record = string2NodeRecord(sval);
      bool ok = addNodeRecord(record);
      if(!ok)
	MOOSTrace("Invalid NODE_REPORT: %s\n", sval.c_str());
    }
    else if(key == "UPC_TRIP_RESET") {
      string sval  = msg.GetString(); 
      bool ok = handleTripReset(sval);
      if(!ok)
	MOOSTrace("Invalid TRIP_RESET: %s\n", sval.c_str());
    }
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PathCheck_MOOSApp::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool PathCheck_MOOSApp::Iterate()
{
  cout << "*" << flush;
  computeAndPostSpeeds();
  detectAndPostOdometry();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool PathCheck_MOOSApp::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "HISTORY_LENGTH") {
      bool valid_length = true;
      if(!isNumber(value))
	valid_length = false;
      else {
	int length = atoi(value.c_str());
	if(length > 1) 
	  m_history_size = (unsigned int)(length);
	else
	  valid_length = false;
      }
      if(!valid_length) {
	MOOSTrace("Improper HISTORY_LENGTH: %s\n", value.c_str());
	return(false);
      }
    }
  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: addNodeRecord

bool PathCheck_MOOSApp::addNodeRecord(const NodeRecord& record)
{
  if(!record.valid())
    return(false);
  
  string vname = record.getName();
  
  // If there is previous record for this vehicle, calculate the
  // delta distance, add it to the odometers.
  if(m_map_records[vname].size() > 1) {
    const NodeRecord& prev_record = m_map_records[vname].back();
    double xdist = prev_record.getX() - record.getX();
    double ydist = prev_record.getY() - record.getY();
    double dist = hypot(xdist, ydist);
    m_map_total_dist[vname] += dist;
    m_map_trip_dist[vname] += dist;
    m_map_changed_dist[vname] = true;
  }
  
  // Now push back the record for later speed calculations.
  m_map_records[vname].push_back(record);

  // Keep the size of the record history limited.
  if(m_map_records[vname].size() > m_history_size)
    m_map_records[vname].pop_front();

  return(true);
}

//---------------------------------------------------------
// Procedure: handleTripReset

bool PathCheck_MOOSApp::handleTripReset(string reset_vname)
{
  // If there is nothing known about this vehicle, just return false
  if(m_map_trip_dist.count(reset_vname) == 0)
    return(false);

  m_map_trip_dist[reset_vname] = 0;
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void PathCheck_MOOSApp::registerVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("UPC_TRIP_RESET", 0);
}


//---------------------------------------------------------
// Procedure: computeAndPostSpeeds

void PathCheck_MOOSApp::computeAndPostSpeeds()
{
  map<string, list<NodeRecord> >::iterator p1 = m_map_records.begin();
  while(p1 != m_map_records.end()) {
    const string& vname = p1->first;
    const list<NodeRecord>& list_records = p1->second;
    // Cannot compute speed unless there are at least two records.
    if(list_records.size() > 1) {
      double prev_x, prev_y, prev_ts, total_speed = 0;
      unsigned int legs = 0;
      bool first_record = true;
      list<NodeRecord>::const_iterator p2 = list_records.begin();
      while(p2 != list_records.end()) {
	double this_x = p2->getX();
	double this_y = p2->getY();
	double this_ts = p2->getTimeStamp();
	if(!first_record) {
	  legs++;
	  double xdelta  = this_x  - prev_x;
	  double ydelta  = this_y  - prev_y;
	  double tsdelta = this_ts - prev_ts;
	  double dist = hypot(xdelta, ydelta);
	  double spd  = dist / tsdelta;
	  total_speed += spd;
	}
	first_record = false;
	prev_x = this_x;
	prev_y = this_y;
	prev_ts = this_ts;
	p2++;
      }
      double avg_speed = (total_speed / (double)(legs));
      string report = "vname=" + vname;
      report += ",avg_spd=" + doubleToString(avg_speed,2);
      m_Comms.Notify("UPC_SPEED_REPORT", report);
    }
    p1++;
  }
}


//---------------------------------------------------------
// Procedure: detectAndPostOdometry

void PathCheck_MOOSApp::detectAndPostOdometry()
{
  map<string, bool>::iterator p = m_map_changed_dist.begin();
  while(p != m_map_changed_dist.end()) {
    const string& vname = p->first;
    bool changed = p->second;
    if(changed) {
      m_map_changed_dist[vname] = false;
      string msg = "vname=" + vname;
      msg += ",total_dist=" + doubleToString(m_map_total_dist[vname],1);
      msg += ",trip_dist=" + doubleToString(m_map_trip_dist[vname],1);
      m_Comms.Notify("UPC_ODOMETRY_REPORT", msg);
    }
    p++;
  }
}

#if 0
  ODOMETRY_REPORT = "vname=alpha, total_dist=9832.1, trip_dist=1991.1"
  SPEED_REPORT    = "vname=alpha, avg_speed=1.0"
#endif

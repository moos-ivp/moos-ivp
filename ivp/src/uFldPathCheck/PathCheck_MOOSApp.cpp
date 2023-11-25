/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PathCheck_MOOSApp.cpp                                */
/*    DATE: Nov 22th 2011                                        */
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
#include <iterator>
#include "PathCheck_MOOSApp.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "NodeRecordUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PathCheck_MOOSApp::PathCheck_MOOSApp()
{
  // Initialize configuration params
  m_history_size = 10;

  // Initialize state variables
  m_node_reports_recd_good = 0;
  m_node_reports_recd_bad  = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PathCheck_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL")) {
      string sval  = msg.GetString(); 
      NodeRecord record = string2NodeRecord(sval);
      bool ok = addNodeRecord(record);
      if(!ok)
	reportRunWarning("Invalid NODE_REPORT: " + sval);
    }
    else if(key == "UPC_TRIP_RESET") {
      string sval  = msg.GetString(); 
      bool ok = handleTripReset(sval);
      if(!ok)
	reportRunWarning("Invalid TRIP_RESET: " + sval);
    }
    else
      reportRunWarning("Unhandled Mail: " + key);

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
  AppCastingMOOSApp::Iterate();

  computeAndPostSpeeds();
  detectAndPostOdometry();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool PathCheck_MOOSApp::OnStartUp()
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
    if((param == "HISTORY_LENGTH") && isNumber(value)) {
      int length = atoi(value.c_str());
      if(length > 1) {
	m_history_size = (unsigned int)(length);
	handled = true;
      }
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: addNodeRecord

bool PathCheck_MOOSApp::addNodeRecord(const NodeRecord& record)
{
  string vname = record.getName();
  m_map_noderep_tstamp[vname] = m_curr_time;

  if(!record.valid()) {
    m_node_reports_recd_bad++;
    return(false);
  }
  else
    m_node_reports_recd_good++;
  
  
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
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("UPC_TRIP_RESET", 0);
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
      double prev_x = 0;
      double prev_y = 0;
      double prev_ts = 0;
      double total_speed = 0;
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
      Notify("UPC_SPEED_REPORT", report);
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
      Notify("UPC_ODOMETRY_REPORT", msg);
    }
    p++;
  }
}

//---------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.

bool PathCheck_MOOSApp::buildReport()
{
  m_msgs << " Node Reports Recd (good): " << m_node_reports_recd_good  << endl;
  m_msgs << " Node Reports Recd (bad):  " << m_node_reports_recd_bad   << endl;
  m_msgs << " MOOS Time Warp:           " << m_time_warp               << endl;
  m_msgs << endl;

  ACTable actab(6,3); // 6 columns, 3 space separators
  
  actab << "Node | TripDist | Distance | ReportAge | Speed | AvgSpeed";
  actab.addHeaderLines();
  
  map<string, double>::iterator p;
  for(p=m_map_total_dist.begin(); p!=m_map_total_dist.end(); p++) {
    string node_name   = p->first;
    double total_dist  = p->second;
    double trip_dist   = m_map_trip_dist[node_name];
    double noderep_age = m_curr_time - m_map_noderep_tstamp[node_name];

    string s_total_dist  = doubleToString(total_dist,1);
    string s_trip_dist   = doubleToString(trip_dist,1);    
    string s_noderep_age = doubleToString(noderep_age,1);
    string s_curr_spd    = "n/a";
    string s_avg_spd     = "n/a";

    m_msgs << node_name  << s_trip_dist << s_total_dist << s_noderep_age;
    m_msgs << s_curr_spd  << s_avg_spd;
  }
  m_msgs << actab.getFormattedString();
  
  return(true);
}


#if 0
  ODOMETRY_REPORT = "vname=alpha, total_dist=9832.1, trip_dist=1991.1"
  SPEED_REPORT    = "vname=alpha, avg_speed=1.0"
#endif










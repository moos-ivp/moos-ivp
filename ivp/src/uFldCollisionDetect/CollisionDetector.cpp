/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CollisionDetector.cpp                                */
/*    DATE: 21 May 2013                                          */
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

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "CollisionDetector.h"
#include "NodeRecordUtils.h"
#include <math.h>
#include "XYRangePulse.h"
#include "CPAEngine.h"

using namespace std;


//---------------------------------------------------------
// Constructor

CollisionDetector::CollisionDetector()
{
  m_col_dist = 5;
  m_delay_time_to_clear = 5;
  m_post_immediately = false;
  m_post_string = "";

  m_pulse_bool = true;
  m_pulse_duration = 4;
  m_pulse_range = 20;

  m_publish_single = true;
  m_publish_pairs = false;

  m_start_checking_time = 200*MOOSTime(); // avoids pre-deploy collision reports 
  m_deploy_delay = 0;
  m_check_collisions = false;
  m_total_collisions = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CollisionDetector::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

    if(key == "NODE_REPORT"){
      //if a node report is received, add the vehicle to the map of known vehicles.
      string sval  = msg.GetString(); 
      NodeRecord record = string2NodeRecord(sval);
      string node_name = record.getName();
      m_moos_map[node_name] = record;
    }
    else if(key == "DEPLOY_ALL"){
      string sval  = msg.GetString(); 
      sval = tolower(sval);
      if(sval == "true"){
	m_start_checking_time = MOOSTime() + m_deploy_delay;
      }
    }
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CollisionDetector::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CollisionDetector::Iterate()
{
  AppCastingMOOSApp::Iterate();
  if((MOOSTime() > m_start_checking_time)){
    m_check_collisions = true;
  }

  // check positions relative to other vehicles
  for (map<string,NodeRecord>::iterator it1=m_moos_map.begin(); it1!=m_moos_map.end(); ++it1){
    
    //check only vehicles that have not yet been checked
    for (map<string,NodeRecord>::iterator it2=m_moos_map.find(it1->first);it2!=m_moos_map.end(); ++it2){
      //vehicle names
      string v1 = it1->first;
      string v2 = it2->first;
      string collision_string = "";

      if(v1 != v2){
	bool collisionKnown = false;
	double clear_time = MOOSTime() - 5; // ensures default of clearing screen case below if no prior collision existed.
	bool posted = false;

	if(! ( m_col_bools.find(make_pair(v1,v2)) == m_col_bools.end() )) {
	  // v1, v2 pair found => this pair has already had at least one collision.  
	  // Determine if they are in an existing collision condition. 
	  // If not in existing collision, then the false from initialization will hold and a new collision will be processed.
	  collisionKnown = (m_col_bools.find(make_pair(v1,v2))->second).colliding;
	  clear_time = (m_col_bools.find(make_pair(v1,v2))->second).display_clear_time;
	  posted =  (m_col_bools.find(make_pair(v1,v2))->second).posted;
	}
	string name_string = "VEHICLE_COLLISION_" + toupper(v1) + "_" + toupper(v2); // variable to be posted to MOOSDB

	//use v1 = os, v2 = cn
	NodeRecord os = m_moos_map[v1];
	NodeRecord cn = m_moos_map[v2];

	double cnx = cn.getX();
	double cny = cn.getY();
	double cnh = cn.getHeading();
	double cnv = cn.getSpeed();
	  
	double osx = os.getX();
	double osy = os.getY();
	double osh = os.getHeading();
	double osv = os.getSpeed();
	double ostol = os.getElapsedTime(MOOSTime());
	  
	CPAEngine cpaengine = CPAEngine(cny,cnx,cnh,cnv,osy,osx);
	double distance = cpaengine.evalCPA(osh,osv,ostol);
       

	// if within collision threshold distance AND collision not already known, take collision actions
	if((distance < m_col_dist) && (!collisionKnown) && (m_check_collisions)){ 
	  // new collision
	  m_total_collisions++;
	  string output_string = "new collision detected: ";
	  string info_string = "v1=" + v1 + ",v2=" + v2 + ",detection_distance=" + doubleToString(distance,2); 
	  output_string += info_string;
	  cout << output_string << endl;
	  reportEvent(output_string);
	  if(m_pulse_bool){
	    MakeRangePulse((it1->second).getX(),(it1->second).getY());
	  }
	  // keep track of current collision in m_col_bools for appcasting
	  // a collision is known to exist between v1 and v2.
#if 0
	  col_data temp_data = (col_data){true,distance,distance,(MOOSTime()+m_delay_time_to_clear),false};
#endif
	  col_data temp_data;
	  temp_data.colliding = true;
	  temp_data.min_dist = distance;
	  temp_data.detect_dist = distance;
	  temp_data.display_clear_time = (MOOSTime()+m_delay_time_to_clear);
	  temp_data.posted = false;

	  m_col_bools[make_pair(v1,v2)] = temp_data;
	  collision_string +=  doubleToString(distance); 
	 
	  if(m_publish_pairs){
	    Notify(name_string,collision_string);
	  }
	  if(m_publish_single){
	    Notify("VEHICLE_COLLISION",info_string);
	  }
	}
	else if((distance < m_col_dist) && (collisionKnown) ){ 
	  // collision remains; update the minimum distance and time until appcast is cleared.
	  (m_col_bools[make_pair(v1,v2)]).min_dist = min(distance,(m_col_bools[make_pair(v1,v2)]).min_dist);
	  (m_col_bools[make_pair(v1,v2)]).display_clear_time = MOOSTime() + m_delay_time_to_clear;
	}
	else if((collisionKnown) && (clear_time >= MOOSTime())){
	  // no collision exists, but still desire the appcast display to occur
	  if(m_post_immediately && (!posted)){
	    if(m_publish_pairs){
	      Notify(name_string,"false");
	    }
	    (m_col_bools.find(make_pair(v1,v2))->second).posted = true;
	  }
	}
	else if((collisionKnown) && (clear_time < MOOSTime())){
	  // no collision exists between these two vehicles and no longer desire appcast display.
	  (m_col_bools[make_pair(v1,v2)]).colliding = false;
	  if((!m_post_immediately) && (!posted)){
	    if(m_publish_pairs){
	      Notify(name_string,"false");
	    }
	    (m_col_bools.find(make_pair(v1,v2))->second).posted = true;
	  }
	}
	else{
	  // no collision exists and message has already been posted; do not re-post.  Case reserved for future use.
	}
      }
    }
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CollisionDetector::OnStartUp()
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
    if(param == "COLLISION_RANGE") {
      // range at which a collision is said to occur.
      handled = true;
      m_col_dist = atof(value.c_str());
    }
    else if(param == "DELAY_TIME_TO_CLEAR") {
      // time delay from collision condition clearing and appcast
      // message clearing prevents quick collisions from not appearing
      // on appcast or clearing before desired data obtained.
      handled = true;
      m_delay_time_to_clear = atof(value.c_str());
    }
    else if(param == "PUBLISH_IMMEDIATELY") {
      // option to publish a collision / clear condition to MOOSDB
      // immediately or synchronously with appcast update.
      string str = tolower(value);
      if(str == "true"){
	m_post_immediately = true;
	handled = true;	
      }
      else if (str == "false"){
	m_post_immediately = false;
	handled = true;
      }
      else{
	handled = false;
      }
      m_post_string = str;
    }
    else if(param == "PULSE") {
      // option to publish a collision / clear condition to MOOSDB
      // immediately or synchronously with appcast update.
      string str = tolower(value);
      if(str == "true"){
	m_pulse_bool = true;
	handled = true;	
      }
      else if (str == "false"){
	m_pulse_bool = false;
	handled = true;
      }
      else{
	handled = false;
      }
    }
    else if(param == "PUBLISH_PAIRS") {
      string str = tolower(value);
      if(str == "true"){
	m_publish_pairs = true;
	handled = true;	
      }
      else if (str == "false"){
	m_publish_pairs = false;
	handled = true;
      }
      else{
	handled = false;
      }
    }
    else if(param == "PUBLISH_SINGLE") {
      string str = tolower(value);
      if(str == "true"){
	m_publish_single = true;
	handled = true;	
      }
      else if (str == "false"){
	m_publish_single = false;
	handled = true;
      }
      else{
	handled = false;
      }
    }
    else if(param == "PULSE_RANGE") {
       handled = true;
       m_pulse_range = atof(value.c_str());
    }
    else if(param == "PULSE_DURATION") {
      handled = true;
      m_pulse_duration = atof(value.c_str());
    }
    else if(param == "DEPLOY_DELAY_TIME") {
      handled = true;
      m_deploy_delay = atof(value.c_str());
    }
    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void CollisionDetector::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("DEPLOY_ALL",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CollisionDetector::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "uFldCollisionDetect                          \n";
  m_msgs << "============================================ \n";

  m_msgs << "\n   Threshold Collision Range:    " << doubleToString(m_col_dist,0) << endl;
  m_msgs << "   Delay Time to Clear Messages: " << doubleToString( m_delay_time_to_clear,0) << endl;
  m_msgs << "   Post to MOOSDB Immediately:   " << m_post_string << endl;
  m_msgs << "   Total Collisions:             " << doubleToString(m_total_collisions,0) << "\n\n" << endl;

  // determine list of vehicles with known collisions for appcasting report:

  // post these known collisions in appcasting format
  ACTable actab(4);
  actab.setColumnMaxWidth(0,30);

  actab << "Vehicle 1  | Vehicle 2  | Min Distance | Detection Distance";
  actab.addHeaderLines();

  for ( map<pair<string,string>,col_data> ::iterator it = m_col_bools.begin(); it!=m_col_bools.end(); ++it){
    if( (it->second).colliding ){ 
      // a collison exists between these two vehicles
      pair<string,string> vehicleNames =  it->first;
      string v1 = vehicleNames.first;
      string v2 = vehicleNames.second;
      actab.addCell(v1);
      actab.addCell(v2);
      actab.addCell(doubleToString( (it->second).min_dist));
      actab.addCell(doubleToString( (it->second).detect_dist));
    }
  }
  m_msgs << actab.getFormattedString();

  return(true);
}


bool CollisionDetector::MakeRangePulse(double x, double y){
  XYRangePulse pulse;
  pulse.set_x(x);
  pulse.set_y(y);
  pulse.set_label("collision_pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(MOOSTime());
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");
  string spec = pulse.get_spec();
  Notify("VIEW_RANGE_PULSE", spec);
  return true;
}




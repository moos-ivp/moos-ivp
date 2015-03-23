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

#include "CollisionDetector.h"

using namespace std;


//---------------------------------------------------------
// Constructor

CollisionDetector::CollisionDetector()
{
  m_preferred_min_cpa_distance = 5;
  m_near_miss_distance = 4;
  m_collision_distance = 3;

  // post configuration variables
  m_delay_time_to_clear = 5;
  m_post_immediately = false;
  m_post_string = "";
  m_check_string = "delayed -- off";

  // pulse configuration variables
  m_pulse_bool = true;
  m_pulse_duration = 4;
  m_pulse_range = 20;

  // interaction checking variables
  m_start_checking_time = 200*MOOSTime(); // avoids pre-deploy interaction reports 
  m_start_running_by_clock = false;       // only allow clock to turn me on once
  m_deploy_delay = 0;
  m_check_collisions = false;

  // state variables -- counters of collision types
  m_total_collisions = 0;
  m_total_near_misses = 0;
  m_total_cpa_violations = 0;
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
    else if(key == "COLLISION_DETECTOR_CHECK_COLLISIONS") {
      // option to publish an interaction / clear condition to MOOSDB
      // immediately or synchronously with appcast update.
      string sval  = msg.GetString(); 
      sval = tolower(sval);
      if(sval == "true"){
	m_check_collisions = true;
	m_check_string = "ON -- forced by msg poke";
      }
      else if (sval == "false"){
	m_check_collisions = false;
	m_check_string = "OFF -- forced by msg poke";
      }
      else{
	// Reserved.
      }
    }
    else if(strContains(key,"COLREGS_AVOID_MSG__")){
      // get vehicle names from COLREGS_AVOID_MSG__$V1(OS)__COLREGS_AVOID_MODE_$V2(CN)
      string v2 = rbiteString(key,'_'); // reverse bite string
      v2 = tolower(v2);
      vector<string> svect = parseString(key,"__");
      string v1 = tolower(svect[1]);
      string submode = msg.GetString();
      string mode = biteString(submode,':');

      m_colregs_mode_map[make_pair(v1,v2)] = make_pair(mode,submode);
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
  if((MOOSTime() > m_start_checking_time) && !(m_start_running_by_clock)){
    m_start_running_by_clock = true;
    m_check_collisions = true;
    m_check_string = "ON";
  }

  // check positions relative to other vehicles
  for (map<string,NodeRecord>::iterator it1=m_moos_map.begin(); it1!=m_moos_map.end(); ++it1){
    
    //check only vehicles that have not yet been checked
    for (map<string,NodeRecord>::iterator it2=m_moos_map.find(it1->first);it2!=m_moos_map.end(); ++it2){
      //vehicle names
      string v1 = it1->first;
      string v2 = it2->first;
      //      string collision_string = "";

      if(v1 != v2){
	bool collisionKnown = false;
	double clear_time = MOOSTime() - 5; // ensures default of clearing screen case below if no prior collision existed.
	bool posted = false;

	if(! ( m_col_bools.find(make_pair(v1,v2)) == m_col_bools.end() )) {
	  // v1, v2 pair found => this pair has already had at least one interaction.  
	  // Determine if they are in an existing interaction condition. 
	  // If not in existing interaction, then the false from initialization will hold and a new interaction will be processed.
	  collisionKnown = (m_col_bools.find(make_pair(v1,v2))->second).getInteracting();
	  clear_time = (m_col_bools.find(make_pair(v1,v2))->second).getDisplayClearTime();
	  posted =  (m_col_bools.find(make_pair(v1,v2))->second).getPosted();
	}
	 // variables to be posted to MOOSDB
	const string name_string = "VEHICLE_INTERACTION_REPORT_" + toupper(v1) + "_" + toupper(v2);
	const string name_string_immediate = "VEHICLE_INTERACTION_IMMEDIATE_" + toupper(v1) + "_" + toupper(v2);

	// os is first alphabetically; cn is other vessel
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
	
	// if within interaction threshold distance AND interaction not already known
	if((distance < m_preferred_min_cpa_distance) && (!collisionKnown) && (m_check_collisions)){ 
	  // this is a new interaction
	  // keep track of current interaction in m_col_bools for appcasting
	  // a interaction is known to exist between v1 and v2.

	  CollisionRecord cr;
	  cr.set2Vehicles(v1, v2);
	  cr.setParameters(m_collision_distance, m_near_miss_distance, m_preferred_min_cpa_distance);
	  cr.setInteracting(true);// = true;
	  cr.setMinDistance(distance);
	  cr.setDetectionDistance(distance);
	  cr.setDisplayClearTime((MOOSTime()+m_delay_time_to_clear));
	  cr.setPosted(false);
	  cr.updateCollisionType();
	  cr.setInteractionTime(MOOSTime());
	  storeVehicleModes(cr,v1,v2);
	  
	  if((m_pulse_bool)){
	    MakeCPAViolationRangePulse((it1->second).getX(),(it1->second).getY());
	    MakeCPAViolationRangePulse((it2->second).getX(),(it2->second).getY());
	    cr.cpaRangeRingFired();
	  }

	  
	  m_col_bools[make_pair(v1,v2)] = cr;
	  string info_string = cr.getString();
	  reportEvent(info_string);
	  
	  if(m_post_immediately){
	      Notify(name_string_immediate,info_string);
	  }
	}
	else if((distance < m_preferred_min_cpa_distance) && (collisionKnown) ){ 
	  // interaction remains; update the minimum distance and time until appcast is cleared.
	  // interaction is assumed if any distance is less than preferred cpa distance

	  CollisionRecord& rec =  (m_col_bools[make_pair(v1,v2)]);
	  rec.setMinDistance(distance);
	  rec.setDisplayClearTime(MOOSTime() + m_delay_time_to_clear);
	  storeVehicleModes(rec,v1,v2);

	  string info_string = (rec).getString();
	  
	  if((distance < m_collision_distance)){
	    if(m_post_immediately && ! (rec).getCollisionPosted()){
	      Notify(name_string_immediate,info_string);
	      (rec).collisionPosted();
	    }
	    if( (m_pulse_bool) &&  (!(rec).getCollisionRangeRingFired())){
	      MakeCollisionRangePulse((it1->second).getX(),(it1->second).getY());
	      MakeCollisionRangePulse((it2->second).getX(),(it2->second).getY());
	      (rec).collisionRangeRingFired();
	    }
	  }
	  else if(distance < m_near_miss_distance){
	    if(m_post_immediately && !(rec).getNearMissPosted()){
	      Notify(name_string_immediate,info_string);
	      (rec).nearMissPosted();
	    }
	    if( (m_pulse_bool) && (!(rec).getNearMissRangeRingFired())){
	      MakeNearMissRangePulse((it1->second).getX(),(it1->second).getY());
	      MakeNearMissRangePulse((it2->second).getX(),(it2->second).getY());
	      (rec).nearMissRangeRingFired();
	    }
	  }
	}
	else if((collisionKnown) && (clear_time >= MOOSTime())){
	  // interaction no longer exists, but still desire the appcast display to occur
	  
	  if((!posted)){
	    postAndUpdate(v1, v2, name_string);
	  }
	  
	}
	else if((collisionKnown) && (clear_time < MOOSTime())){
	  // no interaction exists between these two vehicles and no longer desire appcast display.
	  if( (!posted)){
	    postAndUpdate(v1, v2, name_string);
	  }
	  (m_col_bools[make_pair(v1,v2)]).clearInteraction();
	}
	else{
	  // no interaction exists and message has already been posted; do not re-post.  Case reserved for future use.
	}
      }
    }
  } 
  AppCastingMOOSApp::PostReport();
  return(true);
}

void CollisionDetector::postAndUpdate(string v1, string v2, string name_string){
  string info_string = (m_col_bools.find(make_pair(v1,v2))->second).getString();
  Notify(name_string,info_string);
  (m_col_bools.find(make_pair(v1,v2))->second).setPosted(true);
  string type = (m_col_bools[make_pair(v1,v2)]).getCollisionType();
  if(type == "collision"){
    m_total_collisions++;
  }
  else if(type == "near_miss"){
    m_total_near_misses++;
  }
  else if(type == "min_cpa_violation"){
    m_total_cpa_violations++;
  }
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
      m_collision_distance = atof(value.c_str());
    }
    else if(param == "NEAR_MISS_RANGE") {
      // range at which a near-miss is said to occur.
      handled = true;
      m_near_miss_distance = atof(value.c_str());
    }
    else if(param == "CPA_VIOLATION_RANGE") {
      // range at which a CPA violation is said to occur.
      handled = true;
      m_preferred_min_cpa_distance = atof(value.c_str());
    }
    
    else if(param == "DELAY_TIME_TO_CLEAR") {
      // time delay from collision condition clearing and appcast
      // message clearing prevents quick interactions from not appearing
      // on appcast or clearing before desired data obtained.
      handled = true;
      m_delay_time_to_clear = atof(value.c_str());
    }
    else if(param == "PUBLISH_IMMEDIATELY") {
      // option to publish a interaction / clear condition to MOOSDB
      // immediately or synchronously with appcast update.
      string str = tolower(value);
      if(str == "true"){
	m_post_immediately = true;
	handled = true;	
	m_post_string = "yes";
      }
      else if (str == "false"){
	m_post_immediately = false;
	handled = true;
	m_post_string = "no";
      }
      else{
	handled = false;
      }
    }
    else if(param == "PULSE") {
      // option to publish a interaction / clear condition to MOOSDB
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
  Register("COLLISION_DETECTOR_CHECK_COLLISIONS",0); // allows turning on/off CollisionDetector mid-mission by poke
  Register("COLLISION_DETECTOR_RESPONSE",0);
  Register("COLREGS*","*",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CollisionDetector::buildReport() 
{
  double time = ((MOOSTime() - m_start_checking_time));
  if(time < 0){
    string count_down = doubleToString(-1* time,0);
    
    if( -(time) > m_deploy_delay){
      count_down = "not started";
    }
    else{

    }
    m_msgs << "   Time until active:   " << count_down  << "\n\n\n" << endl;
  }
  
  m_msgs << "============================================ \n";
  m_msgs << "uFldCollisionDetect                          \n";
  m_msgs << "============================================ \n";

  m_msgs << "    Delay Time to Clear Messages:   " << doubleToString( m_delay_time_to_clear,0) << endl;
  m_msgs << "Currently Checking for Collision:   " << m_check_string << endl;
  m_msgs << "      Post to MOOSDB Immediately:   " << m_post_string << "\n" <<endl;
  m_msgs << "       Threshold Collision Range:   " << doubleToString(m_collision_distance,0) << endl;
  m_msgs << "       Threshold Near-Miss Range:   " << doubleToString(m_near_miss_distance,0) << endl;
  m_msgs << "   Threshold CPA Violation Range:   " << doubleToString(m_preferred_min_cpa_distance,0) << "\n" << endl;
  m_msgs << "                Total Collisions:   " << doubleToString(m_total_collisions,0) << "" << endl;
  m_msgs << "               Total Near-Misses:   " << doubleToString(m_total_near_misses,0) << "" << endl;
  m_msgs << "            Total CPA Violations:   " << doubleToString(m_total_cpa_violations,0) << "\n" << endl;
  
  // determine list of vehicles with known interactions for appcasting report:

  // post these known interactions in appcasting format
  ACTable actab(5);
  actab.setColumnMaxWidth(0,30);

  actab << "Vehicle 1  | Vehicle 2  | Detection Distance | Minimum Distance | Type";
  actab.addHeaderLines();

  for ( map<pair<string,string>,CollisionRecord> ::iterator it = m_col_bools.begin(); it!=m_col_bools.end(); ++it){
    if( (it->second).getCollisionPosted() ||
	(it->second).getNearMissPosted() ||
	(it->second).getInteracting()	){ 
      // a collison exists between these two vehicles
      pair<string,string> vehicleNames =  it->first;
      string v1 = vehicleNames.first;
      string v2 = vehicleNames.second;
      actab.addCell(v1);
      actab.addCell(v2);
      actab.addCell(doubleToString( (it->second).getDetectDistance(),2));
      actab.addCell(doubleToString( (it->second).getMinDistance(),2));
      actab.addCell((it->second).getCollisionType());
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
 bool CollisionDetector::MakeCollisionRangePulse(double x, double y){
  XYRangePulse pulse;
  pulse.set_x(x);
  pulse.set_y(y);
  pulse.set_label("collision_pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(MOOSTime());
  pulse.set_color("edge", "red");
  pulse.set_color("fill", "red");
  string spec = pulse.get_spec();
  Notify("VIEW_RANGE_PULSE", spec);
  return true;
}
 bool CollisionDetector::MakeNearMissRangePulse(double x, double y){
  XYRangePulse pulse;
  pulse.set_x(x);
  pulse.set_y(y);
  pulse.set_label("near_miss_pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(MOOSTime());
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");
  string spec = pulse.get_spec();
  Notify("VIEW_RANGE_PULSE", spec);
  return true;
}
 bool CollisionDetector::MakeCPAViolationRangePulse(double x, double y){
  XYRangePulse pulse;
  pulse.set_x(x);
  pulse.set_y(y);
  pulse.set_label("cpa_violation_pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(MOOSTime());
  pulse.set_color("edge", "green");
  pulse.set_color("fill", "green");
  string spec = pulse.get_spec();
  Notify("VIEW_RANGE_PULSE", spec);
  return true;
}

bool CollisionDetector::storeVehicleModes(CollisionRecord& cr_in, string v1, string v2){
  pair<string,string> v1_mode = m_colregs_mode_map[make_pair(v1,v2)];
  pair<string,string> v2_mode = m_colregs_mode_map[make_pair(v2,v1)];
  bool ok1, ok2;
  ok1 = cr_in.setV1Mode(v1_mode);
  ok2 = cr_in.setV2Mode(v2_mode);
  if(ok1 && ok2){
    return true;
  }
  return false;
}


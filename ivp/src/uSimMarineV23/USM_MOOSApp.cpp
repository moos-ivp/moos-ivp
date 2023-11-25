/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: USM_MOOSApp.cpp                                      */
/*    DATE: Oct 25th 2004                                        */
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

#include <iostream>
#include <math.h>
#include "USM_MOOSApp.h"
#include "MBUtils.h"
#include "ACTable.h"
#include "AngleUtils.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

using namespace std;

//------------------------------------------------------------------------
// Constructor

USM_MOOSApp::USM_MOOSApp() 
{
  // Init Simulator variables
  m_sim_prefix  = "USM";
  m_buoyancy_requested = false;
  m_trim_requested = false;
  m_buoyancy_delay = 5;
  m_max_trim_delay = 10;
  m_report_interval = 5; 
  m_last_report     = 0;
  m_report_interval = 5;
  m_pitch_tolerance = 5;
  m_enabled = true;
  m_depth_info_acast = true;
  
  // Init PID variables
  m_pid_allstop_posted = false;
  m_pid_ignore_nav_yaw = false;
  m_pid_verbose    = true;
  m_pid_ok_skew    = 1360;  

  // Indicate Sim/PID coupling variables
  m_pid_coupled = false;
  m_nav_modulo  = 2;
}

//------------------------------------------------------------------------
// Procedure: OnNewMail

bool USM_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString();
    string src  = msg.GetSource();

    //====================================================
    // Part I: Handle PID related Mail (optional)
    //====================================================
    if(m_pid_coupled) {
      double dfT;
      msg.IsSkewed(m_curr_time, &dfT);
      if(fabs(dfT) < m_pid_ok_skew) {
        if((key == "MOOS_MANUAL_OVERIDE") || (key == "MOOS_MANUAL_OVERRIDE"))
          m_pengine.setPIDOverride(sval);
	else if(key == "PID_VERBOSE")
	  setBooleanOnString(m_pid_verbose, sval);
	else if(key == "SPEED_FACTOR")
	  m_pengine.setSpeedFactor(dval);    
	else if(key == "DESIRED_HEADING")
	  m_pengine.setDesHeading(dval);
	else if(key == "DESIRED_SPEED")
	  m_pengine.setDesSpeed(dval);
	else if(key == "DESIRED_DEPTH") 
	  m_pengine.setDesDepth(dval);
	
	// FYI normal PID mail would include NAV_* variables
	// But when PID is embedded in this sim, NAV_* info
	// is contained in local state, e.g., curr_nav_x.
	else if(key == "NAV_HEADING") 
	  m_pengine.setCurrHeading(angle360(dval));
	else if(key == "NAV_SPEED")
	  m_pengine.setCurrSpeed(dval);
	else if(key == "NAV_DEPTH")
	  m_pengine.setCurrDepth(dval);
	else if(key == "NAV_PITCH")
	  m_pengine.setCurrPitch(dval);      
      }
      else
	Notify("USM_NOGO", dfT);
    }

    //====================================================
    // Part I: Handle Simulator related mail
    //====================================================
    if(key == "DESIRED_THRUST") 
      m_model.setThrust(dval);
    else if(key == "DESIRED_FAN") 
      m_model.setThrustFan(dval);
    else if(key == "DESIRED_RUDDER")
      m_model.setRudder(dval, MOOSTime());
    else if(key == "DESIRED_ELEVATOR")
      m_model.setElevator(dval);
    else if(key == "DESIRED_THRUST_L")
      m_model.setThrustLeft(dval);
    else if(key == "DESIRED_THRUST_R")
      m_model.setThrustRight(dval);
    else if(key == "USM_SIM_PAUSED")
      m_model.setPaused(sval);
    else if(key == "THRUST_MODE_REVERSE")
      m_model.setThrustModeReverse(sval);
    else if(key == "THRUST_MODE_DIFFERENTIAL")
      m_model.setThrustModeDiff(sval);
    else if(key == "BUOYANCY_RATE")
      m_model.setParam("buoyancy_rate", dval);
    else if(key == "WIND_CONDITIONS")
      m_model.setParam("wind_conditions", sval);
    else if(key == "ROTATE_SPEED")
      m_model.setParam("rotate_speed", dval);
    else if((key == "CURRENT_X") || (key == "DRIFT_X"))
      m_model.setDriftX(dval, src);
    else if((key == "CURRENT_Y") || (key == "DRIFT_Y"))
      m_model.setDriftY(dval, src);
    else if(key == "DRIFT_VECTOR")  
      m_model.setDriftVector(sval, src, false);
    else if(key == "DRIFT_VECTOR_ADD")
      m_model.setDriftVector(sval, src, true);
    else if(key == "DRIFT_VECTOR_MULT") 
      m_model.magDriftVector(dval, src);
    else if(key == "WATER_DEPTH")
      m_model.setParam("water_depth", dval);    
    else if(key == "OBSTACLE_HIT") 
      m_model.setObstacleHit(tolower(sval) == "true");
    else if(key == "USM_RESET") {
      m_model.initPosition(sval);
      Notify("USM_RESET_COUNT", m_model.getResetCount());
    }
    else if(key == "USM_TURN_RATE") 
      m_model.setParam("turn_rate", dval);
    else if(key == "USM_ENABLED")
      setBooleanOnString(m_enabled, sval);

    // When sim is disabled, presumably another sim is temporarily running
    // and updating the vehicle position. In the meanwhile we just inform
    // the model of the update nav info so it has accurate ownship info
    // when/if this sim becomes enabled again.
    else if((key == "NAV_X") && !m_enabled) 
      m_model.informX(dval);
    else if((key == "NAV_Y") && !m_enabled)
      m_model.informY(dval);
    else if((key == "NAV_HEADING") && !m_enabled) 
      m_model.informHeading(dval);
    
    // Added buoyancy and trim control and sonar handshake. HS 2012-07-22
    else if(key == "BUOYANCY_CONTROL")
      handleMailBuoyancyControl(sval);
    else if(key == "TRIM_CONTROL")
      handleMailTrimControl(sval);
  }
  
  return(true);
}
  
//----------------------------------------------------------------
// Procedure: Iterate

bool USM_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();
  if(!m_enabled) {
    m_model.resetTime(m_curr_time);
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  
  //====================================================
  // Part I: PID Controller Function (optional)
  //====================================================
  if(m_pid_coupled) {
    // Part A: update the PID Engine with latest hdg,spd,dep
    // info. Normally in a stand-alone PID app this is coming
    // via mail in the form of NAV_HEADING etc messages. With
    // embedded PID control this come directly from sim model
    NodeRecord record = m_model.getNodeRecord();
    m_pengine.setCurrHeading(record.getHeading());
    m_pengine.setCurrSpeed(record.getSpeed());
    if(m_pengine.hasDepthControl()) {
      m_pengine.setCurrDepth(record.getDepth());
      m_pengine.setCurrPitch(record.getPitch());      
    }

    // Part B: Update the desired_* values
    m_pengine.updateTime(m_curr_time);
    m_pengine.setDesiredValues();

    // Part C: Post results
    postPengineResults();  
    postPenginePostings(); 
  }

  //====================================================
  // Part II: Vehicle Simulator Function
  //====================================================
  m_model.propagate(m_curr_time);
  NodeRecord record = m_model.getNodeRecord();

  handleBuoyancyAndTrim(record);

  // Note the postings modulated by m_nav_modulo
  postNodeRecordUpdate(m_sim_prefix, record);
  if(m_model.usingDualState()) {
    NodeRecord record_gt = m_model.getNodeRecordGT();
    postNodeRecordUpdate(m_sim_prefix+"_GT", record_gt);
  }

  if(m_model.isDriftFresh()) {
    Notify("USM_DRIFT_SUMMARY", m_model.getDriftSummary());
    m_model.setDriftFresh(false);
  }

  postWindModelVisuals();
  applyWormHoles();
  postWormHolePolys();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}


//------------------------------------------------------------------------
// Procedure: OnStartUp

bool USM_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  //====================================================
  // Part I: PID Controller Config Params (optional)
  //====================================================
  sParams = m_pengine.setConfigParams(sParams);
  if(m_pengine.hasConfigSettingsForPID()) {
    bool ok_yaw = m_pengine.handleYawSettings();
    bool ok_spd = m_pengine.handleSpeedSettings();
    bool ok_dep = m_pengine.handleDepthSettings();
    if(!ok_yaw)
      reportConfigWarning("Improper YAW PID Setting");
    if(!ok_spd)
      reportConfigWarning("Improper SPD PID Setting");
    if( !ok_dep)
      reportConfigWarning("Improper DEP PID Setting");
    if(!ok_yaw || !ok_spd || !ok_dep) 
      return(true);

    m_pid_coupled = true;
  }
  
  
  //====================================================
  // Part II: Vehicle Simulator Config Params
  //====================================================
  m_model.resetTime(m_curr_time);
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string orig  = *p;

    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    double dval  = atof(value.c_str());

    bool handled = false;
    if((param == "start_x") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "start_y") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "start_heading") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "start_speed") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "start_depth") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "buoyancy_rate") && isNumber(value))
      handled = m_model.setParam(param, dval);
    else if((param == "drift_x") && isNumber(value))
      handled = m_model.setDriftX(dval, "");
    else if((param == "drift_y") && isNumber(value))
      handled = m_model.setDriftY(dval, "");

    else if(param == "wind_conditions")
      handled = m_model.setParam("wind_conditions", value);
    else if(param == "polar_plot")
      handled = m_model.setParam("polar_plot", value);
    
    else if(param == "turn_spd_map_full_speed")
      handled = m_model.setTSMapFullSpeed(value);
    else if(param == "turn_spd_map_null_speed")
      handled = m_model.setTSMapNullSpeed(value);
    else if(param == "turn_spd_map_full_rate")
      handled = m_model.setTSMapFullRate(value);
    else if(param == "turn_spd_map_null_rate")
      handled = m_model.setTSMapNullRate(value);

    else if((param == "rotate_speed") && isNumber(value))
      handled = m_model.setParam("rotate_speed", dval);
    else if((param == "max_acceleration") && isNumber(value))
      handled = m_model.setParam("max_acceleration", dval);
    else if((param == "max_deceleration") && isNumber(value))
      handled = m_model.setParam("max_deceleration", dval);
    else if((param == "max_depth_rate") && isNumber(value))
      handled = m_model.setParam("max_depth_rate", dval);
    else if((param == "max_depth_rate_speed") && isNumber(value))
      handled = m_model.setParam("max_depth_rate_speed", dval);
    else if((param == "max_rudder_degs_per_sec") && isNumber(value))
      handled = m_model.setMaxRudderDegreesPerSec(dval);
    else if(param == "prefix")
      handled = setNonWhiteVarOnString(m_sim_prefix, value);
    else if(param == "drift_vector")
      handled = m_model.setDriftVector(value, "");
    else if(param == "sim_pause")
      handled = m_model.setPaused(value);
    else if(param == "dual_state")
      handled = m_model.setDualState(value);
    else if(param == "start_pos")
      handled = m_model.initPosition(value);
    else if(param == "thrust_reflect")
      m_model.setThrustReflect(value);
    else if(param == "thrust_mode_diff") 
      handled = m_model.setThrustModeDiff(value);
    else if(param == "thrust_mode_reverse")
      m_model.setThrustModeReverse(value);	
    else if((param == "thrust_factor") && isNumber(value))
      m_model.setThrustFactor(dval);
    else if(param == "thrust_map")
      handled = m_model.handleFullThrustMapping(value);
    else if(param == "thrust_map_fan")
      handled = m_model.handleFullThrustMapFan(value);
    else if((param == "turn_rate") && isNumber(value))
      handled = m_model.setParam("turn_rate", dval);
    else if((param == "default_water_depth") && isNumber(value))
      handled = m_model.setParam("water_depth", dval);
    else if(param == "trim_tolerance") 
      handled = setDoubleOnString(m_pitch_tolerance, value);
    else if(param == "max_trim_delay") 
      handled = setDoubleOnString(m_max_trim_delay, value);
    else if(param == "wormhole") 
      handled = m_wormset.addWormHoleConfig(value);
    else if((param == "post_des_thrust") && (value != "DESIRED_THRUST"))
      handled = setNonWhiteVarOnString(m_post_des_thrust, value);
    else if((param == "post_des_rudder") && (value != "DESIRED_RUDDER"))
      handled = setNonWhiteVarOnString(m_post_des_rudder, value);
    else if(param == "depth_info_acast")
      handled = setBooleanOnString(m_depth_info_acast, value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if(!m_MissionReader.GetValue("LatOrigin", latOrigin))
    MOOSTrace("uSimMarine: Lat or Lon Origin not set in *.moos file.\n");
  else if(!m_MissionReader.GetValue("LongOrigin", longOrigin))
    MOOSTrace("uSimMarine: LongOrigin not set in *.moos file\n");
  else {
    CMOOSGeodesy geodesy;
    if(!geodesy.Initialise(latOrigin, longOrigin))
      MOOSTrace("uSimMarine: Geodesy init failed.\n");
    else
      m_model.setGeodesy(geodesy);
  }
  
  // Note Geodesy best set (as above) before building cache
  m_model.cacheStartingInfo();
 
  registerVariables();
  MOOSTrace("uSimMarine started \n");
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

bool USM_MOOSApp::OnConnectToServer()
{
  registerVariables();
  MOOSTrace("Sim connected\n");
  
  return(true);
}


//------------------------------------------------------------------------
// Procedure: registerVariables

void USM_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  //====================================================
  // Part I: PID Controller Registrations (optional)
  //====================================================
  if(m_pid_coupled) {
    Register("DESIRED_HEADING", 0);
    Register("DESIRED_SPEED", 0);
    Register("DESIRED_DEPTH", 0);
    Register("SPEED_FACTOR", 0);
    Register("MOOS_MANUAL_OVERIDE", 0);
    Register("MOOS_MANUAL_OVERRIDE", 0);

    // FYI normal PID reg would include NAV_* variables
    // But when PID is embedded in this sim, NAV_* info
    // is contained in local state, e.g., curr_nav_x.
#if 0
    Register("NAV_HEADING", 0);   
    Register("NAV_SPEED", 0);
    Register("NAV_DEPTH", 0);
    Register("NAV_PITCH", 0);
    Register("NAV_YAW", 0);
#endif
  }
  
  //====================================================
  // Part II: Vehicle Simulator Registrations
  //====================================================

  // When coupled PID, the DESIRED_*actuator* values are
  // produced internally by the PIDEngine, and thus not
  // read via MOOS from an external PID source
  if(!m_pid_coupled) {
    Register("DESIRED_RUDDER", 0);
    Register("DESIRED_THRUST", 0);
    Register("DESIRED_ELEVATOR", 0);
    Register("DESIRED_THRUST_L", 0);
    Register("DESIRED_THRUST_R", 0);
    Register("DESIRED_FAN", 0);
  }

  Register("NAV_X",0);
  Register("NAV_Y",0);
  Register("NAV_HEADING",0);

  
  Register("USM_ENABLED",0);
  Register("USM_TURN_RATE",0);

  Register("WIND_CONDITIONS");
  Register("BUOYANCY_RATE", 0);
  Register("WATER_DEPTH", 0);
  Register("CURRENT_X",0);
  Register("DRIFT_X",0);
  Register("CURRENT_Y",0);
  Register("DRIFT_Y",0);
  Register("DRIFT_VECTOR", 0);
  Register("DRIFT_VECTOR_ADD", 0);
  Register("DRIFT_VECTOR_MULT", 0);
  Register("ROTATE_SPEED", 0);
  Register("USM_SIM_PAUSED", 0); 
  Register("USM_RESET", 0);
  Register("OBSTACLE_HIT", 0);
  // Added buoyancy and trim control and sonar handshake
  Register("TRIM_CONTROL",0);
  Register("BUOYANCY_CONTROL",0);
  Register("THRUST_MODE_REVERSE",0);
  Register("THRUST_MODE_DIFFERENTIAL",0);
}

//------------------------------------------------------------------------
// Procedure: handleMailBuoyancyControl()

void USM_MOOSApp::handleMailBuoyancyControl(string sval)
{
  if(tolower(sval) == "true") {
    // Set buoyancy to zero to simulate trim
    m_model.setParam("buoyancy_rate", 0.0);
    m_buoyancy_request_time = MOOSTime();
    std::string buoyancy_status="status=1,error=0,progressing,buoyancy=0.0";
    Notify("BUOYANCY_REPORT", buoyancy_status);
    m_buoyancy_requested = true;
    m_last_report = m_buoyancy_request_time;
  } 
}	    

//------------------------------------------------------------------------
// Procedure: handleMailTrimControl()

void USM_MOOSApp::handleMailTrimControl(string sval)
{
  if(tolower(sval) == "true") {
    m_trim_request_time = MOOSTime();
    std::string trim_status="status=1,error=0,progressing,trim_pitch=0.0,trim_roll=0.0";
    Notify("TRIM_REPORT",trim_status);
    m_trim_requested = true;
    m_last_report = m_trim_request_time;
  }
}

//------------------------------------------------------------------------
// Procedure: handleBuoyancyAndTrim

void USM_MOOSApp::handleBuoyancyAndTrim(NodeRecord record)
{
  if(!m_pengine.hasDepthControl())
    return;
  
  // buoyancy and trim control
  if(m_buoyancy_requested) {
    if((m_curr_time - m_buoyancy_request_time) >= m_buoyancy_delay) {
      string buoyancy_status="status=2,error=0,completed,buoyancy=0.0";
      Notify("BUOYANCY_REPORT", buoyancy_status);
      m_buoyancy_requested = false;
    }
    else if((m_curr_time - m_last_report) >= m_report_interval) {
      string buoyancy_status="status=1,error=0,progressing,buoyancy=0.0";
      Notify("BUOYANCY_REPORT", buoyancy_status);
      m_last_report = m_curr_time; 
    }
  }
  if(m_trim_requested) {
    double pitch_degrees = record.getPitch()*180.0/M_PI;
  
    if(((fabs(pitch_degrees) <= m_pitch_tolerance)
	&& (m_curr_time-m_trim_request_time >= m_buoyancy_delay)) 
       || (m_curr_time-m_trim_request_time) >= m_max_trim_delay) {
      string trim_status="status=2,error=0,completed,trim_pitch="
	+ doubleToString(pitch_degrees) + ",trim_roll=0.0";
      Notify("TRIM_REPORT", trim_status);
      m_trim_requested = false;
    }
    else if((m_curr_time - m_last_report) >= m_report_interval) {
      string trim_status="status=1,error=0,progressing,trim_pitch="
	+ doubleToString(pitch_degrees) + ",trim_roll=0.0";
      Notify("TRIM_REPORT", trim_status);
      m_last_report = m_curr_time; 
    }
  }
}  

//------------------------------------------------------------------------
// Procedure: postNodeRecordUpdate

void USM_MOOSApp::postNodeRecordUpdate(string prefix, 
				       const NodeRecord &record)
{
  if((m_iteration % m_nav_modulo) != 0)
    return; 
  
  double nav_x = record.getX();
  double nav_y = record.getY();

  Notify(prefix+"_X", nav_x, m_curr_time);
  Notify(prefix+"_Y", nav_y, m_curr_time);

  if(m_model.geoOK()) {
    double nav_lat = record.getLat();
    double nav_lon = record.getLon();
    Notify(prefix+"_LAT", nav_lat, m_curr_time);
    Notify(prefix+"_LONG", nav_lon, m_curr_time);
  }

  double new_speed = record.getSpeed();
  new_speed = snapToStep(new_speed, 0.01);

  Notify(prefix+"_HEADING", record.getHeading(), m_curr_time);
  Notify(prefix+"_SPEED", new_speed, m_curr_time);
  Notify(prefix+"_DEPTH", record.getDepth(), m_curr_time);

  // Added by HS 120124 to make it work ok with iHuxley
  if(m_pengine.hasDepthControl()) {
    Notify("SIMULATION_MODE","TRUE", m_curr_time);
    Notify(prefix+"_Z", -record.getDepth(), m_curr_time);
    Notify(prefix+"_PITCH", record.getPitch(), m_curr_time);
    Notify(prefix+"_YAW", record.getYaw(), m_curr_time);
    Notify("TRUE_X", nav_x, m_curr_time);
    Notify("TRUE_Y", nav_y, m_curr_time);
  }

  double hog = angle360(record.getHeadingOG());
  double sog = record.getSpeedOG();

  Notify(prefix+"_HEADING_OVER_GROUND", hog, m_curr_time);
  Notify(prefix+"_SPEED_OVER_GROUND", sog, m_curr_time);
  
  if(record.isSetAltitude()) 
    Notify(prefix+"_ALTITUDE", record.getAltitude(), m_curr_time);
  
}

//------------------------------------------------------------------------
// Procedure: applyWormHoles()

void USM_MOOSApp::applyWormHoles()
{
  // Sanity Check
  if(m_wormset.size() == 0)
    return;

  // In both other cases we need current ownship position
  NodeRecord record = m_model.getNodeRecord();
  double osx = record.getX();
  double osy = record.getY();

  double newx = osx;
  double newy = osy;
  bool transported = m_wormset.apply(m_curr_time, osx,osy, newx,newy);
  
  if(transported) {
    m_model.informX(newx);
    m_model.informY(newy);
  }
}


//------------------------------------------------------------------------
// Procedure: postWormHolePolys()

void USM_MOOSApp::postWormHolePolys()
{
  if(m_wormset.size() == 0)
    return;

  Notify(m_sim_prefix+"_TRANSP", m_wormset.getTransparency());
  
  if((m_iteration % 10) != 0)
    return; 

  for(unsigned int i=0; i<m_wormset.size(); i++) {
    WormHole worm_hole = m_wormset.getWormHole(i);
    vector<XYPolygon> polys = worm_hole.getPolys();
    for(unsigned int j=0; j<polys.size(); j++) {
      XYPolygon poly = polys[j];
      string spec = poly.get_spec();
      Notify("VIEW_POLYGON", spec);
    }
  }
}

//------------------------------------------------------------------------
// Procedure: postWindModelVisuals()

void USM_MOOSApp::postWindModelVisuals()
{
  if(!m_model.sailingEnabled())
    return;

  if((m_iteration % 50) != 0)
    return; 
  
  string spec = m_model.getWindArrowSpec();
  if(spec == "")
    return;

  Notify("VIEW_ARROW", spec);  
}

//------------------------------------------------------------
// Procedure: postPenginePostings()
//   Purpose: Publish any postings the PIDEngine created

void USM_MOOSApp::postPenginePostings()
{
  vector<VarDataPair> m_postings = m_pengine.getPostings();
  for(unsigned int i=0; i<m_postings.size(); i++) {
    VarDataPair pair = m_postings[i];
    string var = pair.get_var();
    if(pair.is_string() && pair.get_sdata_set())
      Notify(var, pair.get_sdata());
    else if(!pair.is_string() && pair.get_ddata_set())
      Notify(var, pair.get_ddata());
  }
  m_pengine.clearPostings();
}


//------------------------------------------------------------
// Procedure: postPengineResults()

void USM_MOOSApp::postPengineResults()
{
  // With embedded PID, DESIRED_* values are passed directly
  // to the Sim model

  double desired_rudder   = m_pengine.getDesiredRudder();
  double desired_thrust   = m_pengine.getDesiredThrust();
  double desired_elevator = m_pengine.getDesiredElevator();
  
  m_model.setRudder(desired_rudder, m_curr_time);
  m_model.setThrust(desired_thrust);
  m_model.setElevator(desired_elevator);

  cout << "post_des_thrust:" << m_post_des_thrust << endl;
  
  if(m_post_des_thrust != "")
    Notify(m_post_des_thrust, desired_thrust);
  if(m_post_des_rudder != "")
    Notify(m_post_des_rudder, desired_rudder);
  
#if 0
  // This block is what would happen in stand-alone PID
  bool all_stop = true;
  if(m_pengine.hasControl())
    all_stop = false;

  if(all_stop) {
    if(m_pid_allstop_posted)
      return;    
    Notify("DESIRED_RUDDER", 0.0);
    Notify("DESIRED_THRUST", 0.0);
    if(m_pengine.hasDepthControl())
      Notify("DESIRED_ELEVATOR", 0.0);
    m_pid_allstop_posted = true;
  }
  else {
    Notify("DESIRED_RUDDER", m_pengine.getDesiredRudder());
    Notify("DESIRED_THRUST", m_pengine.getDesiredThrust());
    if(m_pengine.hasDepthControl())
      Notify("DESIRED_ELEVATOR", m_pengine.getDesiredElevator());
    m_pid_allstop_posted = false;
  }
#endif
}




//------------------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//
// Datum: 43.825300,  -71.087589, (MIT Sailing Pavilion)
//
//   Starting Pose                Current Pose
//   -------- ----------         -------- -----------
//   Heading: 180                Heading: 134.8
//     Speed: 0                    Speed: 1.2
//     Depth: 0                    Depth: 37.2  
//  Altitude: 58                Altitude: 20.8
//     (X,Y): 0,0                  (X,Y): -4.93,-96.05
//       Lat: 43.8253                Lat: 43.82443465       
//       Lon: -70.3304               Lon: -70.33044214      

//  External Drift  X   Y   |  Mag  Ang  |  Rotate  |  Source(s)  
//  --------------  --  --  |  ---  ---  |  ------  |  -----------
//        Starting  0   0   |  0    0    |  0       |  init_config
//         Present  0   0   |  0    0    |  0       |  n/a        
//
//   Dual state: true
// 
//  DESIRED_THRUST=24 ==> Speed=1.2
//  Using Thrust Factor: false
//  Positive Thrust Map: 0:1, 20:2.4, 50:4.2, 80:4.8, 100:5.0
//  Negative Thrust Map: -100:-3.5, -75:-3.2, -10:-2,
//     Max Acceleration: 0
//     Max Deceleration: 0.5
//
//  DESIRED_ELEVATOR=-12 ==> Depth=37.2
//        Max Depth Rate: 0.5 
//  Max Depth Rate Speed: 2.0
//           Water depth: 58

bool USM_MOOSApp::buildReport()
{
  NodeRecord record = m_model.getNodeRecord();
  double nav_x   = record.getX();
  double nav_y   = record.getY();
  double nav_alt = m_model.getWaterDepth() - record.getDepth();
  if(nav_alt < 0)
    nav_alt = 0;

  NodeRecord record_gt = m_model.getNodeRecordGT();
  double nav_x_gt   = record_gt.getX();
  double nav_y_gt   = record_gt.getY();
  double nav_alt_gt = m_model.getWaterDepth() - record_gt.getDepth();
  if(nav_alt_gt < 0)
    nav_alt_gt = 0;

  bool dual_state = m_model.usingDualState();

  double nav_lat = 0;
  double nav_lon = 0;
  double nav_lat_gt = 0;
  double nav_lon_gt = 0;

  if(m_model.geoOK()) {
    nav_lat = record.getLat();
    nav_lon = record.getLon();
    nav_lat_gt = record_gt.getLat();
    nav_lon_gt = record_gt.getLon();
  }

  string datum_lat = m_model.getStartDatumLat();
  string datum_lon = m_model.getStartDatumLon();
 
  string wmod_str  = m_model.getWindModelSpec();
  string polar_str = m_model.getPolarPlotSpec();
  string sailing_str = boolToString(m_model.sailingEnabled());
  string trate_str = doubleToStringX(m_model.getTurnRate(),2);
  
  m_msgs << "Enabled:  " + boolToString(m_enabled) << endl;
  m_msgs << "TurnRate: " + trate_str << endl;
  m_msgs << "Datum: " + datum_lat + "," + datum_lon << endl;
  m_msgs << "Sailing:" << endl;
  m_msgs << "  WindModel: " << wmod_str << endl;
  m_msgs << "  PolarPlot: " << polar_str << endl;
  m_msgs << "  Enabled:   " << sailing_str << endl;
 
  m_msgs << endl << endl;
  // Part 1: Pose Information ===========================================
  ACTable actab(6,1);
  actab << "Start | Pose | Current | Pose (NAV) | Current | Pose (GT)";
  actab.addHeaderLines();
  actab.setColumnPadStr(1, "   "); // Pad w/ extra blanks between cols 1&2
  actab.setColumnPadStr(3, "   "); // Pad w/ extra blanks between cols 3&4
  actab.setColumnJustify(0, "right");
  actab.setColumnJustify(2, "right");
  actab.setColumnJustify(4, "right");

  actab << "Headng:" << m_model.getStartNavHdg();
  actab << "Headng:" << doubleToStringX(record.getHeading(),1);
  if(dual_state)
    actab << "Headng:" << doubleToStringX(record.getHeading(),1);
  else
    actab << "(same)" << "-";

  actab << "Speed:" << m_model.getStartNavSpd();
  actab << "Speed:" << doubleToStringX(record.getSpeed(),2);
  if(dual_state)
    actab << "Speed:" << doubleToStringX(record_gt.getSpeed(),2);
  else
    actab << " " << "-";

  if(m_depth_info_acast) {
    actab << "Depth:" << m_model.getStartNavDep();
    actab << "Depth:" << doubleToStringX(record.getDepth(),1);
    if(dual_state)
      actab << "Depth:" << doubleToStringX(record_gt.getDepth(),1);
    else
      actab << " " << "-";
    
    actab << "Alt:" << m_model.getStartNavAlt();
    actab << "Alt:" << doubleToStringX(nav_alt,1);
    if(dual_state)
      actab << "Alt:" << doubleToStringX(nav_alt_gt,1);
    else
      actab << " " << "-";
  }
  
  actab << "(X,Y):" << m_model.getStartNavX() +","+ m_model.getStartNavY();
  actab << "(X,Y):" << doubleToStringX(nav_x,2) + "," + doubleToStringX(nav_y,2); 
  if(dual_state)
    actab << "(X,Y):" << doubleToStringX(nav_x_gt,2) +","+ doubleToStringX(nav_y_gt,2); 
  else
    actab << " " << "-";

  actab << "Lat:" << m_model.getStartNavLat();
  actab << "Lat:" << doubleToStringX(nav_lat,8);
  if(dual_state)
    actab << "Lat:" << doubleToStringX(nav_lat_gt,8);
  else
    actab << " " << "-";

  actab << "Lon:" << m_model.getStartNavLon();
  actab << "Lon:" << doubleToStringX(nav_lon,8);
  if(dual_state)
    actab << "Lon:" << doubleToStringX(nav_lon_gt,8);
  else
    actab << " " << "-";

  m_msgs << actab.getFormattedString();

  // Part 2: Buoyancy Info ==============================================
  string buoy_rate_now = doubleToStringX(m_model.getBuoyancyRate(),8);

  m_msgs <<  endl << endl;
  m_msgs <<  "Present Buoyancy rate: " << buoy_rate_now << endl;
  m_msgs <<  "        Starting rate: " << m_model.getStartBuoyancy() << endl;

  // Part 3: External Drift Info =======================================
  m_msgs << endl;
  actab = ACTable(7,2);
  actab << "Ext Drift | X | Y | Mag | Ang | Rot. |Source(s)";
  actab.addHeaderLines();
  actab.setColumnJustify(0, "right");
  actab.setColumnPadStr(2, "  |  ");
  actab.setColumnPadStr(4, "  |  ");
  actab.setColumnPadStr(5, "  |  ");

  actab << "Starting" << m_model.getStartDriftX();
  actab << m_model.getStartDriftY();
  actab << m_model.getStartDriftMag();
  actab << m_model.getStartDriftAng();
  actab << m_model.getStartRotateSpd() << "init_config";
  
  string drift_x   = doubleToStringX(m_model.getDriftX(),3);
  string drift_y   = doubleToStringX(m_model.getDriftY(),3);
  string drift_mag = doubleToStringX(m_model.getDriftMag(),4);
  string drift_ang = doubleToStringX(m_model.getDriftAng(),4);
  string rotate_spd = doubleToStringX(m_model.getRotateSpd(),4);
  if(drift_mag == "0")
    drift_ang = "0";

  string drift_srcs = m_model.getDriftSources();
  if(drift_srcs == "")
    drift_srcs = "n/a";
  actab << "Present" << drift_x << drift_y << drift_mag << drift_ang;
  actab << rotate_spd << drift_srcs;
  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  // Part 4: Speed/Thrust Info =======================================
  m_msgs << "Velocity Information: " << endl;
  m_msgs << "--------------------- " << endl;
  m_msgs << "     DESIRED_THRUST=" << doubleToStringX(m_model.getThrust(),1);
  m_msgs << " ==> SPEED=" << doubleToStringX(record.getSpeed(),2) << endl;
  bool using_thrust_factor = m_model.usingThrustFactor();
  m_msgs << "     Using Thrust_Factor: " << boolToString(using_thrust_factor);
  if(using_thrust_factor)
    m_msgs << " (" + doubleToStringX(m_model.getThrustFactor(),4) + ")";
  m_msgs << endl;

  bool using_sailing = m_model.sailingEnabled();
  m_msgs << "           Using Sailing: " << boolToString(using_sailing); 
  m_msgs << endl;

  
  string max_acceleration = doubleToStringX(m_model.getMaxAcceleration(),6);
  string max_deceleration = doubleToStringX(m_model.getMaxDeceleration(),6);
  string posmap = m_model.getThrustMapPos();
  string negmap = m_model.getThrustMapNeg();
  string fanposmap = m_model.getThrustMapFanPos();
  string fannegmap = m_model.getThrustMapFanNeg();
  string thrust_mode = m_model.getThrustModeDiff();
  string desired_thrust_l = "n/a";
  string desired_thrust_r = "n/a";
  if(thrust_mode == "differential") {
    desired_thrust_l = doubleToStringX(m_model.getThrustLeft(),2);
    desired_thrust_r = doubleToStringX(m_model.getThrustRight(),2);
  }
  string thrust_mode_reverse = boolToString(m_model.getThrustModeReverse());

  m_msgs << "    PID Coupled: " << boolToString(m_pid_coupled) << endl;
  
  if(posmap == "")
    posmap = "n/a";
  if(negmap == "")
    negmap = "n/a";
  m_msgs << "     Positive Thrust Map: " << posmap << endl;
  m_msgs << "     Negative Thrust Map: " << negmap << endl;
  m_msgs << "      Pos Fan Thrust Map: " << fanposmap << endl;
  m_msgs << "      Neg Fan Thrust Map: " << fannegmap << endl;
  m_msgs << "        Max Accereration: " << max_acceleration << endl;
  m_msgs << "        Max Decereration: " << max_deceleration << endl << endl;

  m_msgs << "             Thrust Mode: " << thrust_mode << endl;
  m_msgs << "     Thrust Mode Reverse: " << thrust_mode_reverse << endl;
  m_msgs << "        DESIRED_THRUST_L: " << desired_thrust_l << endl;
  m_msgs << "        DESIRED_THRUST_R: " << desired_thrust_r << endl;


  // Part 5: Speed/Depth Change Info ===========================
  if(m_depth_info_acast) {
    string max_depth_rate   = doubleToStringX(m_model.getMaxDepthRate(),6);
    string max_depth_rate_v = doubleToStringX(m_model.getMaxDepthRateSpd(),6);
    m_msgs << endl;
    m_msgs << "Depth Information: " << endl;
    m_msgs << "------------------ " << endl;
    m_msgs << "           Max Depth Rate: " << max_depth_rate   << endl;
    m_msgs << "     Max Depth Rate Speed: " << max_depth_rate_v << endl;
    m_msgs << "              Water Depth: " << m_model.getWaterDepth() << endl;
  }

  if(m_wormset.size() > 0) {
    m_msgs << endl;
    m_msgs << "WormHole Information:" << endl;
    m_msgs << "---------------------" << endl;
    vector<string> wormset_config_summary = m_wormset.getConfigSummary();
    for(unsigned int i=0; i<wormset_config_summary.size(); i++) {
      m_msgs << wormset_config_summary[i] << endl;
    }
  }

  
  return(true);
}







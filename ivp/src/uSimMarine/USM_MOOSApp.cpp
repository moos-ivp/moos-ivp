/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_MOOSApp.cpp                                      */
/*    DATE: Oct 25th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "USM_MOOSApp.h"
#include "MBUtils.h"
#include "AngleUtils.h"

#define USE_UTM

using namespace std;

//------------------------------------------------------------------------
// Constructor

USM_MOOSApp::USM_MOOSApp() 
{
  m_sim_prefix     = "USM";
  m_reset_count    = 0;
}

//------------------------------------------------------------------------
// Procedure: OnNewMail
//      Note: 

bool USM_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    string key = Msg.m_sKey;
    double dval = Msg.m_dfVal;
    string sval = Msg.m_sVal;

    if(key == "DESIRED_THRUST")
      m_model.setThrust(dval);
    else if(key == "DESIRED_RUDDER")
      m_model.setRudder(dval);
    else if(key == "DESIRED_ELEVATOR")
      m_model.setElevator(dval);
    else if(key == "USM_SIM_PAUSED")
      m_model.setPaused(toupper(sval) == "TRUE");
    else if(key == "USM_BUOYANCY_RATE")
      m_model.setParam("buoyancy_rate", dval);
    else if(key == "USM_FORCE_THETA")
      m_model.setParam("torque_theta", dval);
    else if(key == "USM_FORCE_X")
      m_model.setParam("force_x", dval);
    else if(key == "USM_FORCE_Y")
      m_model.setParam("force_y", dval);
    else if(key == "USM_FORCE_VECTOR")
      m_model.setForceVector(sval, false);
    else if(key == "USM_FORCE_VECTOR_ADD")
      m_model.setForceVector(sval, true);
    else if(key == "USM_FORCE_VECTOR_MULT")
      m_model.magForceVector(dval);
    else if(key == "USM_WATER_DEPTH")
      m_model.setParam("water_depth", dval);
    else if(key == "USM_RESET") {
      m_reset_count++;
      m_Comms.Notify("USM_RESET_COUNT", m_reset_count);
      m_model.initPosition(sval);
    }
    else
      MOOSTrace("Unrecognized command: [%s]\n", key.c_str());
  }

  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnStartUp
//      Note: 

bool USM_MOOSApp::OnStartUp()
{
  MOOSTrace("sim starting\n");
  
  m_model.resetTime(MOOSTime());

  double foobar = 23;
  m_MissionReader.GetConfigurationParam("FOOBAR", foobar);

  cout << "Value of FOOBAR is: " << foobar << endl;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine  = *p;
    string param  = stripBlankEnds(MOOSChomp(sLine, "="));
    string value  = stripBlankEnds(sLine);
    double dval   = atof(value.c_str());

    // Backward support of older param style
    if(param=="StartLon")     param="START_X";
    if(param=="StartLat")     param="START_Y";
    if(param=="StartHeading") param="START_HEADING";
    if(param=="StartDepth")   param="START_DEPTH";
    if(param=="StartSpeed")   param="START_SPEED";

    param = toupper(param);

    if(param == "START_X")
      m_model.setParam(param, dval);
    else if(param == "START_Y")
      m_model.setParam(param, dval);
    else if(param == "START_HEADING")
      m_model.setParam(param, dval);
    else if(param == "START_SPEED")
      m_model.setParam(param, dval);
    else if(param == "START_DEPTH")
      m_model.setParam(param, dval);
    else if(param == "BUOYANCY_RATE")
      m_model.setParam(param, dval);
    else if(param == "FORCE_X")
      m_model.setParam("force_x", dval);
    else if(param == "FORCE_Y")
      m_model.setParam("force_y", dval);
    else if(param == "FORCE_THETA")
      m_model.setParam("torque_theta", dval);
    else if((param == "MAX_ACCELERATION") && isNumber(value))
      m_model.setParam("max_acceleration", dval);
    else if((param == "MAX_DECELERATION") && isNumber(value))
      m_model.setParam("max_deceleration", dval);
    else if((param == "MAX_DEPTH_RATE") && isNumber(value))
      m_model.setParam("max_depth_rate", dval);
    else if((param == "MAX_DEPTH_RATE_SPEED") && isNumber(value))
      m_model.setParam("max_depth_rate_speed", dval);

    else if(param == "PREFIX")
      m_sim_prefix = value;
    else if(param == "FORCE_VECTOR")
      m_model.setForceVector(value);
    else if(param == "SIM_PAUSE")
      m_model.setPaused(tolower(value) == "true");
    else if(param == "DUAL_STATE")
      m_model.setDualState(tolower(value) == "true");
    else if(param == "START_POS")
      m_model.initPosition(value);
    else if(param == "THRUST_REFLECT")
      m_model.setThrustReflect(tolower(value)=="true");
    else if(param == "THRUST_FACTOR")
      m_model.setThrustFactor(dval);
    else if(param == "THRUST_MAP")
      handleThrustMapping(value);
    else if(param == "TURN_RATE")
      m_model.setParam("turn_rate", dval);
    else if(param == "DEFAULT_WATER_DEPTH")
      m_model.setParam("water_depth", dval);
  }

  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if(!m_MissionReader.GetValue("LatOrigin", latOrigin)) {
    MOOSTrace("uSimMarine: LatOrigin not set in *.moos file.\n");
    m_geo_ok = false;
  } 
  else if(!m_MissionReader.GetValue("LongOrigin", longOrigin)) {
    MOOSTrace("uSimMarine: LongOrigin not set in *.moos file\n");
    m_geo_ok = false;      
  }
  else {
    m_geo_ok = true;
    // initialize m_geodesy
    if(!m_geodesy.Initialise(latOrigin, longOrigin)) {
      MOOSTrace("uSimMarine: Geodesy init failed.\n");
      m_geo_ok = false;
    }
  }
  
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
// Procedure: OnConnectToServer
//      Note: 

void USM_MOOSApp::registerVariables()
{
  m_Comms.Register("DESIRED_RUDDER", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  m_Comms.Register("DESIRED_ELEVATOR", 0);

  m_Comms.Register("USM_BUOYANCY_RATE", 0);
  m_Comms.Register("USM_WATER_DEPTH", 0);
  m_Comms.Register("USM_FORCE_X", 0);
  m_Comms.Register("USM_FORCE_Y", 0);
  m_Comms.Register("USM_FORCE_VECTOR", 0);
  m_Comms.Register("USM_FORCE_VECTOR_ADD", 0);
  m_Comms.Register("USM_FORCE_VECTOR_MULT", 0);
  m_Comms.Register("USM_FORCE_THETA", 0);
  m_Comms.Register("USM_PAUSE", 0);
  m_Comms.Register("USM_RESET", 0);
}

//------------------------------------------------------------------------
// Procedure: Iterate
//      Note: This is where it all happens.

bool USM_MOOSApp::Iterate()
{
  double curr_time = MOOSTime();
  m_model.propagate(curr_time);
  
  NodeRecord record = m_model.getNodeRecord();
  postNodeRecordUpdate(m_sim_prefix, record, curr_time);

  if(m_model.usingDualState()) {
    NodeRecord record_gt = m_model.getNodeRecordGT();
    postNodeRecordUpdate(m_sim_prefix+"_GT", record_gt, curr_time);
  }

  if(m_model.isForceFresh()) {
    m_Comms.Notify("USM_FSUMMARY", m_model.getForceSummary());
    m_model.setForceFresh(false);
  }

  return(true);
}

//------------------------------------------------------------------------
// Procedure: postNodeRecordUpdate

void USM_MOOSApp::postNodeRecordUpdate(string prefix, 
				       const NodeRecord &record, 
				       double curr_time)
{
  double nav_x = record.getX();
  double nav_y = record.getY();

  m_Comms.Notify(prefix+"_X", nav_x, curr_time);
  m_Comms.Notify(prefix+"_Y", nav_y, curr_time);

  if(m_geo_ok) {
    double lat, lon;
#ifdef USE_UTM
    m_geodesy.UTM2LatLong(nav_x, nav_y, lat, lon);
#else
    m_geodesy.LocalGrid2LatLong(nav_x, nav_y, lat, lon);
#endif
    m_Comms.Notify(prefix+"_LAT", lat, curr_time);
    m_Comms.Notify(prefix+"_LONG", lon, curr_time);
  }

  double new_speed = record.getSpeed();
  new_speed = snapToStep(new_speed, 0.01);

  m_Comms.Notify(prefix+"_HEADING", record.getHeading(), curr_time);
  m_Comms.Notify(prefix+"_SPEED", new_speed, curr_time);
  m_Comms.Notify(prefix+"_DEPTH", record.getDepth(), curr_time);

  // Added by HS 120124 to make it work ok with iHuxley
  m_Comms.Notify("SIMULATION_MODE","TRUE", curr_time);
  m_Comms.Notify(prefix+"_Z", -record.getDepth(), curr_time);
  m_Comms.Notify(prefix+"_PITCH", record.getPitch(), curr_time);
  m_Comms.Notify(prefix+"_YAW", record.getYaw(), curr_time);
  m_Comms.Notify("TRUE_X", nav_x, curr_time);
  m_Comms.Notify("TRUE_Y", nav_y, curr_time);


  double hog = angle360(record.getHeadingOG());
  double sog = record.getSpeedOG();

  m_Comms.Notify(prefix+"_HEADING_OVER_GROUND", hog, curr_time);
  m_Comms.Notify(prefix+"_SPEED_OVER_GROUND", sog, curr_time);
  
  if(record.isSetAltitude()) 
    m_Comms.Notify(prefix+"_ALTITUDE", record.getAltitude(), curr_time);
  
}

//--------------------------------------------------------------------
// Procedure: handleThrustMapping
//

void USM_MOOSApp::handleThrustMapping(string mapping)
{
  vector<string> svector = parseString(mapping, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string thrust = stripBlankEnds(biteString(svector[i], ':'));
    string speed  = stripBlankEnds(svector[i]);
    if(!isNumber(thrust) || !isNumber(speed))
      return;
    double dthrust = atof(thrust.c_str());
    double dspeed  = atof(speed.c_str());
    m_model.addThrustMapping(dthrust, dspeed);
  }
}


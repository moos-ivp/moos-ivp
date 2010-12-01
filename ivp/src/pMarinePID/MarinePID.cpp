/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarinePID.cpp                                        */
/*    DATE: Apr 10 2006                                          */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iterator>
#include <iostream>
#include <math.h>
#include <string.h>
#include "MarinePID.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

MarinePID::MarinePID()
{
  m_has_control    = false;
  m_allow_overide  = true;
  m_allstop_posted = false;
  m_depth_control  = true;
  m_verbose        = "terse";
  m_speed_factor   = 0;

  m_desired_heading = 0;
  m_desired_speed   = 0;
  m_desired_depth   = 0;

  m_current_heading = 0;
  m_current_speed   = 0;
  m_current_depth   = 0;

  m_rudder_bias_duration  = 10;
  m_rudder_bias_limit     = 0;
  m_rudder_bias_side      = 1;
  m_rudder_bias_timestamp = 0;

  m_iteration    = 0;
  m_start_time   = 0;

  m_paused       = false;

  m_tardy_helm_thresh = 2.0;
  m_tardy_nav_thresh  = 2.0;

  m_time_of_last_helm_msg = 0;
  m_time_of_last_nav_msg  = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool MarinePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
  double curr_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    double dfT;

    msg.IsSkewed(curr_time, &dfT);

    string key = toupper(stripBlankEnds(msg.m_sKey));
    if(key == "SPEED_FACTOR") {
      m_speed_factor = msg.m_dfVal;
      m_pengine.setSpeedFactor(m_speed_factor);
    }

    #define ACCEPTABLE_SKEW_FIX_ME 360.0
    if(fabs(dfT)<ACCEPTABLE_SKEW_FIX_ME) {
      if((key == "MOOS_MANUAL_OVERIDE") ||
	 (key == "MOOS_MANUAL_OVERRIDE")) {
	if(MOOSStrCmp(msg.m_sVal, "FALSE")) {
	  m_has_control = true;
	  MOOSTrace("\n");
	  MOOSDebugWrite("pMarinePID Control Is On");
	}
	else if(MOOSStrCmp(msg.m_sVal, "TRUE")) {
	  if(m_allow_overide) {
	    m_has_control = false;
	    MOOSTrace("\n");
	    MOOSDebugWrite("pMarinePID Control Is Off");
	  }
	}
      }
      else if(key == "PID_VERBOSE") {
	if(msg.m_sVal == "verbose")
	  m_verbose = "verbose";
	else if(msg.m_sVal == "quiet")
	  m_verbose = "quiet";
	else
	  m_verbose = "terse";
      }
      else if(key == "NAV_YAW")
	m_current_heading = -MOOSRad2Deg(msg.m_dfVal);
      else if(key == "NAV_HEADING")
	m_current_heading = msg.m_dfVal;
      else if(key == "NAV_SPEED")
	m_current_speed = msg.m_dfVal;
      else if(key == "NAV_DEPTH")
	m_current_depth = msg.m_dfVal;
      
      if(!strncmp(key.c_str(), "NAV_", 4))
	m_time_of_last_nav_msg = curr_time;

      else if(key == "DESIRED_THRUST")
	m_current_thrust = msg.m_dfVal;
      else if(key == "DESIRED_HEADING") {
	m_desired_heading = msg.m_dfVal;
	m_time_of_last_helm_msg = curr_time;
      }
      else if(key == "DESIRED_SPEED") {
	m_desired_speed = msg.m_dfVal;
	m_time_of_last_helm_msg = curr_time;
      }
      else if(key == "DESIRED_DEPTH")
	m_desired_depth = msg.m_dfVal;
      }
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()
//      Note: happens AppTick times per second
//            

bool MarinePID::Iterate()
{
  m_iteration++;
  postCharStatus();

  if(!m_has_control) {
    postAllStop();
    return(false);
  }

  double current_time = MOOSTime();

  if(m_verbose == "verbose") {
    double hz = m_iteration / (MOOSTime() - m_start_time);
    cout << endl << endl << endl;
    cout << "PID REPORT: (" << m_iteration << ")";
    cout << "(" << hz << "/sec)" << endl;
  }


  if((current_time - m_time_of_last_helm_msg) > m_tardy_helm_thresh) {
    if(!m_paused)
      MOOSDebugWrite("Paused Due To Tardy HELM Input: THRUST=0");
    cout << "Paused Due To Tardy HELM Input: THRUST=0" << endl;
    m_paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }
  
  if((current_time - m_time_of_last_nav_msg) > m_tardy_nav_thresh) {
    if(!m_paused)
      MOOSDebugWrite("Paused Due To Tardy NAV Input: THRUST=0");
    cout << "Paused Due To Tardy NAV Input: THRUST=0" << endl;
    m_paused = true;
    m_Comms.Notify("DESIRED_THRUST", 0);
    return(true);
  }

  double rudder = 0;
  double thrust = 0;
  double elevator = 0;

  m_pengine.updateTime(current_time);

  rudder = m_pengine.getDesiredRudder(m_desired_heading, 
				      m_current_heading);
  
  //--------------------
  double rbias_duration = current_time - m_rudder_bias_timestamp;
  if(rbias_duration > m_rudder_bias_duration) {
    rbias_duration = 0;
    m_rudder_bias_timestamp = current_time;
    m_rudder_bias_side *= -1;
  }
  double pct = rbias_duration / m_rudder_bias_duration;
  double bias = m_rudder_bias_side * (pct * m_rudder_bias_limit);
  rudder += bias;
  //--------------------
  

  thrust = m_pengine.getDesiredThrust(m_desired_speed, 
				      m_current_speed, 
				      m_current_thrust);

  if(m_depth_control)
    elevator = m_pengine.getDesiredElevator(m_desired_depth, 
					    m_current_depth);
  
  if((m_desired_speed <= 0.001) && (m_desired_speed >= -0.001))
    thrust = 0;

  vector<string> pid_report;
  if(m_verbose == "verbose") {
    pid_report = m_pengine.getPIDReport();
    for(unsigned int i=0; i<pid_report.size(); i++)
      cout << pid_report[i] << endl;
  }
  m_pengine.clearReport();

  m_paused = false;

  if(thrust ==0)
    rudder = 0;
  m_Comms.Notify("DESIRED_RUDDER", rudder);
  m_Comms.Notify("DESIRED_THRUST", thrust);
  if(m_depth_control)
    m_Comms.Notify("DESIRED_ELEVATOR", elevator);

  m_allstop_posted = false;
  
  return(true);
}
  

//------------------------------------------------------------
// Procedure: postCharStatus()

void MarinePID::postCharStatus()
{
  if(m_has_control) {
    if(m_verbose == "terse")
      MOOSTrace("$");
  }
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: postAllStop()

void MarinePID::postAllStop()
{
  if(m_allstop_posted)
    return;

  m_Comms.Notify("DESIRED_RUDDER", 0);
  m_Comms.Notify("DESIRED_THRUST", 0);
  if(m_depth_control)
    m_Comms.Notify("DESIRED_ELEVATOR", 0);

  m_allstop_posted = true;
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool MarinePID::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  registerVariables();

  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void MarinePID::registerVariables()
{
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register("DESIRED_HEADING", 0);
  m_Comms.Register("DESIRED_SPEED", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  m_Comms.Register("DESIRED_DEPTH", 0);
  m_Comms.Register("PID_VERBOSE", 0);
  m_Comms.Register("SPEED_FACTOR", 0);
  m_Comms.Register("MOOS_MANUAL_OVERIDE", 0);
  m_Comms.Register("MOOS_MANUAL_OVERRIDE", 0);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool MarinePID::OnStartUp()
{
  MOOSTrace("pMarinePID starting....\n");
  
  m_start_time = MOOSTime();
  // m_speed_factor=0 means speed PID will be used.
  m_speed_factor = 0;
  
  string pid_logpath  = "";
   
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = toupper(biteString(sLine, '='));
    string value = stripBlankEnds(sLine);
    double dval  = atof(value.c_str());
    
    if(param == "SPEED_FACTOR")
      m_speed_factor = dval;

    else if(param == "SIM_INSTABILITY")
      m_rudder_bias_limit = dval;
    
    else if((param == "TARDY_HELM_THRESHOLD") && (dval >= 0))
      m_tardy_helm_thresh = dval;
    
    else if((param == "TARDY_NAV_THRESHOLD") && (dval >= 0))
      m_tardy_nav_thresh = dval;
    
    else if(param == "ACTIVE_START") {
      if(tolower(value) == "true")
	m_has_control = true;
    }
    
    else if(param == "VERBOSE") {
      string lval = tolower(value);
      if((lval == "true") || (lval == "verbose"))
	m_verbose = "verbose";
      if(lval == "terse") 
	m_verbose = "terse";
      if(lval == "quiet")
	m_verbose = "quiet";
    }
    
    else if(param == "LOGPATH")
      pid_logpath = value;
  }

  bool ok_yaw = handleYawSettings();
  bool ok_spd = handleSpeedSettings();
  bool ok_dep = handleDepthSettings();
  
  if(!ok_yaw || !ok_spd || !ok_dep) {
    MOOSTrace("Improper PID Settings \n");
    return(false);
  }

  if(m_depth_control)
    cout << "Depth Control is ON" << endl;
  else
    cout << "Depth Control is OFF" << endl;

  registerVariables();
  
  return(true);
}

//--------------------------------------------------------------------
// Procedure: handleYawSettings

bool MarinePID::handleYawSettings()
{
  double yaw_pid_kp   = 0.5;
  double yaw_pid_kd   = 0;
  double yaw_pid_ki   = 0;
  double yaw_pid_ilim = 1.0;
  double max_rudder   = 100;
  double step_rudder  = 0;

  bool   yaw_set_kp = false;
  bool   yaw_set_kd = false;
  bool   yaw_set_ki = false;
  bool   yaw_set_ilim = false;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = toupper(biteString(sLine, '='));
    string value = stripBlankEnds(sLine);
    double dval  = atof(value.c_str());

    if((param == "YAW_PID_KP") && (dval >= 0)) {
      yaw_pid_kp = dval;
      yaw_set_kp = true;
    }
    else if((param == "YAW_PID_KD")  && (dval >= 0)) {
      yaw_pid_kd = dval;
      yaw_set_kd = true;
    }
    else if((param == "YAW_PID_KI")  && (dval >= 0)) {
      yaw_pid_ki = dval;
      yaw_set_ki = true;
    }
    else if((param == "YAW_PID_KI_LIMIT")  && (dval >= 0)) {
      yaw_pid_ilim = dval;
      yaw_set_ilim = true;
    }
    else if((param == "YAW_PID_INTEGRAL_LIMIT")  && (dval >= 0)) {
      yaw_pid_ilim = dval;
      yaw_set_ilim = true;
    }
    else if(((param=="MAX_RUDDER")||(param=="MAXRUDDER")) && (dval >= 0))
      max_rudder = dval;
    else if((param == "STEP_RUDDER")  && (dval >= 0))
      step_rudder = dval;
  }

  if(!yaw_set_kp || !yaw_set_kd || !yaw_set_ki || !yaw_set_ilim) {
    cout << termColor("red");
    if(!yaw_set_kp) 
      cout << "YAW_PID_KP not found in Mission File" << endl;
    if(!yaw_set_kd)
      cout << "YAW_PID_KD not found in Mission File" << endl;
    if(!yaw_set_ki) 
      cout << "YAW_PID_KI not found in Mission File" << endl;
    if(!yaw_set_ilim) 
      cout << "YAW_PID_INTEGRAL_LIMIT not found in Mission File" << endl;
    cout << termColor() << flush;
    return(false);
  }

  ScalarPID crsPID;
  crsPID.SetGains(yaw_pid_kp, yaw_pid_kd, yaw_pid_ki);
  crsPID.SetLimits(yaw_pid_ilim, max_rudder);
  m_pengine.setPID(0, crsPID);
  m_pengine.setRudderStep(step_rudder);

  cout << termColor("blue") << flush;
  cout << "** NEW YAW CONTROLLER VALUES **" << endl;
  cout << "  YAW_PID_KP   = " << doubleToString(yaw_pid_kp, 3) << endl;
  cout << "  YAW_PID_KD   = " << doubleToString(yaw_pid_kd, 3) << endl;
  cout << "  YAW_PID_KI   = " << doubleToString(yaw_pid_ki, 3) << endl;
  cout << "  YAW_PID_ILIM = " << doubleToString(yaw_pid_ilim, 3) << endl;
  cout << "  MAX_RUDDER   = " << doubleToString(max_rudder,  2) << endl;
  cout << "  STEP_RUDDER  = " << doubleToString(step_rudder, 5) << endl;
  cout << termColor() << flush;
  return(true);
}
  

//--------------------------------------------------------------------
// Procedure: handleSpeedSettings

bool MarinePID::handleSpeedSettings()
{
  double spd_pid_kp    = 0.5;
  double spd_pid_kd    = 0;
  double spd_pid_ki    = 0;
  double spd_pid_ilim  = 1.0;
  double max_thrust    = 100;
  double step_thruster = 0;

  bool   spd_set_kp = false;
  bool   spd_set_kd = false;
  bool   spd_set_ki = false;
  bool   spd_set_ilim = false;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = toupper(biteString(sLine, '='));
    string value = stripBlankEnds(sLine);
    double dval  = atof(value.c_str());

    if((param == "SPEED_PID_KP") && (dval >= 0)) {
      spd_pid_kp = dval;
      spd_set_kp = true;
    }
    else if((param == "SPEED_PID_KD")  && (dval >= 0)) {
      spd_pid_kd = dval;
      spd_set_kd = true;
    }
    else if((param == "SPEED_PID_KI")  && (dval >= 0)) {
      spd_pid_ki = dval;
      spd_set_ki = true;
    }
    else if((param == "SPEED_PID_KI_LIMIT")  && (dval >= 0)) {
      spd_pid_ilim = dval;
      spd_set_ilim = true;
    }
    else if((param == "SPEED_PID_INTEGRAL_LIMIT")  && (dval >= 0)) {
      spd_pid_ilim = dval;
      spd_set_ilim = true;
    }
    else if(((param=="MAX_THRUST")||(param=="MAXTHRUST")) && (dval >= 0))
      max_thrust = dval;
    else if((param == "STEP_THRUSTER")  && (dval >= 0))
      step_thruster = dval;
  }

  if(!spd_set_kp || !spd_set_kd || !spd_set_ki || !spd_set_ilim) {
    cout << termColor("red");
    if(!spd_set_kp) 
      cout << "SPEED_PID_KP not found in Mission File" << endl;
    if(!spd_set_kd)
      cout << "SPEED_PID_KD not found in Mission File" << endl;
    if(!spd_set_ki) 
      cout << "SPEED_PID_KI not found in Mission File" << endl;
    if(!spd_set_ilim) 
      cout << "SPEED_PID_INTEGRAL_LIMIT not found in Mission File" << endl;
    cout << termColor() << flush;
    return(false);
  }

  ScalarPID spdPID;
  spdPID.SetGains(spd_pid_kp, spd_pid_kd, spd_pid_ki);
  spdPID.SetLimits(spd_pid_ilim, max_thrust);
  m_pengine.setPID(1, spdPID);
  m_pengine.setThrusterStep(step_thruster);
  m_pengine.setSpeedFactor(m_speed_factor);

  cout << termColor("blue") << flush;
  cout << "** NEW SPEED CONTROLLER VALUES **" << endl;
  cout << "  SPEED_PID_KP    = " << doubleToString(spd_pid_kp, 3) << endl;
  cout << "  SPEED_PID_KD    = " << doubleToString(spd_pid_kd, 3) << endl;
  cout << "  SPEED_PID_KI    = " << doubleToString(spd_pid_ki, 3) << endl;
  cout << "  SPEED_PID_ILIM  = " << doubleToString(spd_pid_ilim, 3) << endl;
  cout << "  MAX_THRUST    = " << doubleToString(max_thrust,  2) << endl;
  cout << "  STEP_THRUSTER = " << doubleToString(step_thruster, 5) << endl;
  cout << termColor() << flush;

  return(true);
}

//--------------------------------------------------------------------
// Procedure: handleDepthSettings()

bool MarinePID::handleDepthSettings()
{
  string depth_ctrl_str = "false";
  m_MissionReader.GetConfigurationParam("DEPTH_CONTROL", depth_ctrl_str);
  depth_ctrl_str  = tolower(depth_ctrl_str);
  m_depth_control = ((depth_ctrl_str == "true") ||
		   (depth_ctrl_str == "1") ||
		   (depth_ctrl_str == "yes"));
  
  if(!m_depth_control)
    return(true);

  double dep_pid_kp    = 0.5;
  double dep_pid_kd    = 0;
  double dep_pid_ki    = 0;
  double dep_pid_ilim  = 1.0;
  double max_elevator  = 100;
  double step_elevator = 0;

  bool   dep_set_kp = false;
  bool   dep_set_kd = false;
  bool   dep_set_ki = false;
  bool   dep_set_ilim = false;

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = toupper(biteString(sLine, '='));
    string value = stripBlankEnds(sLine);
    double dval  = atof(value.c_str());

    if((param == "DEPTH_PID_KP") && (dval >= 0)) {
      dep_pid_kp = dval;
      dep_set_kp = true;
    }
    else if((param == "DEPTH_PID_KD")  && (dval >= 0)) {
      dep_pid_kd = dval;
      dep_set_kd = true;
    }
    else if((param == "DEPTH_PID_KI")  && (dval >= 0)) {
      dep_pid_ki = dval;
      dep_set_ki = true;
    }
    else if((param == "DEPTH_PID_KI_LIMIT")  && (dval >= 0)) {
      dep_pid_ilim = dval;
      dep_set_ilim = true;
    }
    else if((param == "DEPTH_PID_INTEGRAL_LIMIT")  && (dval >= 0)) {
      dep_pid_ilim = dval;
      dep_set_ilim = true;
    }
    else if(((param == "MAX_ELEVATOR") || (param == "MAXELEVATOR")) 
	    && (dval >= 0))
      max_elevator = dval;
    else if((param == "STEP_ELEVATOR")  && (dval >= 0))
      step_elevator = dval;
  }

  if(!dep_set_kp || !dep_set_kd || !dep_set_ki || !dep_set_ilim) {
    cout << termColor("red");
    cout << "WARNING!!!!!" << endl;
    if(!dep_set_kp) 
      cout << "  DEPTH_PID_KP not found in Mission File" << endl;
    if(!dep_set_kd)
      cout << "  DEPTH_PID_KD not found in Mission File" << endl;
    if(!dep_set_ki) 
      cout << "  DEPTH_PID_KI not found in Mission File" << endl;
    if(!dep_set_ilim) 
      cout << "  DEPTH_PID_INTEGRAL_LIMIT not found in .moos File" << endl;
    
    cout << "Please consult the latest document on pMarinePID.  " << endl;
    cout << "The method for depth PID configuration has changed." << endl;
    cout << "Default values are are being used. " << endl;
    cout << termColor() << flush;
  }

  ScalarPID depPID;
  depPID.SetGains(dep_pid_kp, dep_pid_kd, dep_pid_ki);
  depPID.SetLimits(dep_pid_ilim, max_elevator);
  m_pengine.setPID(2, depPID);

  cout << termColor("blue") << flush;
  cout << "** NEW DEPTH CONTROLLER VALUES **" << endl;
  cout << "  DEPTH_PID_KP   = " << doubleToString(dep_pid_kp, 3) << endl;
  cout << "  DEPTH_PID_KD   = " << doubleToString(dep_pid_kd, 3) << endl;
  cout << "  DEPTH_PID_KI   = " << doubleToString(dep_pid_ki, 3) << endl;
  cout << "  DEPTH_PID_ILIM = " << doubleToString(dep_pid_ilim, 3) << endl;
  cout << "  MAX_ELEVATOR   = " << doubleToString(max_elevator, 2) << endl;
  cout << "  STEP_ELEVATOR  = " << doubleToString(step_elevator,5) << endl;
  cout << termColor() << flush;

  return(true);
}










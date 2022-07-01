/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MarinePID.cpp                                        */
/*    DATE: Apr 10 2006                                          */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "MarinePID.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

MarinePID::MarinePID()
{
  m_pid_allstop_posted = false;
  m_pid_ignore_nav_yaw = false;
  m_pid_verbose    = true;
  m_pid_ok_skew    = 360;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool MarinePID::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key   = msg.m_sKey;
    string sval = msg.m_sVal;
    double dval = msg.m_dfVal;
    
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
      else if(!m_pid_ignore_nav_yaw && (key == "NAV_YAW"))
	m_pengine.setCurrHeading(angle360(-MOOSRad2Deg(dval)));
      else if(key == "NAV_HEADING") 
	m_pengine.setCurrHeading(angle360(dval));
      else if(key == "NAV_SPEED")
	m_pengine.setCurrSpeed(dval);
      else if(key == "NAV_DEPTH")
	m_pengine.setCurrDepth(dval);
      else if(key == "NAV_PITCH")
	m_pengine.setCurrPitch(dval);      
    }
  }

  return(true);
}

//--------------------------------------------------------------------
// Procedure: Iterate()

bool MarinePID::Iterate()
{
  AppCastingMOOSApp::Iterate();

  m_pengine.updateTime(m_curr_time);
  m_pengine.setDesiredValues();
  
  postPengineResults();
  postPenginePostings();
  postCharStatus();

  AppCastingMOOSApp::PostReport(); 
  return(true);
}
  

//------------------------------------------------------------
// Procedure: postPenginePostings
//   Purpose: Publish any postings the PIDEngine created

void MarinePID::postPenginePostings()
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
// Procedure: postCharStatus()

void MarinePID::postCharStatus()
{
  if(!m_pid_verbose)
    return;
  
  if(m_pengine.hasControl()) 
    cout << "$" << flush;
  else 
    cout << "*" << flush;
}

//------------------------------------------------------------
// Procedure: postPengineResults()

void MarinePID::postPengineResults()
{
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
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool MarinePID::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool MarinePID::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  m_pengine.setStartTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  m_pengine.setConfigParams(sParams);
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = tolower(orig);
    string param = biteStringX(line, '=');
    string value = line;
    //double dval  = atof(value.c_str());

    bool handled = true;
    if(param == "ignore_nav_yaw") 
      handled = setBooleanOnString(m_pid_ignore_nav_yaw, value);
    else if(param == "pid_verbose") 
      handled = setBooleanOnString(m_pid_verbose, value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
    
  bool ok_yaw = m_pengine.handleYawSettings();
  bool ok_spd = m_pengine.handleSpeedSettings();
  bool ok_dep = m_pengine.handleDepthSettings();
  
  if(!ok_yaw || !ok_spd || !ok_dep) {
    reportConfigWarning("Improper PID Setting");
    return(false);
  }

  registerVariables();
  
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void MarinePID::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  
  Register("NAV_HEADING", 0);
  Register("NAV_SPEED", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_PITCH", 0);
  Register("DESIRED_HEADING", 0);
  Register("DESIRED_SPEED", 0);
  Register("DESIRED_DEPTH", 0);
  Register("PID_VERBOSE", 0);
  Register("SPEED_FACTOR", 0);
  Register("MOOS_MANUAL_OVERIDE", 0);
  Register("MOOS_MANUAL_OVERRIDE", 0);

  if(!m_pid_ignore_nav_yaw)
    Register("NAV_YAW", 0);
  else
    UnRegister("NAV_YAW");
}

//-----------------------------------------------------------------
// Procedure: buildReport()
//      Note: AppCast report is purposefully very light since this
//            app may be run at very high frequency, and in high
//            time warp situations may be important.

bool MarinePID::buildReport()
{
  double frequency = m_pengine.getFrequency();
  string str_freq = doubleToString(frequency,3);

  m_msgs << "Frequency:" << str_freq  << endl;
  m_msgs << "PID has_control: " << boolToString(m_pengine.hasControl()) << endl;

  return(true);
}






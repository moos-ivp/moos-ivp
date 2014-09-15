/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PoseKeep.cpp                                         */
/*   DATE: Sep 15th 2014                                        */
/****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ACTable.h"
#include "PoseKeep.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PoseKeep::PoseKeep()
{
  m_curr_heading = 0;
  m_hold_heading = 0;
  m_hold_heading_set = false;
  m_curr_heading_tstamp = 0;

  m_thrust_l = 0;
  m_thrust_r = 0;

  m_heading_diff = 0;
  m_adjustment   = 0;
  m_tolerance    = 2;   // degrees

  m_active = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PoseKeep::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval   = msg.GetDouble();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "NAV_HEADING") {
      m_curr_heading_tstamp = MOOSTime();
      m_curr_heading = dval;
    }
    else if(key == "HOLD_HEADING") {
      m_hold_heading = dval;
      m_hold_heading_set = true;
    }
    else if(key == "HOLD_TOLERANCE") {
      if(dval >= 0)
	m_tolerance = dval;
    }
    else if(key == "DEPLOY") {
      if(tolower(sval) == "hold")
	m_active = true;
      else
	m_active = false;
    }
    
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PoseKeep::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PoseKeep::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_active) 
    adjustHeading();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PoseKeep::OnStartUp()
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
    if((param == "hold_tolerance") && isNumber(value)) 
      handled = handleConfigHoldTolerance(value);
    else if(param == "BAR") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void PoseKeep::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DEPLOY", 0);
  Register("HOLD_X", 0);
  Register("HOLD_Y", 0);
  Register("HOLD_XY", 0);
  Register("HOLD_LOOKAT_X", 0);
  Register("HOLD_LOOKAT_Y", 0);
  Register("HOLD_DURATION", 0);
  Register("HOLD_DURATION", 0);
  Register("HOLD_ENDFLAG", 0);
  Register("HOLD_HEADING", 0);
  Register("NAV_HEADING", 0);
}


//------------------------------------------------------------
// Procedure: adjustHeading

void PoseKeep::adjustHeading() 
{
  if(!m_active)
    return;

  // Heading diff should be in the range [-180, 180]
  m_heading_diff = angle180(m_curr_heading - m_hold_heading);

  if(m_heading_diff < m_tolerance)
    return;

  m_adjustment = m_heading_diff * 100 / 180;

  rotateVehicle(m_adjustment);
}

//------------------------------------------------------------
// Procedure: rotateVehicle
//      Note: 100 --> L:100   R:-100
//           -100 --> L:-100  R:100
//             50 --> L:50    R:-50

void PoseKeep::rotateVehicle(double val) 
{
  val = vclip(val, -100, 100);
  m_thrust_l = val;
  m_thrust_r = -val;
}


//------------------------------------------------------------
// Procedure: handleConfigHoldTolerance

bool PoseKeep::handleConfigHoldTolerance(string value) 
{
  double dval = atof(value.c_str());
  if(dval <= 0) {
    reportConfigWarning("hold_tolerance should be a positive value");
    return(false);
  }

  m_tolerance = dval;
  return(true);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PoseKeep::buildReport() 
{
  string s_curr_heading = doubleToString(m_curr_heading, 2);
  string s_tstamp = doubleToString((MOOSTime() - m_curr_heading_tstamp), 2);
  if(m_curr_heading_tstamp == 0)
    s_tstamp = "n/a";

  string s_hold_heading = doubleToString(m_hold_heading, 2);
  if(!m_hold_heading_set)
    s_hold_heading = "n/a";

  string s_active = boolToString(m_active);


  m_msgs << "============================================" << endl;
  m_msgs << "Settings:                                   " << endl;
  m_msgs << "           Active: " << s_active << endl;
  m_msgs << "  Current Heading: " << s_curr_heading << " (" << s_tstamp << ")" << endl;
  m_msgs << "     Hold Heading: " << s_hold_heading << endl;
  m_msgs << "" << endl;
  m_msgs << "" << endl;
  m_msgs << "============================================ \n";

  return(true);
}

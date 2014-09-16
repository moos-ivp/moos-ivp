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
  m_tolerance    = 1;   // degrees

  m_start_time = 0;

  m_hold_x = 0;
  m_hold_y = 0;

  m_active = false;

  m_duration = -1;   // seconds, -1 means no duration applied
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

    bool handled = true;
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
    else if(key == "NAV_X") {
      m_osx = dval;
      m_osx_set = true;
    }
    else if(key == "NAV_Y") {
      m_osy = dval;
      m_osy_set = true;
    }
    else if(key == "DEPLOY") {
      if(tolower(sval) == "hold") {
	if(m_active == false) {
	  postFlags("active_flags");
	  m_start_time = MOOSTime();
	}
	m_active = true;
      }
      else {
	m_active = false;
	m_start_time = 0;
	postFlags("inactive_flags");
      }
    }
    else if(key == "HOLD_POINT")
      handleMailHoldPoint(sval);
    else if(key == "HOLD_ENDFLAG") 
      handled = handleMailFlag("end_flag", sval);
    else if(key == "HOLD_ACTIVEFLAG") 
      handled = handleMailFlag("active_flag", sval);
    else if(key == "HOLD_INACTIVEFLAG") 
      handled = handleMailFlag("inactive_flag", sval);
    else if(key == "MVIEWER_LCLICK")
      handleMailHoldPoint(sval);
    
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);

    // Post a warning for unhandled mail
    if(!handled)
      reportRunWarning("Unhandled Mail: " + key + "=" + sval);
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

  checkForTimeOut();
  
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
    else if((param == "hold_duration") && isNumber(value)) 
      handled = handleConfigHoldDuration(value);
    else if(param == "endflag") 
      handled = handleConfigFlag("end_flag", value);
    else if(param == "activeflag") 
      handled = handleConfigFlag("active_flag", value);
    else if(param == "inactiveflag") 
      handled = handleConfigFlag("inactive_flag", value);

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
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("NAV_HEADING", 0);
  Register("MVIEWER_LCLICK", 0);
  Register("HOLD_POINT", 0);

  Register("HOLD_DURATION", 0);
  Register("HOLD_ENDFLAG", 0);
  Register("HOLD_HEADING", 0);
}


//------------------------------------------------------------
// Procedure: adjustHeading

void PoseKeep::adjustHeading() 
{
  if(!m_active)
    return;

  // Heading diff should be in the range [-180, 180]
  m_heading_diff = angle180(m_curr_heading - m_hold_heading);
  m_adjustment   = 0;

  double abs_heading_diff = m_heading_diff;
  if(abs_heading_diff < 0)
    abs_heading_diff = abs_heading_diff * -1;

  if(abs_heading_diff > m_tolerance)
    m_adjustment = m_heading_diff * 100 / 180;

  string summary = "diff=" + doubleToString(m_heading_diff,1);
  summary += ", adjust=" + doubleToString(m_adjustment,3);

  if(abs_heading_diff <= m_tolerance)
    return;

  Notify("HOLD_DEBUG", summary);

  m_summaries.push_front(summary);
  if(m_summaries.size() > 15)
    m_summaries.pop_back();

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
  m_thrust_l = -val;
  m_thrust_r = val;
  
  Notify("DESIRED_THRUST_L", m_thrust_l);
  Notify("DESIRED_THRUST_R", m_thrust_r);
}


//------------------------------------------------------------
// Procedure: checkForTimeOut

void PoseKeep::checkForTimeOut() 
{
  // Sanity checks
  if(m_start_time == 0)
    return;
  if(m_duration < 0)
    return;
  
  double elapsed = MOOSTime() - m_start_time;
  if(elapsed < m_duration) 
    return;

  m_active = false;
  postFlags("inactive_flag");
  postFlags("endflag_flag");

  for(unsigned int i=0; i<m_end_flags.size(); i++) {
    VarDataPair pair = m_end_flags[i];
    string var = pair.get_var();
    if(pair.is_string())
      Notify(var, pair.get_sdata());
    else
      Notify(var, pair.get_ddata());
  }
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
// Procedure: handleConfigHoldDuration

bool PoseKeep::handleConfigHoldDuration(string value) 
{
  double dval = atof(value.c_str());
  if(dval <= 0) {
    reportConfigWarning("hold_duration should be a positive value");
    return(false);
  }

  m_duration = dval;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigFlag

bool PoseKeep::handleConfigFlag(string flag_type, string value)
{
  bool ok = addFlag(flag_type, value);
  if(!ok)
    reportConfigWarning("Unhandled " + flag_type + " parameter: " + value);

  return(ok);
}

//------------------------------------------------------------
// Procedure: handleMailFlag

bool PoseKeep::handleMailFlag(string flag_type, string value)
{
  if(tolower(value) == "clear") {
    if((flag_type == "end_flag") || (flag_type == "end_flags"))
      m_end_flags.clear();
    else if((flag_type == "active_flag") || (flag_type == "active_flags"))
      m_active_flags.clear();
    else if((flag_type == "inactive_flag") || (flag_type == "inactive_flags"))
      m_inactive_flags.clear();
    else 
      return(false);
    return(true);
  }

  bool ok = addFlag(flag_type, value);
  if(!ok)
    reportRunWarning("Unhandled Mail: HOLD_ENDFLAG=" + value);

  return(ok);
}

//------------------------------------------------------------
// Procedure: addFlag

bool PoseKeep::addFlag(string flag_type, string str)
{
  // Sanity check
  if((flag_type != "end_flag") && (flag_type != "end_flags") && 
     (flag_type != "active_flag") && (flag_type != "active_flags") && 
     (flag_type != "inactive_flag") && (flag_type != "inactive_flags"))
    return(false);

  bool   is_num  = false;
  string moosvar = biteStringX(str, '=');
  string moosval = str;

  if(isNumber(moosval))
    is_num = true;
  else { 
    bool quoted = false;
    if(isQuoted(moosval)) {
      quoted = true;
      moosval = stripQuotes(moosval);
    }
    if(quoted && isNumber(moosval))
      is_num = true;
  }

  if(strContainsWhite(moosvar))
    return(false);

  VarDataPair pair;
  if(is_num) 
    pair = VarDataPair(moosvar, atof(moosval.c_str()));
  else
    pair = VarDataPair(moosvar, moosval);


  if((flag_type == "end_flag") || (flag_type == "end_flags"))
    m_end_flags.push_back(pair);
  else if((flag_type == "active_flag") || (flag_type == "active_flags"))
    m_active_flags.push_back(pair);
  else 
    m_inactive_flags.push_back(pair);

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailHoldPoint
//   Example: x=-19.0,y=-36.0

bool PoseKeep::handleMailHoldPoint(string str) 
{
  string xpos, ypos;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "x")
      xpos = value;
    else if(param == "y")
      ypos = value;
  }
  
  if((xpos == "") || (ypos == "") || !isNumber(xpos) || !isNumber(ypos)) {
    reportRunWarning("HOLD_POINT x or y value is missing or improper: " + str);
    return(false);
  }
  
  if(!m_osx_set || !m_osy_set) {
    reportRunWarning("HOLD_POINT cannot be set, because nav position unknown");
    return(false);
  }

  m_hold_x = atof(xpos.c_str());
  m_hold_y = atof(ypos.c_str());

  if((m_hold_x == m_osx) && (m_hold_y == m_osy))
    return(false);

  m_hold_heading = relAng(m_osx, m_osy, m_hold_x, m_hold_y);
  
  return(true);
}


//------------------------------------------------------------
// Procedure: postFlags()

bool PoseKeep::postFlags(string flag_type)  
{
  // Sanity check
  if((flag_type != "end_flag") && (flag_type != "end_flags") && 
     (flag_type != "active_flag") && (flag_type != "active_flags") && 
     (flag_type != "inactive_flag") && (flag_type != "inactive_flags"))
    return(false);

  vector<VarDataPair> flags;
  if((flag_type == "end_flag") || (flag_type == "end_flags"))
    flags = m_end_flags;
  else if((flag_type == "active_flag") || (flag_type == "active_flags"))
    flags = m_active_flags;
  else if((flag_type == "inactive_flag") || (flag_type == "inactive_flag"))
    flags = m_inactive_flags;
  
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string var = pair.get_var();
    if(pair.is_string())
      Notify(var, pair.get_sdata());
    else
      Notify(var, pair.get_ddata());
  }

  return(true);
}



//------------------------------------------------------------
// Procedure: buildReport()

bool PoseKeep::buildReport() 
{
  string s_active = boolToString(m_active);
  string s_duration = "n/a";
  if(m_duration > 0)
    s_duration = doubleToStringX(m_duration,2);

  string s_tolerance = doubleToStringX(m_tolerance,1);

  string s_curr_position = "n/a";
  if(m_osx_set && m_osy_set) {
    s_curr_position = "(" + doubleToString(m_osx,1) + ",";
    s_curr_position += doubleToString(m_osy,1) + ")";
  }

  string s_curr_heading = doubleToString(m_curr_heading, 2);
  string s_tstamp = doubleToString((MOOSTime() - m_curr_heading_tstamp), 2);
  if(m_curr_heading_tstamp == 0)
    s_tstamp = "n/a";

  string s_hold_heading = doubleToString(m_hold_heading, 2);
  if(!m_hold_heading_set)
    s_hold_heading = "n/a";

  string s_heading_diff = doubleToString(m_heading_diff, 1);
  string s_adjustment   = doubleToString(m_adjustment, 1);

  string s_thrust_l = "n/a";
  string s_thrust_r = "n/a";
  if(m_active) {
    s_thrust_l = doubleToStringX(m_thrust_l, 1);
    s_thrust_r = doubleToStringX(m_thrust_r, 1);
  }

  string s_time_remaining = "n/a";
  if((m_duration > 0) && (m_active)) {
    double elapsed = MOOSTime() - m_start_time;
    double remaining = m_duration - elapsed;
    s_time_remaining = doubleToString(remaining, 2);
  }

  m_msgs << "============================================" << endl;
  m_msgs << "Settings:                                   " << endl;
  m_msgs << "           Active: " << s_active              << endl;
  m_msgs << "         Duration: " << s_duration            << endl;
  m_msgs << "        Tolerance: " << s_tolerance << " (degrees)" << endl;
  m_msgs << " Current Position: " << s_curr_position       << endl;
  m_msgs << "  Current Heading: " << s_curr_heading << 
    " (updated " << s_tstamp  << " secs ago)" << endl;
  m_msgs << "     Hold Heading: " << s_hold_heading        << endl;
  m_msgs << "" << endl;
  m_msgs << "     Heading Diff: " << s_heading_diff        << endl;
  m_msgs << "       Adjustment: " << s_adjustment          << endl;
  m_msgs << "                                            " << endl;
  m_msgs << " DESIRED_THRUST_L: " << s_thrust_l            << endl;
  m_msgs << " DESIRED_THRUST_R: " << s_thrust_r            << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Time Remaining: " << s_time_remaining         << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "End Flags:                             " << endl;
  for(unsigned int i=0; i<m_end_flags.size(); i++)
    m_msgs << "  " << m_end_flags[i].getPrintable()        << endl;
  m_msgs << "Active Flags:                             " << endl;
  for(unsigned int i=0; i<m_active_flags.size(); i++)
    m_msgs << "  " << m_active_flags[i].getPrintable()     << endl;
  m_msgs << "Inactive Flags:                             " << endl;
  for(unsigned int i=0; i<m_inactive_flags.size(); i++)
    m_msgs << "  " << m_inactive_flags[i].getPrintable()   << endl;
  m_msgs << "                                            " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Summaries:                                  " << endl;
  
  list<string>::iterator p;
  for(p=m_summaries.begin(); p!=m_summaries.end(); p++) {
    string summary = *p;
    m_msgs << "  " << summary << endl;
  }

  return(true);
}

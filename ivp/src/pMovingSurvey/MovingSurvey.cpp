/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT                                             */
/*    FILE: MovingSurvey.cpp                                */
/*    DATE: October 7th, 2019                               */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MovingSurvey.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsSegl.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MovingSurvey::MovingSurvey()
{
  // Initialize config variables
  m_speed   = 0;
  m_heading = 0;
  m_rpm     = 0;

  m_update_interval  = 0;   // 0 means constant updates
  m_reverse_interval = -1;  // -1 means no reversals ever

  m_update_var = "SURVEY_UPDATE";

  // Initialize state variables
  m_active = false;
  m_prev_time = 0;
  m_last_turn_time = 0;
  
  m_survey_x = 0;
  m_survey_y = 0;
}

//---------------------------------------------------------
// Destructor

MovingSurvey::~MovingSurvey()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MovingSurvey::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "DEPLOY") 
       m_active = (tolower(sval) == "true");

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MovingSurvey::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool MovingSurvey::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_active) {
    updateTurnStatus();
    bool survey_updated = updateSurveyLocation();
    if(survey_updated)
      postUpdatedSurvey();
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MovingSurvey::OnStartUp()
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
    if(param == "survey")
      handled = handleConfigSurvey(value);
    else if(param == "heading") 
      handled = setDoubleOnString(m_heading, value);
    else if(param == "speed") 
      handled = setNonNegDoubleOnString(m_speed, value);
    else if(param == "reverse_interval") 
      handled = setNonNegDoubleOnString(m_reverse_interval, value);
   else if(param == "update_interval") 
      handled = setNonNegDoubleOnString(m_update_interval, value);
    else if(param == "rpm") {
      handled = setNonNegDoubleOnString(m_rpm, value);
      if(handled)
	m_dps = (m_rpm * 360) / 60;
    }    
    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MovingSurvey::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DEPLOY", 0);
}

//---------------------------------------------------------
// Procedure: handleConfigSurvey()

bool MovingSurvey::handleConfigSurvey(string str)
{
  XYSegList segl = string2SegList(str);
  if(segl.size() == 0)
    return(false);

  m_survey = segl;
  m_survey_x = segl.get_center_x();
  m_survey_y = segl.get_center_y();

  m_survey.set_label("rsurvey");
  
  return(true);
}


//---------------------------------------------------------
// Procedure: updateSurveyLocation()

bool MovingSurvey::updateSurveyLocation()
{
  // Part 1: Update the time, distance and rotations.
  double curr_time = MOOSTime();
  if(m_prev_time == 0) {
    m_prev_time = curr_time;
    return(false);
  }
  
  double elapsed = curr_time - m_prev_time;
  if(elapsed < m_update_interval)
    return(false);

  m_prev_time = curr_time;	       

  double dist = elapsed * m_speed;
  double degs_per_sec = (m_rpm * 360) / 60;
  double degs = elapsed * degs_per_sec;
  
  // Part 2: Update the survey position
  double px,py;
  projectPoint(m_heading, dist, m_survey_x, m_survey_y, px, py);

  m_survey.new_center(px, py);
  m_survey_x = px;
  m_survey_y = py;

  // Part 3: Update the survey rotation angle
  m_survey.rotate(degs);
  return(true);
}

//---------------------------------------------------------
// Procedure: updateTurnStatus()

void MovingSurvey::updateTurnStatus()
{
  if(m_reverse_interval <= 0)
    return;
  
  if(m_last_turn_time == 0) {
    m_last_turn_time = m_curr_time;
    return;
  }

  double elapsed = m_curr_time - m_last_turn_time;
  if(elapsed < m_reverse_interval)
    return;

  m_heading = angle360(m_heading + 180);
  m_last_turn_time = m_curr_time;
}


//---------------------------------------------------------
// Procedure: postUpdatedSurvey()

void MovingSurvey::postUpdatedSurvey()
{
  // Sanity checks
  if((m_survey.size() == 0) || (m_update_var == ""))
    return;
  
  string msg = "xpoints=" + m_survey.get_spec_pts();
  Notify(m_update_var, msg);

  Notify("VIEW_SEGLIST", m_survey.get_spec());
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MovingSurvey::buildReport() 
{
  string str_spd = doubleToStringX(m_speed,3); 
  string str_hdg = doubleToStringX(m_heading,3); 
  string str_rpm = doubleToStringX(m_rpm,3); 
  string str_up_interval = doubleToStringX(m_update_interval,3); 
  
  m_msgs << "============================================" << endl;
  m_msgs << "Configuration:                              " << endl;
  m_msgs << "  Speed:   " << str_spd << endl;
  m_msgs << "  Heading: " << str_hdg << endl;
  m_msgs << "  RPM:     " << str_rpm << endl;
  m_msgs << "  Update Interval: " << str_up_interval << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State:                                      " << endl;
  m_msgs << "  survey_x: " << doubleToStringX(m_survey_x,2) << endl;
  m_msgs << "  survey_y: " << doubleToStringX(m_survey_y,2) << endl;
  m_msgs << "  active: " << boolToString(m_active) << endl;
  m_msgs << "  survey: " << m_survey.get_spec_pts() << endl;
  m_msgs << "============================================" << endl;

  return(true);
}





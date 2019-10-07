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
#include "XYFormatUtilsSegl.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MovingSurvey::MovingSurvey()
{
  m_speed = 0;
  m_heading = 0;
  m_rpm = 0;

  m_survey_x = 0;
  m_survey_y = 0;

  m_prev_time = 0;
  m_update_freq = 2;    // 2X per second
  
  m_update_var = "SURVEY_UPDATE";
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
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";

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
  // Do your thing here!

  updateSurveyLocation();
  postUpdatedSurvey();

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
  // Register("FOOBAR", 0);
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

void MovingSurvey::updateSurveyLocation()
{
  // Part 1: Update the time, distance and rotations.
  double curr_time = MOOSTime();
  if(m_prev_time == 0) {
    m_prev_time = curr_time;
    return;
  }
  double elapsed = curr_time - m_prev_time;
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
}


//---------------------------------------------------------
// Procedure: postUpdatedSurvey()

void MovingSurvey::postUpdatedSurvey()
{
  // Sanity checks
  if((m_survey.size() == 0) || (m_update_var == ""))
    return;
  
  string msg = "points=" + m_survey.get_spec_pts();
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
  
  m_msgs << "============================================" << endl;
  m_msgs << "Configuration:                              " << endl;
  m_msgs << "  Speed:   " << str_spd << endl;
  m_msgs << "  Heading: " << str_hdg << endl;
  m_msgs << "  RPM:     " << str_rpm << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State:                                      " << endl;
  m_msgs << "  survey_x: " << doubleToStringX(m_survey_x,2) << endl;
  m_msgs << "  survey_y: " << doubleToStringX(m_survey_y,2) << endl;
  m_msgs << "  survey: " << m_survey.get_spec_pts() << endl;
  m_msgs << "============================================" << endl;

  return(true);
}





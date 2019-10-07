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
    if(param == "survey") {
      handled = handleConfigSurvey(value);
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
  double curr_time = MOOSTime();
  
  if(m_prev_time == 0) {
    m_prev_time = curr_time;
    return;
  }
  
  double dist = (curr_time - m_prev_time) * m_speed;
  
  double px,py;
  projectPoint(m_heading, dist, m_survey_x, m_survey_y, px, py);

  m_survey_x = px;
  m_survey_y = py;

  m_prev_time = curr_time;	       
}


//---------------------------------------------------------
// Procedure: postUpdatedSurvey()

void MovingSurvey::postUpdatedSurvey()
{
  // Sanity checks
  if((m_survey.size() == 0) || (m_update_var == ""))
    return;
  
  string msg = m_survey.get_spec_pts();
  Notify(m_update_var, msg);

  Notify("VIEW_SEGLIST", m_survey.get_spec());
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MovingSurvey::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}





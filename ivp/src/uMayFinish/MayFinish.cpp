/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MayFinish.cpp                                   */
/*    DATE: October 31st, 2024                              */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "MayFinish.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MayFinish::MayFinish()
{
  // Config vars
  m_max_db_uptime = -1;  // Neg value means no time limit

  // State vars
  m_exit_val      = -1;  // Not ready to exit
  m_now_db_uptime = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MayFinish::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    string sval = msg.GetString(); 
    double dval = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "DB_UPTIME") 
      m_now_db_uptime = dval;
    
    else if(key == m_finish_var) 
      m_current_val = sval;

    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MayFinish::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool MayFinish::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Actually exit when exiting was determined on previous
  // iteration so we can get one final report posted.
  if(m_exit_val >= 0)
    exit(m_exit_val);
  
  if(m_current_val == m_finish_val)
    m_exit_val = 0;
  
  else if((m_max_db_uptime > 0) && (m_now_db_uptime > m_max_db_uptime))
    m_exit_val = 1;

  // Fake age of last report to ensure one final report.
  if(m_exit_val >= 0)
    m_last_report_time = 0;
    
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MayFinish::OnStartUp()
{
  string directives  = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  //if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
  //  reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "finish_var") 
      handled = setFinishVar(value);
    else if(param == "finish_val") 
      handled = setFinishVal(value);
    else if(param == "max_db_uptime") 
      handled = setMaxDBUpTime(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // Wait to set default values here, not in the constructor so
  // we don't register for variable that we don't need.
  if(m_finish_var == "")
    m_finish_var = "MISSION_EVALUATED";
  if(m_finish_val == "")
    m_finish_val = "true";
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MayFinish::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  if(m_finish_var != "")
    Register(m_finish_var);

  // Only register for DB_UPTIME if max_db_uptime is set.
  // Register("DB_UPTIME");
} 


//---------------------------------------------------------
// Procedure: setFinishVar()

bool MayFinish::setFinishVar(string var)
{
  if((var == "") || strContainsWhite(var))
    return(false);

  m_finish_var = var;
  return(true);
}

//---------------------------------------------------------
// Procedure: setFinishVal()

bool MayFinish::setFinishVal(string val)
{
  if(val == "")
    return(false);
  
  m_finish_val = val;
  return(true);
}

//---------------------------------------------------------
// Procedure: setMaxDBUpTime()

bool MayFinish::setMaxDBUpTime(string val)
{
  bool ok = setDoubleOnString(m_max_db_uptime, val);
  if(!ok)
    return(false);

  Register("DB_UPTIME");
  return(true);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MayFinish::buildReport() 
{
  m_msgs << "Config: " << endl;
  m_msgs << "  Finish Var: " << m_finish_var << endl;
  m_msgs << "  Finish Val: " << m_finish_val << endl;
  m_msgs << "  Max DBTime: " << m_max_db_uptime << endl;
  m_msgs << endl;
  m_msgs << "State:" << endl;
  if(m_max_db_uptime > 0)
    m_msgs << "  DB_UPTIME: " << doubleToStringX(m_now_db_uptime,1) << endl;
  else
    m_msgs << "  DB_UPTIME: n/a" << endl;
  m_msgs << "  " << m_finish_var << ": " << m_current_val << endl;

  if((m_max_db_uptime < 0) && (m_finish_var == ""))
    m_msgs << "THIS APP HAS NO EXIT CONDITIONS: IT WILL CONTINUE PERPETUALLY" << endl;
  
  if(m_exit_val > 0)
    m_msgs << "EXITING: " << m_exit_val << endl;
  
  return(true);
}





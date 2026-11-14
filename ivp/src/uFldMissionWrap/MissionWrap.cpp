/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MissionWrap.cpp                                 */
/*    DATE: November 12th, 2026                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MissionWrap.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MissionWrap::MissionWrap()
{
  // Config vars
  m_required_nodes = 0;

  // State vars
  m_results_posted = false;
  m_all_pass = false;
  m_pct_pass = 0;
  m_avg_score = 0;
  m_min_score = 0;
  m_max_score = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MissionWrap::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr = msg.IsString();
#endif

    bool handled = false;
    if(key == "MISSION_WRAP")  
      handled = handleMailMWrap(sval);
    else if(key == "APPCAST_REQ") // handled by AppCastingMOOSApp
      handled = true;

    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MissionWrap::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()


bool MissionWrap::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_results_posted) {
    updateResults();    
    if(m_map_wraps.size() >= m_required_nodes)
      postResults();
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MissionWrap::OnStartUp()
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
    if(param == "required_nodes")
      handled = setUIntOnString(m_required_nodes, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MissionWrap::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MISSION_WRAP", 0);
}


//------------------------------------------------------------
// Procedure: handleMailMWrap()

bool MissionWrap::handleMailMWrap(string str) 
{
  MWrap mwrap = stringToMWrap(str);

  if(!mwrap.valid())
    return(false);
  
  string vname = mwrap.getVName();

  m_map_wraps[vname] = mwrap;
  
  return(true);
}


//------------------------------------------------------------
// Procedure: updateResults()

void MissionWrap::updateResults() 
{
  if(m_map_wraps.size() == 0)
    return;
  
  bool   all_pass  = true;
  double tot_pass  = 0;
  double tot_score = 0;
  double min_score = 0;
  double max_score = 0;
  
  map<string, MWrap>::iterator p;
  for(p=m_map_wraps.begin(); p!=m_map_wraps.end(); p++) {
    MWrap mwrap = p->second;
    bool pass = mwrap.getPass();
    if(pass)
      tot_pass++;
    else
      all_pass = false;

    tot_score += mwrap.getScore();

    if((p == m_map_wraps.begin()) || (mwrap.getScore() < min_score))
      min_score = mwrap.getScore();
    if((p == m_map_wraps.begin()) || (mwrap.getScore() > max_score))
      max_score = mwrap.getScore();
  }

  m_all_pass  = all_pass;
  m_pct_pass  = tot_pass  / ((double) m_map_wraps.size());
  m_avg_score = tot_score / ((double) m_map_wraps.size());
  m_min_score = min_score;
  m_max_score = max_score;
}

//------------------------------------------------------------
// Procedure: postResults()

void MissionWrap::postResults() 
{
  Notify("MISSION_PASS_ALL", boolToString(m_all_pass));
  Notify("MISSION_PASS_PCT", boolToString(m_pct_pass));
  Notify("MISSION_SCORE_AVG", m_avg_score);
  Notify("MISSION_SCORE_MIN", m_min_score);
  Notify("MISSION_SCORE_MAX", m_max_score);
  Notify("MISSION_COMPLETE", "true");
 
  m_results_posted = true;
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MissionWrap::buildReport() 
{
  string need_nodes = uintToString(m_required_nodes);
  string have_nodes = uintToString(m_map_wraps.size());
  m_msgs << "Configuration: "  << endl;
  m_msgs << "  required_nodes =  " << need_nodes << endl;
  
  ACTable actab(4);
  actab << "VName | Pass | Score | Age ";
  actab.addHeaderLines();

  map<string, MWrap>::iterator p;
  for(p=m_map_wraps.begin(); p!=m_map_wraps.end(); p++) {
    string vname = p->first;
    MWrap  mwrap = p->second;
    bool   pass  = mwrap.getPass();
    double dscore = mwrap.getScore();
    double utc   = mwrap.getUTC();

    string score = doubleToStringX(dscore,2);
    string spass = boolToString(pass);
    string age   = doubleToString((m_curr_time - utc),1);
    
    actab << vname << spass << score << age;
    m_msgs << actab.getFormattedString();
  }

  m_msgs << endl;
  m_msgs << "Summary Results: " << have_nodes << endl;
  m_msgs << "  rcv_nodes = " << have_nodes << endl;
  m_msgs << "  all_pass  = " << boolToString(m_all_pass) << endl;
  m_msgs << "  pct_pass  = " << doubleToString(m_pct_pass,2) << endl;
  m_msgs << "  avg_score = " << doubleToString(m_avg_score,2) << endl;
  m_msgs << "  min_score = " << doubleToString(m_min_score,2) << endl;
  m_msgs << "  max_score = " << doubleToString(m_max_score,2) << endl;
  m_msgs << "Results Posted: " << boolToString(m_results_posted) << endl;


  
  return(true);
}





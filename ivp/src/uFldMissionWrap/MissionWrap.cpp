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
#include "HashUtils.h"
#include "MacroUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MissionWrap::MissionWrap()
{
  // Config vars
  m_required_nodes = 0;
  m_colsep=",";
  
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

    bool handled = true;
    if(key == "MISSION_WRAP")  
      handled = handleMailMWrap(sval);
    else if(key == "MISSION_HASH") 
      handleMailMissionHash(sval);

    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      handled = false;

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
    if(m_map_wraps.size() >= m_required_nodes) {
      postResultsToFile(m_report_columns, true);
      postResultsToMOOS();
    }
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MissionWrap::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "required_nodes")
      handled = setUIntOnString(m_required_nodes, value);
    else if(param == "report_column")
      m_report_columns.push_back(value);
    else if(param == "prereport_column")
      m_prereport_columns.push_back(value);
    else if(param == "report_file") {
      handled = okFileToWrite(value);
      m_report_file = value;
    }
    else if(param == "mission")
      handled = setNonWhiteVarOnString(m_mission, value);
    else if(param == "patch")
      handled = setNonWhiteVarOnString(m_patch, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();
  postResultsToFile(m_prereport_columns, false);

  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MissionWrap::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MISSION_WRAP", 0);
  Register("MISSION_HASH", 0);
}


//---------------------------------------------------------
// Procedure: handleMailMissionHash()

void MissionWrap::handleMailMissionHash(string sval)
{
  m_mission_hash = sval;
  m_mhash_short  = missionHashShort(m_mission_hash);
  m_mhash_lshort = missionHashLShort(m_mission_hash);
  m_mhash_utc    = missionHashUTC(m_mission_hash);
}

//---------------------------------------------------------
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
// Procedure: postResultsToFile()
//   Purpose: Write results content to a given file, based on the
//            columns requested in the configuration.
//      Note: It is called initially near the start of the mission
//            to publish the prereport columns. Then at the finish
//            to publish the remaining report columns.
//      Note: The reason for the prereport columns is that, if only
//            the prereport columns are present in the results file,
//            this is an indication that the mission was aborted
//            due to a hard timeout interrupt. Ensuring every line
//            has at least the prereport columns ensures that each
//            mission launch is represented in the reports file,
//            even if it didn't complete.
//      Note: Invoked initially with prereport and finish=false
//            Invoked at end with report lines and finish=true

void MissionWrap::postResultsToFile(vector<string> columns, bool finish)
{
  if(m_report_file == "")
    return;

  bool file_exists = okFileToRead(m_report_file);
  
  FILE *f = fopen(m_report_file.c_str(), "a");
  if(!f) {
    reportRunWarning("Unable to write to file: " + m_report_file);
    return;
  }

  if(!finish && file_exists)
    fprintf(f, "\n");
  
  // Add separator if finishing a non-empty prereport line
  string line;
  if(finish && (m_prereport_columns.size() != 0))
    line += m_colsep;

  for(unsigned int i=0; i<columns.size(); i++) {
    string column = expandMacros(columns[i]);
    if(i != 0) 
      line += m_colsep;
    line += column;
  }
  
  fprintf(f, "%s", line.c_str());

  fclose(f);
}


//------------------------------------------------------------
// Procedure: postResultsToMOOS()

void MissionWrap::postResultsToMOOS() 
{
  
  Notify("MISSION_PASS_ALL", boolToString(m_all_pass));
  Notify("MISSION_PASS_PCT", doubleToStringX(m_pct_pass,3));
  Notify("MISSION_SCORE_AVG", m_avg_score);
  Notify("MISSION_SCORE_MIN", m_min_score);
  Notify("MISSION_SCORE_MAX", m_max_score);
  Notify("MISSION_EVALUATED", "true");
 
  m_results_posted = true;
}

//-----------------------------------------------------------
// Procedure: expandMacros()

string MissionWrap::expandMacros(string sdata) const
{

  // Handle Mission Hash macros
  if(strContains(sdata, "$[MHASH_SHORT]"))
    sdata = macroExpand(sdata, "MHASH_SHORT", m_mhash_short);
  if(strContains(sdata, "$[MHASH]"))
    sdata = macroExpand(sdata, "MHASH", m_mission_hash);
  if(strContains(sdata, "$[MHASH_LSHORT]"))
    sdata = macroExpand(sdata, "MHASH_LSHORT", m_mhash_lshort);
  if(strContains(sdata, "$[MHASH_UTC]"))
    sdata = macroExpand(sdata, "MHASH_UTC", m_mhash_utc);  

  // Handle Result macros
  if(strContains(sdata, "$[ALL_PASS]")) 
    sdata = macroExpand(sdata, "ALL_PASS", m_all_pass);
  if(strContains(sdata, "$[PCT_PASS]")) 
    sdata = macroExpand(sdata, "PCT_PASS", m_pct_pass);
  if(strContains(sdata, "$[AVG_SCORE]")) 
    sdata = macroExpand(sdata, "AVG_SCORE", m_avg_score);
  if(strContains(sdata, "$[MIN_SCORE]")) 
    sdata = macroExpand(sdata, "MIN_SCORE", m_min_score);
  if(strContains(sdata, "$[MAX_SCORE]")) 
    sdata = macroExpand(sdata, "MAX_SCORE", m_max_score);

  // Handle Time/Date macros
  if(strContains(sdata, "$[YEAR]")) 
    sdata = macroExpand(sdata, "MONTH", getCurrYear());
  if(strContains(sdata, "$[MONTH]")) 
    sdata = macroExpand(sdata, "MONTH", getCurrMonth());
  if(strContains(sdata, "$[DAY]")) 
    sdata = macroExpand(sdata, "DAY", getCurrDay());
  if(strContains(sdata, "$[HOUR]")) 
    sdata = macroExpand(sdata, "HOUR", getCurrHour());
  if(strContains(sdata, "$[MIN]")) 
    sdata = macroExpand(sdata, "MIN", getCurrMinute());
  if(strContains(sdata, "$[SEC]")) 
    sdata = macroExpand(sdata, "SEC", getCurrSeconds());
  if(strContains(sdata, "$[DATE]")) 
    sdata = macroExpand(sdata, "DATE", getCurrDate());
  if(strContains(sdata, "$[TIME]")) 
    sdata = macroExpand(sdata, "TIME", getCurrTime());
  
  return(sdata);
}

  

//------------------------------------------------------------
// Procedure: buildReport()

bool MissionWrap::buildReport() 
{
  string need_nodes = uintToString(m_required_nodes);
  string have_nodes = uintToString(m_map_wraps.size());
  m_msgs << "Configuration: "  << endl;
  m_msgs << "  required_nodes =  " << need_nodes << endl;
  m_msgs << endl;
  
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

    string age = "-";
    if(utc > 0)
      age = doubleToString((m_curr_time - utc),1);
    
    actab << vname << spass << score << age;
   }
  m_msgs << actab.getFormattedString();
 
  m_msgs << endl << endl;
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





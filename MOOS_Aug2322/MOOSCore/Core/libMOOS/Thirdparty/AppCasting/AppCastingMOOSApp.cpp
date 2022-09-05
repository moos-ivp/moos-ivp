/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCastingMOOSApp.cpp                                */
/*    DATE: June 5th 2012                                        */
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

#include <algorithm>
#include <iostream>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#ifndef _WIN32
#include "unistd.h"
#else
#	include <stdio.h>
#	include <io.h>
#	define isatty _isatty  // MSVC define _isatty() instead of isatty()
#endif

using namespace std;
//----------------------------------------------------------------
// Constructor(s)

AppCastingMOOSApp::AppCastingMOOSApp()
{
  m_iteration  = 0;
  m_curr_time  = 0;
  m_start_time = 0;
  m_time_warp  = 1;

  m_last_iterate_time        = 0;
  m_last_report_time         = 0;
  m_last_report_time_appcast = 0;
  m_iterate_start_time       = 0;
  m_term_report_interval     = 0.5;

  m_term_reporting  = true;
  m_new_run_warning = false;
  m_new_cfg_warning = false;

  // If "log" stdio is posted. If "file" stdio goes to a file.
  m_app_logging   = "off";
  m_deprecated_ok = false;

  m_comms_policy = "open";
  m_comms_policy_config = "open";
}

//----------------------------------------------------------------
// Procedure: Iterate()

bool AppCastingMOOSApp::Iterate()
{
  m_iteration++;
  m_curr_time = MOOSTime();

  if(m_app_logging == "log") {

    // Part 1: Put together the report from the PREVIOUS iteration                        
    string str_log;
    if(m_iteration == 1)
      str_log = "iter=1,log=";
    if(m_app_logging_info != "")
      str_log = m_app_logging_info + ",";
    
    
    str_log += m_cout.str();
    // Replace all newline chars with "!@#"
    char rs_char = 30; 
    std::replace(str_log.begin(), str_log.end(), '\n', rs_char);
    size_t index = 0;
    string rs_str(1, rs_char);
    while(true) {
      index = str_log.find(rs_str, index);
      if(index == std::string::npos)
	break;
      str_log.replace(index, 1, "!@#");
      index += 3;
    }

    // Part 2: Publish the report
    Notify("APP_LOG", str_log);

    // Part 3: Create the base message for the next iteration 
    m_cout.str("");
    m_cout << "iter=" << m_iteration +1 << ",log=";
    m_app_logging_info = "";
  }

  //m_cout << "test message on iteration " << m_iteration << endl;
  
  // Handle the construction of the ITER_GAP
  if(m_last_iterate_time != 0) {
    double app_freq = GetAppFreq();
    if(app_freq > 0) {
      double app_gap = 1.0 / app_freq;
      double iter_gap = m_curr_time - m_last_iterate_time;
      string app_name = MOOSToUpper((const string&)(m_sMOOSName));
      string var = app_name + "_ITER_GAP";
      Notify(var,  (iter_gap / app_gap));
    }
  }
  m_last_iterate_time = m_curr_time;

  // Prepare the front end of calculating the ITER_LEN
  m_iterate_start_time = m_curr_time;

  return(true);
}

//----------------------------------------------------------------
// Procedure: PostReport()

void AppCastingMOOSApp::PostReport(const string& directive)
{
  m_ac.setIteration(m_iteration);

  double app_freq = GetAppFreq();
  if(app_freq > 0) {
    double app_gap = 1.0 / app_freq;
    double iter_len = MOOSTime() - m_iterate_start_time;
    string app_name = MOOSToUpper((const string&)(m_sMOOSName));
    string var = app_name + "_ITER_LEN";
    Notify(var,  (iter_len / app_gap));
  }

  if(m_time_warp <= 0)
    return;

  // By default
  bool term_reporting  = m_term_reporting;
  bool appcast_allowed = true;
  if(directive.find("noterm") != string::npos)
    term_reporting = false;
  else if(directive.find("doterm") != string::npos)
    term_reporting = true;

  if(directive.find("noappcast") != string::npos)
    appcast_allowed = false;

  double moos_elapsed_time_term = m_curr_time - m_last_report_time;
  double real_elapsed_time_term = moos_elapsed_time_term / m_time_warp;

  double moos_elapsed_time_appcast = m_curr_time - m_last_report_time_appcast;
  double real_elapsed_time_appcast = moos_elapsed_time_appcast / m_time_warp;

  if((real_elapsed_time_term < m_term_report_interval) &&
     (real_elapsed_time_appcast < m_term_report_interval))
    return;

  bool appcast_pending = false;
  if(appcast_allowed)
    appcast_pending = appcastRequested() || (m_iteration < 2);

  // If no report for terminal and no report for appcast, just return!
  if(!term_reporting && !appcast_pending)
    return;

  // If not appcasting and not yet ready for term output, just return!
  if(!appcast_pending && (real_elapsed_time_term < m_term_report_interval))
    return;

  // If not term reporting and not yet ready for appcast, just return!
  if(!term_reporting && (real_elapsed_time_appcast < m_term_report_interval))
    return;

  // Clear the messages. Boilerplate 2-line sequence for clearing a stringstream
  m_msgs.clear();
  m_msgs.str("");

  bool report_built = buildReport();
  if(!report_built)
    return;

  m_ac.msg(m_msgs.str());

  if(term_reporting) {
    //m_new_run_warning = false;
    //m_new_cfg_warning = false;
    m_last_report_time = m_curr_time;
    cout << "\n\n\n\n\n\n\n\n\n";
    cout << m_ac.getFormattedString();
  }

  if(appcast_pending) {
    m_new_run_warning = false;
    m_new_cfg_warning = false;
    m_last_report_time_appcast = m_curr_time;
    m_Comms.Notify("APPCAST", m_ac.getAppCastString());
  }
}

//----------------------------------------------------------------
// Procedure: OnStartUp()

bool AppCastingMOOSApp::OnStartUp()
{
  preOnStartUp();
  return(OnStartUpDirectives());
}

//----------------------------------------------------------------
// Procedure: preOnStartUp()
//      Note: Do this prior to OnStartUp in case stdout has been
//            redirected, it will catch all the startup output.

void AppCastingMOOSApp::preOnStartUp()
{
  STRING_LIST sParams;
  string config_block = GetAppName();
  m_MissionReader.GetConfiguration(config_block, sParams);

  m_MissionReader.GetValue("COMMUNITY", m_host_community);
    
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); ++p) {
    string line  = *p;
    string param = MOOSToUpper(MOOSChomp(line, "="));
    string value = line;
    string lvalue = value;
    MOOSToLower(lvalue);
    MOOSTrimWhiteSpace(param);
    MOOSTrimWhiteSpace(value);

    if(param == "COMMS_POLICY") {
      if((lvalue != "open") && (lvalue != "lean") && (lvalue != "dire"))
	reportConfigWarning("Invalid comms_policy: " + value);
      else {
	m_comms_policy = lvalue;
	m_comms_policy_config = lvalue;
      }
    }
    else if(param == "APP_LOGGING") {
      if(lvalue == "true")
	lvalue = "log";
      if((lvalue == "log") || (lvalue == "file") || (lvalue == "off"))
	m_app_logging = lvalue;
      else
	reportConfigWarning("Invalid APP_LOGGING: " + value);
    }

    else if(param == "DEPRECATED_OK") {
      if(lvalue == "true")
	m_deprecated_ok = true;
      else if(lvalue == "false")
	m_deprecated_ok = false;
      else
	reportConfigWarning("Invalid DEPRECATED_OK: " + value);
    }
  }

  if(m_app_logging == "log")
    std::cout.rdbuf(m_cout.rdbuf());

  if(deprecated() && !m_deprecated_ok) {
    string msg1 = GetAppName() + " is deprecated.";
    string msg2 = "Set deprecated_ok = true to silence deprecation warning.";
    reportRunWarning(msg1);
    reportRunWarning(msg2);
    if(m_deprecated_alt != "")
      reportRunWarning("The supported alternative is: " + m_deprecated_alt);
  }
  
#if 0
  else if(m_app_logging == "file") {
    string filename = ("cout_" + m_host_community + "_" + m_sMOOSName);
    MOOSToLower(filename);
    m_outfile = std::ofstream(filename.c_str());
    std::cout.rdbuf(m_outfile.rdbuf()); //redirect std::cout to out.txt!
  }
#endif
}

//----------------------------------------------------------------
// Procedure: OnStartUpDirectives

bool AppCastingMOOSApp::OnStartUpDirectives(string directives)
{
  // First handle any special directives
  bool   must_have_moosblock = true;
  bool   must_have_community = true;
  string alt_config_block_name;

  while(directives != "") {
    string directive = MOOSChomp(directives, ",");
    MOOSTrimWhiteSpace(directive);
    string left  = MOOSChomp(directive, "=");
    string right = directive;

    MOOSTrimWhiteSpace(left);
    MOOSTrimWhiteSpace(right);
    if(MOOSStrCmp(left, "must_have_moosblock"))
      must_have_moosblock = MOOSStrCmp(right, "true");
    if(MOOSStrCmp(left, "must_have_community"))
      must_have_community = MOOSStrCmp(right, "true");
    else if(MOOSStrCmp(left, "alt_config_block_name"))
      alt_config_block_name = right;
  }
  // Done handling special directives

  bool   return_value = true;
  string appstart = GetAppName() + " starting ...";

  cout << "***************************************************" << endl;
  cout << "*  " << appstart                                     << endl;
  cout << "***************************************************" << endl;

  // #1 Global Config Variable: Determining the MOOSDB community name.
  // Missing community name may be a show-stopper, make sure false is 
  // returned, but continue starting up. Let the individual app 
  // developer decide how to interpret a return of false.
  if(!m_MissionReader.GetValue("COMMUNITY", m_host_community)) {
    if(must_have_community) {
      reportConfigWarning("XCommunity/Vehicle name not found in mission file");
      return_value = false;
    }
  }

  // #2 Global Config Variable: Determining if terminal reports are suppressed
  string term_reporting;
  if(m_MissionReader.GetValue("TERM_REPORTING", term_reporting)) {
    if(MOOSStrCmp(term_reporting, "false")) {
      m_term_reporting = false;
      cout << "Terminal reports suppressed";
    }
    else if(!MOOSStrCmp(term_reporting, "true"))
      reportConfigWarning("Invalid value for TERM_REPORTING: " + term_reporting);
  }
  
  // #3 Allow certain appcasting defaults to be overridden
  STRING_LIST sParams;
  string config_block = GetAppName();
  if(alt_config_block_name != "")
    config_block = alt_config_block_name;

  // #4 Check if there is a config block and if config block is mandatory
  if(!m_MissionReader.GetConfiguration(config_block, sParams)) {
    if(must_have_moosblock) {
      reportConfigWarning("No mission config block found for " + config_block);
      return_value = false;
    }
  }

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); ++p) {
    string line  = *p;
    string param = MOOSToUpper(MOOSChomp(line, "="));
    string value = line;
    MOOSTrimWhiteSpace(param);
    MOOSTrimWhiteSpace(value);


    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "param =  " << param << endl;

    if(param == "TERM_REPORT_INTERVAL") {
      if(!MOOSIsNumeric(value))
	reportConfigWarning("Invalid TERM_REPORT_INTERVAL: " + value);
      else {
	m_term_report_interval = atof(value.c_str());
	if(m_term_report_interval < 0)
	  m_term_report_interval = 0;
	if(m_term_report_interval > 10)
	  m_term_report_interval = 10;
      }
    }
    else if(param == "MAX_APPCAST_EVENTS") {
      if(!MOOSIsNumeric(value))
	reportConfigWarning("Invalid MAX_APPCAST_EVENTS: " + value);
      else {
	int max_events = atoi(value.c_str());
	max_events = (max_events < 0)  ?  0 : max_events;
	max_events = (max_events > 50) ? 50 : max_events;
	m_ac.setMaxEvents((unsigned int)(max_events));
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "max events " << max_events << endl;
	cout << "max events " << m_ac.getMaxEvents() << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;      
      }
    }
    else if(param == "MAX_APPCAST_RUN_WARNINGS") {
      if(!MOOSIsNumeric(value))
	reportConfigWarning("Invalid MAX_APPCAST_EVENTS: " + value);
      else {
	int max_run_warnings = atoi(value.c_str());
	max_run_warnings = (max_run_warnings < 0) ? 0 : max_run_warnings;
	max_run_warnings = (max_run_warnings > 1000) ? 100 : max_run_warnings;
	m_ac.setMaxRunWarnings((unsigned int)(max_run_warnings));

      }
    }
  }    

  // #5 Initialize the AppCast Instance
  m_ac.setNodeName(m_host_community);
  m_ac.setProcName(GetAppName());

  // #6 Set key member variables base on basic MOOS calls
  m_curr_time  = MOOSTime();
  m_start_time = MOOSTime();
  m_time_warp  = GetMOOSTimeWarp();
  
  // #7 A negative or zero time warp is insane. Report this as a warning
  // and set to something that won't produce a NaN when dividing by 
  // time_warp to convert from moos_elapsed_time to real_elapsed_time.
  if(m_time_warp <= 0) {
    stringstream ss;
    ss << "MOOSTimeWarp invalid: " << m_time_warp << ". Using 0.001 instead.";
    reportConfigWarning(ss.str());
    m_time_warp = 0.001;
    return_value = false;
  }

  // Use isatty to detect if stdout is going to /dev/null/
  // If so, set m_term_reporting to false.
  if(!MOOSStrCmp(term_reporting, "true") && (isatty(1) == 0))
    m_term_reporting = false;

  return(return_value);
}

//----------------------------------------------------------------
// Procedure: RegisterVariables

void AppCastingMOOSApp::RegisterVariables()
{
  m_Comms.Register("APPCAST_REQ", 0);
  m_Comms.Register("TERM_REPORT_INTERVAL", 0);
  m_Comms.Register("COMMS_POLICY", 0);
}

//----------------------------------------------------------------
// Procedure: OnNewMail()

bool AppCastingMOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_curr_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end();) {
    CMOOSMsg &msg = *p;
    if(msg.GetKey() == "APPCAST_REQ") {
      handleMailAppCastRequest(msg.GetString());
      p = NewMail.erase(p);
    }
    else if(msg.GetKey() == "COMMS_POLICY") {
      string sval = msg.GetString();
      MOOSToLower(sval);
      bool ok = handleMailCommsPolicy(sval);
      if(!ok)
	reportRunWarning("Unhandled COMMS_POLICY:" + sval);
      
      p = NewMail.erase(p);
    }
    else if(msg.GetKey() == "TERM_REPORT_INTERVAL") {
      double dval = msg.GetDouble();
      m_term_report_interval = dval;
      if(m_term_report_interval < 0.4)
	m_term_report_interval = 0.4;
      p = NewMail.erase(p);
    }
    else if(msg.GetKey() == "_async_timing")
      p = NewMail.erase(p);
    else
      ++p;
  }
  return(true);
}

//----------------------------------------------------------------
// Procedure: handleMailAppCastRequest
//   Example: str = node=henry,         (name of this community)
//                  app=pHostInfo,      (name of this app)
//                  duration=10,        (lifespan of the request)
//                  key=uMAC_438,       (name of client requesting)
//                  thresh=any          (threshold for AC generation)

void AppCastingMOOSApp::handleMailAppCastRequest(const string& str)
{
  string s_key;
  string s_duration;
  string s_thresh = "any";

  string request = str;
  while(request != "") {
    string pair = MOOSChomp(request, ",");
    string param = MOOSToUpper(MOOSChomp(pair, "="));
    string value = pair;
    MOOSTrimWhiteSpace(param);
    MOOSTrimWhiteSpace(value);

    if(param == "NODE") {
      if((value != m_host_community) && (value != "all"))
	return;
    }
    else if(param == "APP") {
      if((value != GetAppName()) && (value != "all"))
	return;
    }
    else if(param == "DURATION")
      s_duration = value;
    else if(param == "THRESH")
      s_thresh = value;
    else if(param == "KEY") {
      s_key = value;
    }
  }

  if(s_key == "")
    return;

  double d_duration = atof(s_duration.c_str());
  d_duration = (d_duration < 0) ? 0 : d_duration;
  d_duration = (d_duration > 30) ? 30 : d_duration;

  m_map_bcast_duration[s_key] = d_duration;
  m_map_bcast_tstart[s_key]   = m_curr_time;
  m_map_bcast_thresh[s_key]   = s_thresh;
}

//----------------------------------------------------------------
// Procedure: appcastRequested
//      Note: Check all possible subscribers (keys) to see if any one of 
//            them has an appcast request that hasn't expired. All it 
//            takes is one, and the appcast is thereby warranted.

bool AppCastingMOOSApp::appcastRequested()
{
  bool requested = false;

  map<string,double>::iterator p;
  for(p=m_map_bcast_duration.begin(); p!=m_map_bcast_duration.end(); ++p) {
    string key      = p->first;
    double duration = p->second;
    double elapsed  = m_curr_time - m_map_bcast_tstart[key];

    // Found an un-expired subscription for appcasts from a client.
    if(elapsed < duration) {
      if(m_map_bcast_thresh[key] == "any")
	requested = true;
      else if((m_map_bcast_thresh[key] == "run_warning") && m_new_run_warning)
	requested = true;
    }
  }
  if(m_new_cfg_warning)
    requested = true;

  // reset the new_run_warning and new_cfg_warning indicators here.
  return(requested);
}

//----------------------------------------------------------------
// Procedure: reportEvent

void AppCastingMOOSApp::reportEvent(const string& str)
{
  double timestamp = m_curr_time - m_start_time;
  m_ac.event(str, timestamp);
}

//----------------------------------------------------------------
// Procedure: reportConfigWarning

void AppCastingMOOSApp::reportConfigWarning(const string& str)
{
  if(str == "")
    return;
  
  m_new_cfg_warning = true;
  m_ac.cfgWarning(str);
}

//----------------------------------------------------------------
// Procedure: reportUnhandledConfigWarning

void AppCastingMOOSApp::reportUnhandledConfigWarning(const string& orig)
{
  if(orig == "")
    return;
  
  string orig_copy = orig;
  string param = MOOSToUpper(MOOSChomp(orig_copy, "="));
  MOOSTrimWhiteSpace(param);
  if((param == "APPTICK")    || (param == "APP_LOGGING")          ||
     (param == "MAXAPPTICK") || (param == "TERM_REPORT_INTERVAL") ||
     (param == "COMMSTICK")  || (param == "MAX_APPCAST_EVENTS")   ||
     (param == "DEPRECATED_OK"))
    return;

  reportConfigWarning("Unhandled config line: " + orig);
}

//----------------------------------------------------------------
// Procedure: reportRunWarning

bool AppCastingMOOSApp::reportRunWarning(const string& str)
{
  if(str == "")
    return(false);
  m_new_run_warning = true;
  m_ac.runWarning(str);
  return(false);
}

//----------------------------------------------------------------
// Procedure: retractRunWarning

void AppCastingMOOSApp::retractRunWarning(const string& str)
{
  if(m_ac.getRunWarningCount() == 0)
    return;

  // Retraction will only succeed if the retraction message exactly 
  // matches a previously posted warning.
  bool successfully_retracted = m_ac.retractRunWarning(str);

  // If successful, treat as if this is a new warning so that updates
  // to the user may be made.
  if(successfully_retracted)
    m_new_run_warning = true;
}

//----------------------------------------------------------------
// Procedure: getWarningCount()
//      Note: Acceptable arguments: "config", "run", "all"

unsigned int AppCastingMOOSApp::getWarningCount(const string& filter) const
{
  unsigned int total = 0;
  if((filter == "all") || (filter == "config"))
    total += m_ac.getCfgWarningCount();

  if((filter == "all") || (filter == "run"))
    total += m_ac.getRunWarningCount();

  return(total);
}

//----------------------------------------------------------------
// Procedure: handleMailCommsPolicy()

bool AppCastingMOOSApp::handleMailCommsPolicy(const string& str)
{
  // Sanity check valid policy
  if((str != "open") && (str != "lean") && (str != "dire"))
    return(false);

  // Cannot have open policy if original param config is stricter
  if(str == "open") {
    if((m_comms_policy_config == "lean") ||
       (m_comms_policy_config == "dire"))
	return(true);
  }
  
  // Cannot have lean policy if original param config is dire
  if(str == "lean") {
    if(m_comms_policy_config == "dire")
      return(false);
  }

  m_comms_policy = str;
  
  return(true);
}

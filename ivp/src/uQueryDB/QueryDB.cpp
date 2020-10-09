/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: QueryDB.cpp                                          */
/*    DATE: Dec 29th 2015                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdio>
#include "QueryDB.h"
#include "MBUtils.h"

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Constructor

QueryDB::QueryDB()
{
  // Init State Vars
  m_info_buffer  = new InfoBuffer;
  m_exit_value   = -1;
  m_elapsed_time = 0;
  
  // Init Config Vars
  m_wait_time   = 10;
  m_verbose     = true;
  m_sServerHost = ""; 
  m_lServerPort = 0;
  m_max_time    = -1;

  m_check_for_halt = true; // By default checking halt conditions
  m_report_check_vars = false;
  
  m_configure_comms_locally = false;
}

//------------------------------------------------------------
// Procedure: setWaitTime()

bool QueryDB::setWaitTime(string str)
{
  return(setNonNegDoubleOnString(m_wait_time, str));
}

//------------------------------------------------------------
// Procedure: setServerPort()

bool QueryDB::setServerPort(string str)
{
  if(!isNumber(str))
    return(false);

  m_sServerPort = atoi(str.c_str());
  return(true);
}

//------------------------------------------------------------
// Procedure: setMissionFile()

bool QueryDB::setMissionFile(string file_str)
{
  // Sanity Check: If mission file previously set, return false
  if(m_mission_file != "")
    return(false);
  
  if(!okFileToRead(file_str))
    return(false);

  m_mission_file = file_str;
  return(true);
}

//------------------------------------------------------------
// Procedure: addHaltCondition()

bool QueryDB::addHaltCondition(string str)
{
  return(m_halt_conditions.addNewCondition(str));
}


//------------------------------------------------------------
// Procedure: Iterate()

bool QueryDB::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_exit_value >= 0) {
    reportCheckVars();
    exit(m_exit_value);
  }

  if(m_check_for_halt) {
    checkMaxTimeReached();
    checkHaltConditions();
  }
  else
    checkPassFailConditions();

  reportRunWarning("bogus");
  retractRunWarning("bogus");

  if(m_verbose) 
    AppCastingMOOSApp::PostReport();  

  return(true);
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool QueryDB::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
 
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval   = msg.GetDouble();
    string sval   = msg.GetString(); 

    if(key == "DB_UPTIME")
      m_elapsed_time = dval;

    if(msg.IsDouble()) {
      m_halt_conditions.updateInfoBuffer(key, dval);
      m_pass_conditions.updateInfoBuffer(key, dval);
      m_fail_conditions.updateInfoBuffer(key, dval);
    }
    else if(msg.IsString()) {
      m_halt_conditions.updateInfoBuffer(key, sval);
      m_pass_conditions.updateInfoBuffer(key, sval);
      m_fail_conditions.updateInfoBuffer(key, sval);
    }
    updateInfoBuffer(msg);
    
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp()

bool QueryDB::OnStartUp()
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

    bool handled = true;
    if((param == "condition") || (param == "halt_condition"))
      handled = m_halt_conditions.addNewCondition(value);
    else if((param == "max_time") || (param == "halt_max_time"))
      handled = setDoubleOnString(m_max_time, value);

    else if(param == "pass_condition")
      handled = m_pass_conditions.addNewCondition(value);
    else if(param == "fail_condition")
      handled = m_fail_conditions.addNewCondition(value);
    else if((param == "check_var") && !strContainsWhite(value)) {
      if(!vectorContains(m_check_vars, value))
	m_check_vars.push_back(value);
      m_report_check_vars = true;
    }
    else
      handled = false;
      
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // Fake an appcast request
  string req = "node=" + m_host_community;
  req += ", app=" + GetAppName();
  req += ", duration=" + doubleToStringX(m_wait_time+30);
  req += ", key=uQueryDB,thresh=any";
  Notify("APPCAST_REQ", req);

  // Heuristic: Always add DB_UPTIME to the list of check_vars.
  // If other check_vars are being used, this will also be
  // reported. Especially important report var for bookkeeping
  if(!vectorContains(m_check_vars, "DB_UPTIME"))
    m_check_vars.push_back("DB_UPTIME");
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool QueryDB::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void QueryDB::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  
  vector<string> vars = m_halt_conditions.getAllVars();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);

  vars = m_pass_conditions.getAllVars();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);
  
  vars = m_fail_conditions.getAllVars();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);
  
  for(unsigned int i=0; i<m_check_vars.size(); i++) 
    Register(m_check_vars[i], 0);
  
  Register("DB_UPTIME", 0);
  Register("DB_TIME", 0);
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool QueryDB::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.GetKey();
  string sdata = msg.GetString();
  double ddata = msg.GetDouble();

  if(msg.IsDouble())
    return(m_info_buffer->setValue(key, ddata));
  else if(msg.IsString()) 
    return(m_info_buffer->setValue(key, sdata));
  return(false);
}

//------------------------------------------------------------
// Procedure: ConfigureComms
//      Note: Overload the MOOSApp::ConfigureComms implementation
//            which would have grabbed the port/host info from the
//            .moos file instead.

bool QueryDB::ConfigureComms()
{
  //cout << "QueryDB::ConfigureComms:" << endl;
  //cout << "  m_sServerHost: " << M_Sserverhost << endl;
  //cout << "  m_lServErport: " << m_lServerPort << endl;

  if(!m_configure_comms_locally) 
    return(CMOOSApp::ConfigureComms());

  //register a callback for On Connect
  m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect, this);
  
  //and one for the disconnect callback
  m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect, this);
  
  //start the comms client....
  if(m_sMOOSName.empty())
    m_sMOOSName = m_sAppName;
  
  m_nCommsFreq = 10;

  m_Comms.Run(m_sServerHost.c_str(), 
	      m_lServerPort,
	      m_sMOOSName.c_str(), 
	      m_nCommsFreq);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: reportCheckVars()
//   Purpose: If check_vars are specified, summarize these
//            vars to stdout upon exit. The intention is that
//            these lines may be picked up by a shell script.

void QueryDB::reportCheckVars()
{
  vector<string> svector = m_info_buffer->getReport(m_check_vars);
  if(svector.size() == 0)
    return;
  
  FILE *f = fopen(".checkvars", "w");
  if(!f)
    return;
  
  for(unsigned int i=0; i<svector.size(); i++) {
    string line = stripBlankEnds(svector[i]);
    string var = biteStringX(line, ' ');
    string val = line;
    fprintf(f, "checkvar = %s=%s\n", var.c_str(), val.c_str());
  }

  fclose(f);
}


//-----------------------------------------------------------
// Procedure: checkMaxTimeReached()
//      Sets: m_exit_value
//            0 if enabled and time has been reached
//            1 if disabled or time has not been reached

void QueryDB::checkMaxTimeReached()
{
  // Sanity check: if max_time not enabled, do nothing
  if(m_max_time < 0)
    return;

  // Sanity check: if exit value has been set, it cannot change
  if(m_exit_value != -1)
    return;
  
  if(m_elapsed_time < m_max_time)
    return;
  
  m_exit_value = 0;
}


//-----------------------------------------------------------
// Procedure: checkHaltConditions()
//      Sets: m_exit_value
//            1  if not all mail has been received yet
//               for all variables involved in condition
//            1  Logic condition is false
//            0  Logic condition is true

void QueryDB::checkHaltConditions()
{
  bool satisfied = m_halt_conditions.checkConditions();
  if(satisfied)
    m_exit_value = 0;
  else
    m_exit_value = 1;
}

//-----------------------------------------------------------
// Procedure: checkPassFailConditions()
//      Sets: m_exit_value
//            1  if not all mail has been received yet for all
//               vars involved in either pass or fail conditions
//            1  A pass condition unsat
//            1  Not all fail conditions unsat
//            0  otherwise (THIS MEANS PASS)

void QueryDB::checkPassFailConditions()
{
  bool all_pass_conds_met = m_pass_conditions.checkConditions("all");
  bool any_fail_conds_met = m_fail_conditions.checkConditions("any");

  if(!all_pass_conds_met) {
    m_exit_value = 1;
    m_notable_condition = m_pass_conditions.getNotableCondition();
  }
  else if(any_fail_conds_met) {
    m_exit_value = 1;
    m_notable_condition = m_fail_conditions.getNotableCondition();
  }
  else
    m_exit_value = 0;
}


//------------------------------------------------------------
// Procedure: buildReport()

bool QueryDB::buildReport() 
{
  // =======================================================
  // Report Style 1: When in halt condition mode
  // =======================================================
  if(m_check_for_halt) {
    string max_time_str = "n/a";
    if(m_max_time > 0)
      max_time_str = doubleToStringX(m_max_time,2);

    string hconds_count_str = uintToString(m_halt_conditions.size());
    string unmet_logic_str = m_halt_conditions.getNotableCondition();
    if(unmet_logic_str == "")
      unmet_logic_str = "n/a";
    
    m_msgs << "Config (halting):  " << endl;
    m_msgs << "  max_time:        " << max_time_str      << endl;
    m_msgs << "  halt_conditions: " << hconds_count_str  << endl;
    m_msgs << "  unmet condition: " << unmet_logic_str   << endl;
    m_msgs << endl;
    
    if(m_max_time > 0) {
      string str_elapsed = doubleToString(m_elapsed_time,1);
      m_msgs << "Elapsed Time: " << str_elapsed << endl;
    }

    m_msgs << endl;
    m_msgs << "InfoBuffer: (halt condition vars)            " << endl;
    m_msgs << "============================================ " << endl;
    vector<string> ibh_report = m_halt_conditions.getInfoBuffReport(true);
    if(ibh_report.size() == 0)
      m_msgs << "<empty>" << endl;
    for(unsigned int i=0; i<ibh_report.size(); i++)
      m_msgs << ibh_report[i] << endl;
    
  }
  // =======================================================
  // Report Style 2: When in pass/fail condition mode
  // =======================================================
  else {
    string pconds_count_str = uintToString(m_pass_conditions.size());
    string fconds_count_str = uintToString(m_fail_conditions.size());
    string pass_fail_result = "fail: " + m_notable_condition;
    if(m_exit_value == 0)
      pass_fail_result = "pass";
    
    m_msgs << "Config (pass/fail):" << endl;
    m_msgs << "  pass_conditions: " << pconds_count_str  << endl;
    m_msgs << "  fail_conditions: " << fconds_count_str  << endl;
    m_msgs << "Result: " << pass_fail_result << endl;
    m_msgs << endl;
    
    m_msgs << endl;
    m_msgs << "InfoBuffer: (pass condition vars)            " << endl;
    m_msgs << "============================================ " << endl;
    vector<string> ibp_report = m_pass_conditions.getInfoBuffReport(true);
    if(ibp_report.size() == 0)
      m_msgs << "<empty>" << endl;
    for(unsigned int i=0; i<ibp_report.size(); i++)
      m_msgs << ibp_report[i] << endl;
    
    m_msgs << endl;
    m_msgs << "InfoBuffer: (fail condition vars)            " << endl;
    m_msgs << "============================================ " << endl;
    vector<string> ibf_report = m_fail_conditions.getInfoBuffReport(true);
    if(ibf_report.size() == 0)
      m_msgs << "<empty>" << endl;
    for(unsigned int i=0; i<ibf_report.size(); i++)
      m_msgs << ibf_report[i] << endl;

    m_msgs << endl;
    m_msgs << "InfoBuffer: (check vars)                     " << endl;
    m_msgs << "============================================ " << endl;
    vector<string> var_report = m_info_buffer->getReport(m_check_vars);
    if(var_report.size() == 0)
      m_msgs << "<empty>" << endl;
    for(unsigned int i=0; i<var_report.size(); i++)
      m_msgs << var_report[i] << endl;
  }
  
  return(true);
}



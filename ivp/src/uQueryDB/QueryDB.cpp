/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
// Constructor()

QueryDB::QueryDB()
{
  // Init State Vars
  m_info_buffer  = new InfoBuffer;
  m_exit_value   = -1;
  m_elapsed_time = 0;

  // Init Config Vars
  m_sServerHost = ""; 
  m_lServerPort = 0;
  m_max_time    = 0;

  m_check_var_format = "esv";
  m_configure_comms_locally = false;

  m_pass_conditions.setInfoBuffer(m_info_buffer);
  m_fail_conditions.setInfoBuffer(m_info_buffer);
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
// Procedure: addPassCondition()

bool QueryDB::addPassCondition(string str)
{
  bool handled = m_pass_conditions.addNewCondition(str);

  return(handled);
}


//------------------------------------------------------------
// Procedure: addFailCondition()

bool QueryDB::addFailCondition(string str)
{
  bool handled = m_fail_conditions.addNewCondition(str);

  return(handled);
}

//------------------------------------------------------------
// Procedure: setConfigWaitTime()

bool QueryDB::setConfigWaitTime(string str)
{
  return(setNonNegDoubleOnString(m_max_time, str));
}

//------------------------------------------------------------
// Procedure: addConfigCheckVar()

bool QueryDB::addConfigCheckVar(string str)
{
  if(strContainsWhite(str))
    return(false);
  
  if(vectorContains(m_check_vars, str))
    return(false);

  m_check_vars.push_back(str);

  return(true);
}

//------------------------------------------------------------
// Procedure: setConfigCheckVarFormat()
//     Notes: csv is comma-separated-value
//            esv is equals-separated-value
//            wsv is whitespace-separated-value
//            vo is value only

bool QueryDB::setConfigCheckVarFormat(string str)
{
  str = tolower(str);
  if((str != "esv") && (str != "csv") && (str != "wsv") && (str != "vo"))
    return(false);

  m_check_var_format = str;

  return(true);
}

//------------------------------------------------------------
// Procedure: Iterate()

bool QueryDB::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Part 1: Handle exiting if exiting is warranted  
  if(m_exit_value >= 0) {
    reportCheckVars();
    exit(m_exit_value);
  }

  // Part 2: Re-evaluate exit conditions AFTER the above exit handling
  // to allow a PostReport to reflect the reason for exit.
  m_elapsed_time = m_curr_time - m_start_time;
  if((m_max_time > 0) && (m_elapsed_time >= m_max_time))
    m_exit_value = 1;
  
  checkPassFailConditions();

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

    if(msg.IsDouble()) {
      m_pass_conditions.updateInfoBuffer(key, dval);
      m_fail_conditions.updateInfoBuffer(key, dval);
    }
    else if(msg.IsString()) {
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
  string directives = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  //m_MissionReader.GetConfiguration(GetAppName(), sParams);
  m_MissionReader.GetConfiguration("uQueryDB", sParams);

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    bool handled = true;
    if((param == "wait") || (param == "halt_max_time"))
      handled = setDoubleOnString(m_max_time, value);

    else if(param == "condition")
      handled = m_pass_conditions.addNewCondition(value);
    else if(param == "pass_condition")
      handled = m_pass_conditions.addNewCondition(value);
    else if(param == "fail_condition")
      handled = m_fail_conditions.addNewCondition(value);
    else if(param == "check_var")
      handled = addConfigCheckVar(value);
    else if(param == "check_var_format")
      handled = setConfigCheckVarFormat(value);
    else
      handled = false;
      
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
    
  m_ac.setProcName("uQueryDB");

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
  
  vector<string> vars = m_pass_conditions.getAllVars();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);
  
  vars = m_fail_conditions.getAllVars();
  for(unsigned int i=0; i<vars.size(); i++) 
    Register(vars[i], 0);
  
  for(unsigned int i=0; i<m_check_vars.size(); i++) 
    Register(m_check_vars[i], 0);
  
  //Register("DB_UPTIME", 0);
  //Register("DB_TIME", 0);
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

  m_Comms.Run(m_sServerHost.c_str(), m_lServerPort,
	      m_sMOOSName.c_str(), m_nCommsFreq);
  
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
    if(m_check_var_format == "wsv")
      fprintf(f, "%s %s\n", var.c_str(), val.c_str());
    else if(m_check_var_format == "csv")
      fprintf(f, "%s,%s\n", var.c_str(), val.c_str());
    else if(m_check_var_format == "esv")
      fprintf(f, " %s=%s\n", var.c_str(), val.c_str());
    else
      fprintf(f, "%s\n", val.c_str());
  }

  fclose(f);
}


//-----------------------------------------------------------
// Procedure: checkPassFailConditions()
//      Sets: m_exit_value
//            1  if not all mail has been received yet for all
//               vars involved in either pass or fail conditions
//            1  A pass condition unsat
//            1  Any fail conditions satisfied
//            0  otherwise (THIS MEANS PASS)
//      Note: The xlaunch.sh utility script, which uses uQueryDB,
//            looks for a return value of 0 (pass). Upon 0, it
//            will halt the mission. 

void QueryDB::checkPassFailConditions()
{
  bool all_pass_conds_met = m_pass_conditions.checkConditions("all");
  bool any_fail_conds_met = m_fail_conditions.checkConditions("any");

  if(!all_pass_conds_met)
    m_notable_condition = m_pass_conditions.getNotableCondition();
  else if(any_fail_conds_met)
    m_notable_condition = m_fail_conditions.getNotableCondition();
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
  string maxtime_str = "n/a";
  string elapsed_str  = "n/a";
  if(m_max_time > 0) {
    elapsed_str = doubleToString(m_elapsed_time,1);
    maxtime_str = doubleToString(m_max_time, 1);
  }
  
  m_msgs << "Config:" << endl;
  m_msgs << "  m_sServerHost: " << m_sServerHost << endl;
  m_msgs << "  m_lServErport: " << m_lServerPort << endl;
  m_msgs << "  Max Time:      " << maxtime_str << endl;
  m_msgs << "State:           " << endl;
  m_msgs << "  Start Time:    " << doubleToStringX(m_start_time) << endl;
  m_msgs << "  Curr Time:     " << doubleToStringX(m_curr_time) << endl;
  m_msgs << "  Elapsed Time:  " << elapsed_str  << endl;
  m_msgs << "  Exit Value:    " << m_exit_value << endl;

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

  return(true);
}




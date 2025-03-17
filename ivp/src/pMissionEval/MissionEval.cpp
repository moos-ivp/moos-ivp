/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MissionEval.cpp                                      */
/*    DATE: Sep 19th, 2020                                       */
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

#include "MBUtils.h"
#include "ACTable.h"
#include "MissionEval.h"
#include "VarDataPairUtils.h"
#include "MacroUtils.h"
#include "HashUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MissionEval::MissionEval()
{
  m_result_flags_posted = false;
  m_prereport_posted = false;
  
  m_info_buffer = new InfoBuffer;
  m_logic_tests.setInfoBuffer(m_info_buffer);

  m_db_uptime = 0;
  m_mhash_utc = 0;

  m_mission_result = "un-started";
  m_mission_form   = "mission";
  m_report_line_format = "white";
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MissionEval::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    double dval  = msg.GetDouble();
    double mtime = msg.GetTime();

    reportEvent("Mail:" + key);
    
    // Consider mail handled by default if handled by mail_flag_set
    m_mfset.handleMail(key, m_curr_time);

    // Pass mail to InfoBuffer
    if(msg.IsDouble())
      m_info_buffer->setValue(key, dval, mtime);
    else if(msg.IsString())
      m_info_buffer->setValue(key, sval, mtime);
      
    if(key == "MISSION_HASH")
      handleMailMissionHash(sval, mtime);

    else if(key == "DB_UPTIME")
      m_db_uptime = dval;
  }

  // After MailFlagSet has handled all mail from this iteration,
  // post any flags that result. Flags will be cleared in m_mfset.
  postFlags(m_mfset.getNewFlags());
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()
  
bool MissionEval::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool MissionEval::Iterate()
{
  AppCastingMOOSApp::Iterate();

  m_info_buffer->setCurrTime(m_curr_time);
  
  if(!m_prereport_posted) {
    if(m_mission_hash != "") {
      postPreResults();
      m_prereport_posted = true;
    }
  }
  
  m_logic_tests.update();

  // If both logic_aspect evaluated, post results
  if(m_logic_tests.isEvaluated()) {
    if(!m_prereport_posted) {
      postPreResults();
      m_prereport_posted = true;
    }
    postResults();
  }
  
  string aspect_status = m_logic_tests.getStatus();
  if(aspect_status != m_logic_tests_status_prev) {
    Notify("MISSION_LCHECK_STAT", aspect_status);
    m_logic_tests_status_prev = aspect_status;
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MissionEval::OnStartUp()
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
    if(param == "lead_condition") 
      handled = m_logic_tests.addLeadCondition(value);
    else if(param == "pass_condition") 
      handled = m_logic_tests.addPassCondition(value);
    else if(param == "fail_condition") 
      handled = m_logic_tests.addFailCondition(value);
    
    else if(param == "report_column")
      m_report_columns.push_back(value);
    else if(param == "prereport_column")
      m_prereport_columns.push_back(value);
    else if(param == "report_file") {
      handled = okFileToWrite(value);
      m_report_file = value;
    }


    else if(param == "result_flag")
      handled = addVarDataPairOnString(m_result_flags, value);
    else if(param == "pass_flag")
      handled = addVarDataPairOnString(m_pass_flags, value);
    else if(param == "fail_flag")
      handled = addVarDataPairOnString(m_fail_flags, value);

    else if(param == "mission_form")
      handled = setNonWhiteVarOnString(m_mission_form, value);
    else if(param == "mission_mod")
      handled = setNonWhiteVarOnString(m_mission_mod, value);
    
    else if(param == "report_line_format")
      handled = handleConfigColumnFormat(value);
    
    else if(param == "mailflag") 
      handled = m_mfset.addFlag(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  //===========================================================
  // Check proper configuration
  //===========================================================
  // Part 1: Check LCheck proper config
  string aspect_warning = m_logic_tests.checkConfig();
  if(aspect_warning != "")
    reportConfigWarning(aspect_warning);

  // Part 3: MUST be using either logic-based or time-based checks!
  if(!m_logic_tests.enabled()) 
    reportConfigWarning("MUST provide either logic based checks");
  
  m_mission_result = "pending";
  Notify("MISSION_RESULT", "pending");

  findMacroVars();
  
  registerVariables();	

  m_info_buffer->setValue("MISSION_FORM", m_mission_form);
  
  vector<string> spec = m_logic_tests.getSpec();
  for(unsigned int i=0; i<spec.size(); i++)
    cout << spec[i] << endl;

  return(true);
}

    
//---------------------------------------------------------
// Procedure: handleMailMissionHash()

void MissionEval::handleMailMissionHash(string sval, double msg_time)
{
  m_mission_hash = sval;

  // Values stored both in local member variables
  m_mhash_short  = missionHashShort(m_mission_hash);
  m_mhash_utc    = missionHashUTC(m_mission_hash);

  // And stored in the info_buffer
  m_info_buffer->setValue("MHASH_SHORT", m_mhash_short, msg_time);
  m_info_buffer->setValue("MHASH_UTC", m_mhash_utc, msg_time);
}

//---------------------------------------------------------
// Procedure: handleConfiguColumnFormat()

bool MissionEval::handleConfigColumnFormat(string sval)
{
  sval = tolower(sval);
  if((sval != "csp") && (sval != "white"))
    return(false);
  
  m_report_line_format = sval;
  return(true);
}

//---------------------------------------------------------
// Procedure: findMacroVars()
//   Purpose: o Look in the result, pass and fail flags, for macros
//              like result_flag = SCORE=$[CLOSE_ENCOUNTERS]. 
//            o The macro CLOSE_ENCOUNTERS will be regarded as a 
//              variable to register for. When the flag is posted, 
//              the latest value of CLOSE_ENCOUNTERS (e.g. 8), will
//              be published: SCORE=8

void MissionEval::findMacroVars()
{
  for(unsigned int i=0; i<m_result_flags.size(); i++) {
    set<string> macro_set = m_result_flags[i].getMacroSet();
    m_macro_vars.insert(macro_set.begin(), macro_set.end());
  }
  for(unsigned int i=0; i<m_pass_flags.size(); i++) {
    set<string> macro_set = m_pass_flags[i].getMacroSet();
    m_macro_vars.insert(macro_set.begin(), macro_set.end());
  }
  for(unsigned int i=0; i<m_fail_flags.size(); i++) {
    set<string> macro_set = m_fail_flags[i].getMacroSet();
    m_macro_vars.insert(macro_set.begin(), macro_set.end());
  }

  // Discover macros in the report line
  for(unsigned int i=0; i<m_report_columns.size(); i++) {
    string column = m_report_columns[i];
    vector<string> report_vars = getMacrosFromString(column);
    for(unsigned int j=0; j<report_vars.size(); j++) {
      m_macro_vars.insert(report_vars[j]);
    }
  }

  // Discover macros in the report line
  for(unsigned int i=0; i<m_prereport_columns.size(); i++) {
    string column = m_prereport_columns[i];
    vector<string> report_vars = getMacrosFromString(column);
    for(unsigned int j=0; j<report_vars.size(); j++) {
      m_macro_vars.insert(report_vars[j]);
    }
  }
}


//---------------------------------------------------------
// Procedure: registerVariables()

void MissionEval::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  // Register for variables used in logic_aspect
  set<string> aspect_vars = m_logic_tests.getLogicVars();

  // Now register for all unique vars
  set<string>::iterator p;
  for(p=aspect_vars.begin(); p!=aspect_vars.end(); p++) {
    string moos_var = *p;
    Register(moos_var, 0);
  }

  // Now register for all flag macros
  for(p=m_macro_vars.begin(); p!=m_macro_vars.end(); p++) {
    string var = *p;

    // Some vars available as macros are "internal", they are not MOOS
    // variables and we don't want to needless register for them.
    if((var == "MHASH_SHORT")  || (var == "MHASH_UTC") ||
       (var == "MISSION_FORM") || (var == "WALL_TIME"))
      continue;
    
    Register(var, 0);
  }

  // Register for any variables involved in the MailFlagSet
  vector<string> mflag_vars = m_mfset.getMailFlagKeys();
  for(unsigned int i=0; i<mflag_vars.size(); i++)
    Register(mflag_vars[i], 0);

  Register("MISSION_HASH");  
  Register("DB_UPTIME");  
}


//------------------------------------------------------------
// Procedure: postResults()

void MissionEval::postResults()
{
  if(m_result_flags_posted)
    return;
  m_result_flags_posted = true;

  // =========================================================
  // Part 1: No matter the results, once the eval conditions 
  // have been evaluated, the result_flags are posted, if any
  postFlags(m_result_flags);
  Notify("MISSION_EVALUATED", "true");
  

  // =========================================================
  // Part 2: If eval conditions met, post pass_flags, otherwise
  // fail_flags
  if(m_logic_tests.isSatisfied()) {
    m_mission_result = "pass";
    Notify("MISSION_RESULT", "pass");
    postFlags(m_pass_flags);
  }
  else {
    m_mission_result = "fail";
    Notify("MISSION_RESULT", "fail");
    postFlags(m_fail_flags);
  }

  // =========================================================
  // Part 3: Post report_line to a file if configured
  if(m_report_file == "")
    return;

  FILE *f = fopen(m_report_file.c_str(), "a");
  if(!f) {
    reportRunWarning("Unable to write to file: " + m_report_file);
    return;
  }

  string line;
  for(unsigned int i=0; i<m_report_columns.size(); i++) {
    string column = expandMacros(m_report_columns[i]);
    if(i != 0) {
      if(m_report_line_format == "csp")
	line += ", ";
      else
	line += "  ";
    }
    line += column;
  }
  fprintf(f, "%s\n", line.c_str());
  fclose(f);
  m_report_latest_line = line;
  

#if 0
  for(unsigned int i=0; i<m_report_columns.size(); i++) {
    string column = expandMacros(m_report_columns[i]);
    if(i != 0) {
      column = "  " + column;
      fprintf(f, "%s", column.c_str());
  }
  fprintf(f, "\n");
  fclose(f);
#endif
}


//------------------------------------------------------------
// Procedure: postPreResults()

void MissionEval::postPreResults()
{
  if(m_report_file == "")
    return;

  FILE *f = fopen(m_report_file.c_str(), "a");
  if(!f) {
    reportRunWarning("Unable to write to file: " + m_report_file);
    return;
  }

  string line;
  for(unsigned int i=0; i<m_prereport_columns.size(); i++) {
    string column = expandMacros(m_prereport_columns[i]);
    if(i != 0) {
      if(m_report_line_format == "csp")
	line += ", ";
      else
	line += "  ";
    }
    line += column;
  }
  fprintf(f, "%s  ", line.c_str());
  fclose(f);
  m_report_latest_line = line;
}


//------------------------------------------------------------
// Procedure: postFlags()
//   Purpose: For any set of flags, post each. For flags that
//            contain a macro, expand the macro first.

void MissionEval::postFlags(const vector<VarDataPair>& flags)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();
    string postval;

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      postval = doubleToStringX(dval,4);
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      postval = stripBlankEnds(pair.get_sdata());
      postval = expandMacros(postval);
      if(postval != "")
	Notify(moosvar, postval);
    }
    reportEvent(moosvar + "=" + postval);
  }
}


//-----------------------------------------------------------
// Procedure: expandMacros()

string MissionEval::expandMacros(string sdata) const
{
  if(strContains(sdata, "$[MHASH_SHORT]")) {
    string mhash_short = "[" + missionHashShort(m_mission_hash) + "]";
    sdata = macroExpand(sdata, "MHASH_SHORT", mhash_short);
  }
  
  if(strContains(sdata, "$[MHASH]"))
    sdata = macroExpand(sdata, "MHASH", m_mission_hash);
  
  if(strContains(sdata, "$[MISSION_HASH]"))
    sdata = macroExpand(sdata, "MISSION_HASH", m_mission_hash);
  
  if(strContains(sdata, "$[MHASH_LSHORT]")) {
    string mhash_lshort = missionHashLShort(m_mission_hash);
    sdata = macroExpand(sdata, "MHASH_LSHORT", mhash_lshort);
  }
  
  if(strContains(sdata, "$[MISSION_FORM]")) 
    sdata = macroExpand(sdata, "MISSION_FORM", m_mission_form);
  
  if(strContains(sdata, "$[MMOD]")) 
    sdata = macroExpand(sdata, "MMOD", m_mission_mod);
  
  if(strContains(sdata, "$[GRADE]")) 
    sdata = macroExpand(sdata, "GRADE", m_mission_result);
  
  if(strContains(sdata, "$[MHASH_UTC]")) {
    string mhash_utc = tokStringParse(m_mission_hash, "utc", ',', '=');
    sdata = macroExpand(sdata, "MHASH_UTC", mhash_utc);
  }
  
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
  
  if(strContains(sdata, "$[WALL_TIME]")) {
    if(m_time_warp > 0) {
      double wall_time = m_db_uptime / m_time_warp;
      string wall_str = doubleToString(wall_time,2); 
      sdata = macroExpand(sdata, "WALL_TIME", wall_str);
    }
  }

  set<string>::iterator p;
  for(p=m_macro_vars.begin(); p!=m_macro_vars.end(); p++) {
    string var = *p;
    bool ok;
    string sval = m_info_buffer->sQuery(var, ok);
    if(ok) 
      sdata = macroExpand(sdata, var, sval);
    else {
      double dval = m_info_buffer->dQuery(var, ok);
      if(ok) 
	sdata = macroExpand(sdata, var, dval);
    }
  }
  
  return(sdata);
}

  
//------------------------------------------------------------
// Procedure: buildReport()

bool MissionEval::buildReport() 
{
  // Part 1: Overview Info
  string rflag_count_str = uintToString(m_result_flags.size());
  string pflag_count_str = uintToString(m_pass_flags.size());
  string fflag_count_str = uintToString(m_fail_flags.size());  
  m_msgs << "Overall State: " << m_mission_result << endl;
  m_msgs << "============================================== " << endl;
  m_msgs << " logic_tests_stat: " << m_logic_tests.getStatus() << endl;
  m_msgs << "     result flags: " << rflag_count_str << endl;
  m_msgs << "       pass flags: " << pflag_count_str << endl;
  m_msgs << "       fail flags: " << fflag_count_str << endl;
  m_msgs << "       curr_index: " << m_logic_tests.currIndex() << endl;
  //  m_msgs << "      report_line: [" << m_report_line << "]" << endl;
  m_msgs << "      report_file: " << m_report_file << endl << endl;


  m_msgs << "==============================================" << endl;
  m_msgs << "Supported Macros: " << endl;
  m_msgs << "==============================================" << endl;
  m_msgs << endl;
  
  ACTable actab(2,2);
  actab << "Macro | CurrVal \n";
  actab.addHeaderLines();

  set<string>::iterator q;
  for(q=m_macro_vars.begin(); q!=m_macro_vars.end(); q++) {
    string var = *q;
    bool ok;
    string sval = m_info_buffer->sQuery(var, ok);
    double dval = m_info_buffer->dQuery(var, ok);
    if(sval == "")
      sval = doubleToStringX(dval,2);
    actab << var << sval;
  }
  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;
  

  m_msgs << "==============================================" << endl;
  m_msgs << "Logic Sequence: " << endl;
  m_msgs << "==============================================" << endl;
  m_msgs << endl;
  
  ACTable actab2(5,2);
  actab2 << "Index | Status | Conds | Pass Flags | Fail FLags \n";
  actab2.addHeaderLines();

  unsigned int currix = m_logic_tests.currIndex();
  unsigned int lsize = m_logic_tests.size();
  for(unsigned int i=0; i<lsize; i++) {
    string stat = "pending";
    if(i == currix)
      stat = "current";
    else if(i < currix)
      stat = "Done";
    unsigned int lcond = m_logic_tests.leadConditions(i);
    unsigned int pflag = m_logic_tests.passConditions(i);
    unsigned int fflag = m_logic_tests.failConditions(i);
    actab2 << i << stat << lcond << pflag << fflag;
  }
  m_msgs << actab2.getFormattedString();
  m_msgs << endl;
  m_msgs << endl;

  m_msgs << "==============================================" << endl;
  m_msgs << "Reporting: " << endl;
  m_msgs << "==============================================" << endl;
  m_msgs << "  report_file: " << m_report_file << endl;
  for(unsigned int i=0; i<m_report_columns.size(); i++)
    m_msgs << "   report_col: " << m_report_columns[i] << endl;
  m_msgs << "  latest_line: " << m_report_latest_line << endl;
  m_msgs << endl;


#if 0
  list<string>::iterator p;
  // Part 2: LogicAspect Info
  if(m_logic_tests.enabled()) {
    list<string> summary_lines1 = m_logic_tests.getReport();
    for(p=summary_lines1.begin(); p!=summary_lines1.end(); p++) {
      string line = *p;
      m_msgs << line << endl;
    }
    m_msgs << endl;
  }
#endif

  return(true);
}


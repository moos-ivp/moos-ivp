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
#include "MissionEval.h"
#include "VarDataPairUtils.h"
#include "MacroUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MissionEval::MissionEval()
{
  m_result_flags_posted = false;

  m_info_buffer = new InfoBuffer;
  m_logic_tests.setInfoBuffer(m_info_buffer);
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
      
    // Pass mail to VCheckSet
    m_vcheck_set.handleMail(key, sval, dval, mtime);
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
  
  m_logic_tests.update();
  m_vcheck_set.update(m_curr_time);

  // If both logic_aspect and vcheck_set evaluated, post results
  if(m_logic_tests.isEvaluated() && m_vcheck_set.isEvaluated())
    postResults();
  
  string aspect_status = m_logic_tests.getStatus();
  if(aspect_status != m_logic_tests_status_prev) {
    Notify("MISSION_LCHECK_STAT", aspect_status);
    m_logic_tests_status_prev = aspect_status;
  }
  
  string vcheck_status = m_vcheck_set.getStatus();
  if(vcheck_status != m_vcheck_status_prev) {
    Notify("MISSION_VCHECK_STAT", vcheck_status);
    m_vcheck_status_prev = vcheck_status;
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

    else if(param == "vcheck_start")
      handled = m_vcheck_set.setStartTrigger(value);
    else if(param == "vcheck")
      handled = m_vcheck_set.addVCheck(value);
    else if(param == "fail_on_first")
      handled = m_vcheck_set.setFailOnFirst(value);
    else if(param == "vcheck_max_report")
      handled = m_vcheck_set.setMaxReportSize(value);
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

  // Part 2: Check VCheck proper config
  string vcheck_warning = m_vcheck_set.checkConfig();
  if(vcheck_warning != "")
    reportConfigWarning(vcheck_warning);

  // Part 3: MUST be using either logic-based or time-based checks!
  if(!m_logic_tests.enabled() && !m_vcheck_set.enabled()) 
    reportConfigWarning("MUST provide either logic or time-based checks");
  
  m_mission_result = "pending";
  Notify("MISSION_RESULT", "pending");

  findMacroVars();
  
  registerVariables();	

  vector<string> spec = m_logic_tests.getSpec();
  for(unsigned int i=0; i<spec.size(); i++)
    cout << spec[i] << endl;

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
}


//---------------------------------------------------------
// Procedure: registerVariables()

void MissionEval::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  // Register for variables used in logic_aspect
  set<string> aspect_vars = m_logic_tests.getLogicVars();
  // Register for variables used in vcheck_set
  set<string> vcheck_vars = m_vcheck_set.getVars();

  // Make one set
  aspect_vars.insert(vcheck_vars.begin(), vcheck_vars.end());

  // Now register for all unique vars
  set<string>::iterator p;
  for(p=aspect_vars.begin(); p!=aspect_vars.end(); p++) {
    string moos_var = *p;
    Register(moos_var, 0);
  }

  // Now register for all flag macros
  for(p=m_macro_vars.begin(); p!=m_macro_vars.end(); p++) {
    string moos_var = *p;
    Register(moos_var, 0);
  }

  // Register for any variables involved in the MailFlagSet
  vector<string> mflag_vars = m_mfset.getMailFlagKeys();
  for(unsigned int i=0; i<mflag_vars.size(); i++)
    Register(mflag_vars[i], 0);

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
  if(m_logic_tests.isSatisfied() && m_vcheck_set.isSatisfied()) {
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

  for(unsigned int i=0; i<m_report_columns.size(); i++) {
    string column = expandMacros(m_report_columns[i]);
    if(i != 0)
      column = "  " + column;
    fprintf(f, "%s", column.c_str());
  }
  fprintf(f, "\n");
  fclose(f);
  
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
  unsigned int vcheck_pass = m_vcheck_set.getTotalPassed();
  unsigned int vcheck_fail = m_vcheck_set.getTotalFailed();
  double  vcheck_pass_rate = m_vcheck_set.getPassRate();
  string   vcheck_fail_rpt = m_vcheck_set.getFailReport();
  if(vcheck_fail_rpt == "")
    vcheck_fail_rpt = "none";
  
  sdata = macroExpand(sdata, "VCHK_PASS", vcheck_pass);
  sdata = macroExpand(sdata, "VCHK_FAIL", vcheck_fail);
  sdata = macroExpand(sdata, "VCHK_PASS_RATE", vcheck_pass_rate);
  sdata = macroExpand(sdata, "VCHK_FAIL_RPT", vcheck_fail_rpt);

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
  m_msgs << "============================================ " << endl;
  m_msgs << " logic_tests_stat: " << m_logic_tests.getStatus() << endl;
  m_msgs << "  time_check_stat: " << m_vcheck_set.getStatus() << endl;
  m_msgs << "     result flags: " << rflag_count_str << endl;
  m_msgs << "       pass flags: " << pflag_count_str << endl;
  m_msgs << "       fail flags: " << fflag_count_str << endl << endl;
  m_msgs << "       curr_index: " << m_logic_tests.currIndex() << endl << endl;
  //  m_msgs << "      report_line: [" << m_report_line << "]" << endl;
  m_msgs << "      report_file: " << m_report_file << endl << endl;

  m_msgs << "FlagMacrosVars: " << m_macro_vars.size() << endl;
  m_msgs << "============================================ " << endl;
  set<string>::iterator q;
  for(q=m_macro_vars.begin(); q!=m_macro_vars.end(); q++) {
    string var = *q;
    bool ok;
    string sval = m_info_buffer->sQuery(var, ok);
    double dval = m_info_buffer->dQuery(var, ok);
    if(sval == "")
      sval = doubleToStringX(dval,2);
    m_msgs << " macro:" << *q << "=" << sval << endl;
  }
  m_msgs << endl;
  
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

  // Part 3: VCheck Info
  if(m_vcheck_set.enabled()) {
    list<string> summary_lines2 = m_vcheck_set.getReport();
    for(p=summary_lines2.begin(); p!=summary_lines2.end(); p++) {
      string line = *p;
      m_msgs << line << endl;
    }
  }

  return(true);
}


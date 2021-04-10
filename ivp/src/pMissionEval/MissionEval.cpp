/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: MissionEval.cpp                                      */
/*   DATE: Sep 19th, 2020                                       */
/****************************************************************/

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
}

//---------------------------------------------------------
// Procedure: OnNewMail

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
    
    // Pass mail to LCheckSet
    if(msg.IsDouble())
      m_lcheck_set.handleMail(key, dval);
    else if(msg.IsString())
      m_lcheck_set.handleMail(key, sval);
      
    // Pass mail to VCheckSet
    m_vcheck_set.handleMail(key, sval, dval, mtime);
  }

  // After MailFlagSet has handled all mail from this iteration,
  // post any flags that result. Flags will be cleared in m_mfset.
  postFlags(m_mfset.getNewFlags());
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer
  
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

  m_lcheck_set.update(m_curr_time);
  m_vcheck_set.update(m_curr_time);

  // If both lcheck_set and vcheck_set evaluated, post results
  if(m_lcheck_set.isEvaluated() && m_vcheck_set.isEvaluated())
    postResults();
  
  string lcheck_status = m_lcheck_set.getStatus();
  if(lcheck_status != m_lcheck_status_prev) {
    Notify("MISSION_LCHECK_STAT", lcheck_status);
    m_lcheck_status_prev = lcheck_status;
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

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
	   
    bool handled = false;
    if(param == "lead_condition")
      handled = m_lcheck_set.addLeadCondition(value);
    else if(param == "pass_condition") 
      handled = m_lcheck_set.addPassCondition(value);
    else if(param == "fail_condition") 
      handled = m_lcheck_set.addFailCondition(value);

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
  // Part 1: MUST be using either logic-based or time-based checks!
  if(!m_lcheck_set.enabled() && !m_vcheck_set.enabled()) 
    reportConfigWarning("MUST provide either logic or time-based checks");
  
  // Part 2: Check LCheck proper config
  string lcheck_warning = m_lcheck_set.checkConfig();
  if(lcheck_warning != "")
    reportConfigWarning(lcheck_warning);

  // Part 3: Check LCheck proper config
  string vcheck_warning = m_vcheck_set.checkConfig();
  if(vcheck_warning != "")
    reportConfigWarning(vcheck_warning);
  
  
  m_mission_result = "pending";
  Notify("MISSION_RESULT", "pending");

  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MissionEval::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  // Register for variables used in lcheck_set
  set<string> lcheck_vars = m_lcheck_set.getVars();
  // Register for variables used in vcheck_set
  set<string> vcheck_vars = m_vcheck_set.getVars();

  // Make one set
  set<string>::iterator p;
  for(p=vcheck_vars.begin(); p!=vcheck_vars.end(); p++) 
    lcheck_vars.insert(*p);
  
  // Now register for all unique vars
  for(p=lcheck_vars.begin(); p!=lcheck_vars.end(); p++) {
    string moos_var = *p;
    Register(moos_var, 0);
    m_reg_vars.insert(moos_var);
#if 0
    if(strEnds(moos_var, "_DELTA")) {
      rbiteStringX(moos_var, '_');
      if(moos_var != "") {
	m_reg_vars.insert(moos_var);
	Register(moos_var, 0);
      }
    }
#endif
  }

  // Register for any variables involved in the MailFlagSet
  vector<string> mflag_vars = m_mfset.getMailFlagKeys();
  for(unsigned int i=0; i<mflag_vars.size(); i++)
    Register(mflag_vars[i], 0);
}


//------------------------------------------------------------
// Procedure: postResults

void MissionEval::postResults()
{
  if(m_result_flags_posted)
    return;
  m_result_flags_posted = true;

  // No matter the results, once the eval conditions have been
  // evaluated, the result_flags are posted, if any
  postFlags(m_result_flags);
  Notify("MISSION_EVALUATED", "true");
  
  // If eval conditions met, post pass_flags, otherwise fail_flags
  if(m_lcheck_set.isSatisfied() && m_vcheck_set.isSatisfied()) {
    m_mission_result = "pass";
    Notify("MISSION_RESULT", "pass");
    postFlags(m_pass_flags);
  }
  else {
    m_mission_result = "fail";
    Notify("MISSION_RESULT", "fail");
    postFlags(m_fail_flags);
  }
}

//------------------------------------------------------------
// Procedure: postFlags()

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
  m_msgs << " logic_check_stat: " << m_lcheck_set.getStatus() << endl;
  m_msgs << "  time_check_stat: " << m_vcheck_set.getStatus() << endl;
  m_msgs << "     result flags: " << rflag_count_str << endl;
  m_msgs << "       pass flags: " << pflag_count_str << endl;
  m_msgs << "       fail flags: " << fflag_count_str << endl << endl;

  list<string>::iterator p;
  // Part 2: LCheck Info
  if(m_lcheck_set.enabled()) {
    list<string> summary_lines1 = m_lcheck_set.getReport();
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

  set<string> logic_vars = m_lcheck_set.getVars();
  
  m_msgs << endl;
  m_msgs << "LogicVars: " << endl;
  m_msgs << "============================================ " << endl;
  set<string>::iterator q;
  for(q=logic_vars.begin(); q!=logic_vars.end(); q++)
    m_msgs << "  " << *q << endl;
  
  return(true);
}

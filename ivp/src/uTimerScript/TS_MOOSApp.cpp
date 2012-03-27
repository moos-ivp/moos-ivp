/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TS_MOOSApp.cpp                                       */
/*    DATE: May 21st 2009                                        */
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

#include <cstdlib>
#include <iterator>
#include "TS_MOOSApp.h"
#include "MBUtils.h"
#include "ColorParse.h"

#ifdef _WIN32
#include <process.h>
//    #include "MOOSAppRunnerThread.h"
#define getpid _getpid
#endif

using namespace std;

//---------------------------------------------------------
// Constructor

TS_MOOSApp::TS_MOOSApp()
{
  // Initial values for state variables.
  m_elapsed_time   = 0;
  m_previous_time  = -1;
  m_start_time     = 0;
  m_connect_tstamp = 0;
  m_status_tstamp  = 0;
  m_status_needed  = false;
  m_skip_time      = 0;
  m_pause_time     = 0;
  m_utc_time       = 0;
  m_paused         = false;
  m_conditions_ok  = true;
  m_posted_count   = 0;
  m_posted_tcount  = 0;
  m_reset_count    = 0;
  m_iterations     = 0; 
  m_iter_char      = 'a';
  m_verbose        = true;
  m_shuffle        = true;
  m_atomic         = false;
  m_upon_awake     = "n/a";
  m_script_name    = "unnamed";

  // Default values for configuration parameters.
  m_reset_max      = 0;
  m_reset_forever  = true;
  m_reset_time     = -1; // -1:none, 0:after-last, NUM:atNUM
  m_var_forward    = "UTS_FORWARD";
  m_var_pause      = "UTS_PAUSE";
  m_var_status     = "UTS_STATUS";
  m_var_reset      = "UTS_RESET";

  m_info_buffer    = new InfoBuffer;

  m_time_warp.setRange(1.0, 1.0);
  m_delay_start.setRange(0, 0);
  m_delay_reset.setRange(0, 0);

  seedRandom();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

    if((key == m_var_forward) && !mstr) {
      if(dval <= 0)
	jumpToNextPostingTime();
      else
	m_skip_time += dval;
    }
    else if(key == m_var_reset) {
      string str = tolower(sval);
      if((str == "reset") || (str == "true"))
	handleReset();
    }
    else if(key == "DB_UPTIME") {
      if(m_connect_tstamp == 0) 
	m_connect_tstamp = (MOOSTime() - dval);
    }
    else if(key == m_var_pause) {
      string pause_val = tolower(sval);
      m_status_needed = true;
      if(pause_val == "true")
	m_paused = true;
      else if(pause_val == "false")
	m_paused = false;
      else if(pause_val == "toggle")
	m_paused = !m_paused;
    }
    else
      updateInfoBuffer(msg);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool TS_MOOSApp::Iterate()
{
  // By default we declare that a posting of the status message is not
  // needed - until either an event occurs, elapsed time or state chng.
  m_status_needed = false;

  // If we are transitioning from a state when the conditions were not
  // met, to one where they are, this constitutes an "awakening".
  // (a) If upon_awake==reset, then a further check is made to see if 
  // there has been script progress defined by (m_elapsed_time > 0),
  // otherwise the reset is not perform.
  // (b) if upon_awake==restart, then the script is simply restarted.
  bool new_conditions_ok = m_conditions_ok;
  if(!m_atomic || (m_posted_count == 0) || 
     (m_pairs.size() == m_posted_count))
    new_conditions_ok = checkConditions();

  if(new_conditions_ok && !m_conditions_ok) { // Possible awakening
    if((m_upon_awake == "reset") && (m_elapsed_time > 0))
      handleReset();
    else if(m_upon_awake == "restart")
      handleRestart();
  }

  // If the status of conditions changes, worth posting a status msg.
  if(new_conditions_ok != m_conditions_ok)
    m_status_needed = true;
  // Now apply the results of the new conditions state
  m_conditions_ok = new_conditions_ok;

  m_utc_time = MOOSTime();
  if(m_start_time == 0) {
    m_start_time = m_utc_time;
    scheduleEvents(m_shuffle);
  }

  if(m_paused || !m_conditions_ok) {
    double delta_time = 0;
    if(m_previous_time != -1)
      delta_time = m_utc_time - m_previous_time;
    m_pause_time += delta_time;
  }

  // Make sure these four steps are done *before* any resets.
  m_previous_time = m_utc_time;
  m_elapsed_time = (m_utc_time - m_start_time) + m_skip_time - m_pause_time;
  m_elapsed_time *= m_time_warp.getValue();
  if((m_status_tstamp == 0) || ((m_utc_time - m_status_tstamp) > 5))
    m_status_needed = true;
  
  bool all_posted = checkForReadyPostings();

  // Do the reset only if all events are posted AND the reset_time is 
  // set to zero reset indicating desired after all postings complete.
  if((all_posted) && (m_reset_time ==0))
    handleReset();

  // The below check for reset (based on purely elapsed time) is done
  // in addition to, and after the above check for reset (based on all
  // events being posted). Because - it is possible to have (a) elapsed
  // time > reset time, but some events not posted (b) those unposted
  // events having a time less than reset time.
  if((m_reset_time > 0) && (m_elapsed_time >= m_reset_time))
    handleReset();

  m_iterations++;
  m_iter_char++;
  if(m_iter_char > 122)
    m_iter_char -= 26;

  if(m_status_needed) 
    postStatus();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TS_MOOSApp::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  RegisterVariables();  

  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool TS_MOOSApp::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("uTimerScript starting....\n");

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string line  = stripBlankEnds(*p);
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);

      if(param == "event")
	addNewEvent(value);
      else if(param == "paused")
	setBooleanOnString(m_paused, value);
      else if(param == "script_atomic")
	setBooleanOnString(m_atomic, value);
      else if(param == "verbose")
	setBooleanOnString(m_verbose, value);
      else if(param == "shuffle")
	setBooleanOnString(m_shuffle, value);
      else if(param == "upon_awake") {
	string lvalue = tolower(value);
	if((lvalue=="reset") || (lvalue=="restart") || (lvalue =="n/a"))
	  m_upon_awake = lvalue;
      }
      else if(param == "reset_max") {
	string str = tolower(value);
	if((str == "any") || (str == "unlimited") || (str == "nolimit"))
	  m_reset_forever = true;
	else if(isNumber(value) && (atoi(value.c_str()) >= 0)) {
	  m_reset_max = atoi(value.c_str());
	  m_reset_forever = false;
	}
      }
      else if(param == "reset_time") {
	string str = tolower(value);
	if((str == "end") || (str == "all-posted"))
	  m_reset_time = 0;
	else if(str == "none")  // Default
	  m_reset_time = -1;
	else if(isNumber(value) && (atof(value.c_str()) > 0))
	  m_reset_time = atof(value.c_str());
      }
      else if(param == "script_name")
	m_script_name = value;
      else if((param == "forward_var") || (param == "forward_variable")) {
	if(!strContainsWhite(value))
	  m_var_forward = value;
      }
      else if((param == "reset_var") || (param == "reset_variable")) {
	if(!strContainsWhite(value))
	  m_var_reset = value;
      }
      else if((param == "pause_var") || (param == "pause_variable")) {
	if(!strContainsWhite(value))
	  m_var_pause = value;
      }
      else if(param == "status_var") {
	if(!strContainsWhite(value))
	  m_var_status = value;
      }
      else if(param == "time_warp") {
	string left  = stripBlankEnds(biteString(value, ':'));
	string right = stripBlankEnds(value);
	double lval  = atof(left.c_str());
	double rval  = atof(right.c_str());
	if(right == "")
	  rval = lval;
	if((lval > 0) && (lval <= rval))
	  m_time_warp.setRange(lval, rval);
      }
      else if(param == "delay_start") {
	string left  = stripBlankEnds(biteString(value, ':'));
	string right = stripBlankEnds(value);
	if(right == "")
	  right = left;
	double lval  = atof(left.c_str());
	double rval  = atof(right.c_str());
	if(isNumber(left) && isNumber(right) && (lval >= 0) && (lval <= rval))
	  m_delay_start.setRange(lval, rval);
      }
      else if(param == "delay_reset") {
	string left  = stripBlankEnds(biteString(value, ':'));
	string right = stripBlankEnds(value);
	if(right == "")
	  right = left;
	double lval  = atof(left.c_str());
	double rval  = atof(right.c_str());
	if(isNumber(left) && isNumber(right) && (lval >= 0) && (lval <= rval))
	  m_delay_reset.setRange(lval, rval);
      }
      else if((param == "rand_var") || (param == "randvar")) {
	string result = m_rand_vars.addRandomVar(value);
	if(result != "") {
	  cout << termColor("red");
	  cout << "Problem configuring random variable: " << endl;
	  cout << "  " << value << endl;
	  cout << "  " << result << endl;
	  cout << termColor() << endl;
	}
      }
      else if(param == "condition") {
	LogicCondition new_condition;
	bool ok = new_condition.setCondition(value);
	if(ok)
	  m_logic_conditions.push_back(new_condition);
	m_conditions_ok = false; // Assume false until shown otherwise
      }
      else {
	if((param != "apptick") && (param != "commstick")) {
	  cout << termColor("red") << "Unrecognized Parameter: ";
	  cout << param << termColor() << endl;
	}
      }
    }
  }
  
  if(m_verbose) {
    cout << termColor("blue") << "Random Variable configurations: " << endl;
    unsigned int i, vsize = m_rand_vars.size();
    for(i=0; i<vsize; i++)
      cout << "  [" << i << "]: " << m_rand_vars.getStringSummary(i) << endl;
    cout << termColor() << endl;
  }

  // Reset (Determine new values for) random variables of all three types:
  // Those that are reset once at the beginning of uTimerScript startup, 
  // those that are reset once each time the script is re-set, and those
  // that are reset after each posting.
  m_rand_vars.reset("at_start");
  m_rand_vars.reset("at_reset");
  m_rand_vars.reset("at_post");

  RegisterVariables();
  if(m_verbose)
    printRawScript();

  return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void TS_MOOSApp::RegisterVariables()
{
  m_Comms.Register(m_var_forward, 0);
  m_Comms.Register(m_var_pause, 0);
  m_Comms.Register(m_var_reset, 0);
  m_Comms.Register("DB_UPTIME", 0);

  // Get all the variable names from all present conditions.
  vector<string> all_vars;
  unsigned int i, vsize = m_logic_conditions.size();
  for(i=0; i<vsize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

  // Register for all variables found in all conditions.
  unsigned int all_size = all_vars.size();
  for(i=0; i<all_size; i++)
    m_Comms.Register(all_vars[i], 0);
 
}

//------------------------------------------------------------
// Procedure: addNewEvent()
// EVENT = var=FOOBAR, val=true, time=45.0

bool TS_MOOSApp::addNewEvent(string event_str)
{
  string new_var;
  string new_val;
  double new_ptime_min = -1;
  double new_ptime_max = -1;

  vector<string> svector = parseStringQ(event_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "var")
      new_var = value;
    else if(param == "val") {
      string idx_string = uintToString(m_pairs.size());
      idx_string = padString(idx_string, 3);
      idx_string = findReplace(idx_string, ' ', '0');
      value = findReplace(value, "$$IDX", idx_string);
      value = findReplace(value, "$(IDX)", idx_string);
      value = findReplace(value, "$[IDX]", idx_string);
      new_val = value;
    }
    else if(param == "time") {
      if(isNumber(value)) {
	double dval = atof(value.c_str());
	if(dval >= 0) {
	  new_ptime_min = dval;
	  new_ptime_max = dval;
	}
      }
      else if(strContains(value, ':')) {
	string left   = stripBlankEnds(biteString(value, ':'));
	string right  = stripBlankEnds(value);
	double dleft  = atof(left.c_str());
	double dright = atof(right.c_str());
	if(isNumber(left) && isNumber(right) && (dleft <= dright)) {
	  new_ptime_min = dleft;
	  new_ptime_max = dright;
	}
      }
    }
  }
  
  if((new_var=="") || (new_val=="") || (new_ptime_min == -1))
    return(false);

  VarDataPair new_pair(new_var, new_val, "auto");
  m_pairs.push_back(new_pair);
  m_ptime.push_back(-1);
  m_ptime_min.push_back(new_ptime_min);
  m_ptime_max.push_back(new_ptime_max);
  m_poked.push_back(false);

  return(true);
}

//------------------------------------------------------------
// Procedure: scheduleEvents
//   Purpose: (1) Post an updated header if in verbose mode. 
//            (2) Determine if there are events that do not yet have
//                timestamps.
//            (3) Shuffle (assign timestamps) if shuffle requested or
//                if one or more events do not have a timestamp.
//            (4) Re-set all state variables to initial values. 

void TS_MOOSApp::scheduleEvents(bool shuffle_requested)
{
  // Part (1): Post an updated header if in verbose mode.
  if(m_verbose) {
    cout << termColor("magenta");
    cout << "Script (Re)Initialized. Time Warp=" << m_time_warp.getValue();
    cout << "  DelayStart=" << m_delay_start.getValue();
    cout << "  DelayReset=" << m_delay_reset.getValue();
    cout << termColor() << endl;
  }
  
  // Part (2): Determine if there are events that do not yet have timestamps.
  bool unscheduled_events = false;
  unsigned int i, vsize = m_pairs.size();
  for(i=0; i<vsize; i++)
    if(m_ptime[i] == -1)
      unscheduled_events = true;

  // Part (3): Shuffle (assign timestamps) if shuffle requested or if one or
  // more events do not have a timestamp.
  if(shuffle_requested || unscheduled_events) {
    for(i=0; i<vsize; i++) {
      int    rand_int = rand() % 10000;
      double rand_pct = (double)(rand_int) / 10000;
      m_ptime[i] = m_ptime_min[i] + (rand_pct * (m_ptime_max[i] - m_ptime_min[i]));
    }
  }

  // Part (4): Re-set all state variables to initial values. 
  m_time_warp.reset();
  m_rand_vars.reset("at_reset");
  m_rand_vars.reset("at_post");
  m_elapsed_time  = 0;
  m_previous_time = -1;
  m_skip_time     = 0;
  m_pause_time    = 0;
  m_posted_count  = 0;

  // Mark all the posts as being unposted
  unsigned int j, psize=m_poked.size();
  for(j=0; j<psize; j++)
    m_poked[j] = false;
}
    
//------------------------------------------------------------
// Procedure: printRawScript
// 

void TS_MOOSApp::printRawScript()
{
  unsigned int i, vsize = m_pairs.size();
  
  cout << termColor("magenta") << endl;
  cout << "The Raw Script: ========================================" << endl;
  cout << "Total Elements: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    string vdpair_str = m_pairs[i].getPrintable();
    double ptime  = m_ptime[i];
    bool   poked  = m_poked[i];

    cout << "[" << i << "] " << vdpair_str << ", TIME:" << ptime;
    cout << ", RANGE=[" << m_ptime_min[i] << "," << m_ptime_max[i] << "]";
    cout << ", POSTED=" << boolToString(poked) << endl;
  }
  cout << "====================================================" << endl;
  cout << termColor() << endl;
}
    
//----------------------------------------------------------------
// Procedure: checkForReadyPostings()
//   Purpose: Go through the list of events and possibly post the
//            event if the elapsed time is greater or equal to the
//            post time, and if not posted previously.
//   Returns: True if all postings have been made by the end of this
//            function call - false otherwise

bool TS_MOOSApp::checkForReadyPostings()
{
  unsigned int i, vsize = m_pairs.size();
  bool all_poked = true;
  for(i=0; i<vsize; i++) {
    double delay = 0;
    if(m_reset_count == 0)
      delay += m_delay_start.getValue();
    else
      delay += m_delay_reset.getValue();
    // Condtions for posting: (1) enough elapsed time, (2) not already
    // poked, (3) poke time is not after reset-time if reset-time set.
    if(((m_elapsed_time - delay) >= m_ptime[i]) && !m_poked[i] && 
       ((m_reset_time <= 0) || (m_ptime[i] <= m_reset_time))) {
      executePosting(m_pairs[i]);
      m_status_needed = true;
      
      // If just now poked, note it, so it won't be poked again
      m_posted_count++;
      m_posted_tcount++;
      m_poked[i] = true;
    }
    all_poked = all_poked && m_poked[i];
  }

  return(all_poked);
}

//----------------------------------------------------------------
// Procedure: executePosting()

void TS_MOOSApp::executePosting(VarDataPair pair)
{
  double db_uptime = MOOSTime() - m_connect_tstamp;

  m_rand_vars.reset("at_post");
  string variable = pair.get_var();

  if(!pair.is_string()) {
    printPosting(variable, pair.get_ddata(), db_uptime);
    m_Comms.Notify(variable, pair.get_ddata());
    return;
  }
    
  string sval = pair.get_sdata();
  // Handle special case where MOOSDB_UPTIME *is* the post
  if((sval == "$(DBTIME)") || (sval == "$[DBTIME]")) {
    printPosting(variable, db_uptime, db_uptime);
    m_Comms.Notify(variable, db_uptime);
    return;
  }

  // Handle special case where UTC_TIME *is* the post
  if((sval == "$(UTCTIME)") || (sval == "$[UTCTIME]")) {
    printPosting(variable, db_uptime, db_uptime);
    m_Comms.Notify(variable, db_uptime);
    return;
  }

  // Handle special case where COUNT *is* the post
  else if((sval == "$(COUNT)") || (sval == "$[COUNT]")) {
    printPosting(variable, m_posted_count, db_uptime);
    m_Comms.Notify(variable, m_posted_count);
    return;
  }
  
  // Handle special case where COUNT *is* the post
  else if((sval == "$(TCOUNT)") || (sval == "$[TCOUNT]")) {
    printPosting(variable, m_posted_tcount, db_uptime);
    m_Comms.Notify(variable, m_posted_tcount);
    return;
  }
  
  sval = findReplace(sval, "$(DBTIME)", doubleToString(db_uptime, 2));
  sval = findReplace(sval, "$(UTCTIME)", doubleToString(m_utc_time, 2));
  sval = findReplace(sval, "$(TCOUNT)", uintToString(m_posted_tcount));
  sval = findReplace(sval, "$(COUNT)", uintToString(m_posted_count));
 
  sval = findReplace(sval, "$[DBTIME]", doubleToString(db_uptime, 2));
  sval = findReplace(sval, "$[UTCTIME]", doubleToString(m_utc_time, 2));
  sval = findReplace(sval, "$[TCOUNT]", uintToString(m_posted_tcount));
  sval = findReplace(sval, "$[COUNT]", uintToString(m_posted_count));

  sval = findReplace(sval, "$(1)", randomNumString(1));
  sval = findReplace(sval, "$(2)", randomNumString(2));
  sval = findReplace(sval, "$(3)", randomNumString(3));
  sval = findReplace(sval, "$(4)", randomNumString(4));
  sval = findReplace(sval, "$(5)", randomNumString(5));
  sval = findReplace(sval, "$(6)", randomNumString(6));
  sval = findReplace(sval, "$(7)", randomNumString(7));
  sval = findReplace(sval, "$(8)", randomNumString(8));
  sval = findReplace(sval, "$(9)", randomNumString(9));
  sval = findReplace(sval, "$(10)", randomNumString(10));
  sval = findReplace(sval, "$(11)", randomNumString(11));
  sval = findReplace(sval, "$(12)", randomNumString(12));
  sval = findReplace(sval, "$(13)", randomNumString(13));
  sval = findReplace(sval, "$(14)", randomNumString(14));
  sval = findReplace(sval, "$(15)", randomNumString(15));
  sval = findReplace(sval, "$(16)", randomNumString(16));
  sval = findReplace(sval, "$(17)", randomNumString(17));
  sval = findReplace(sval, "$(18)", randomNumString(18));
  sval = findReplace(sval, "$(19)", randomNumString(19));
  sval = findReplace(sval, "$(20)", randomNumString(20));

  
  unsigned int i, vsize = m_rand_vars.size();
  for(i=0; i<vsize; i++) {
    string macro1 = "$(" + m_rand_vars.getVarName(i) + ")";
    string macro2 = "$[" + m_rand_vars.getVarName(i) + "]";
    sval = findReplace(sval, macro1, m_rand_vars.getStringValue(i));
    sval = findReplace(sval, macro2, m_rand_vars.getStringValue(i));
  }

  // Handle all the math expansions. For safety, the while loop may
  // execute only 20 times.
  unsigned int replace_max = 10;
  unsigned int replace_amt = 0;
  bool maybe_more = true;
  while(maybe_more && (replace_amt <= replace_max)) {
    maybe_more = handleMathExpr(sval);
    replace_amt++;
  }

  if(isNumber(sval) && !pair.is_quoted()) {
    double dval = atof(sval.c_str());
    printPosting(variable, dval, db_uptime);
    m_Comms.Notify(variable, dval);
  }
  else {
    printPosting(variable, sval, db_uptime);
    m_Comms.Notify(variable, sval);
  }
}

//----------------------------------------------------------------
// Procedure: printPosting()

void TS_MOOSApp::printPosting(string var, string sval, double db_uptime)
{
  if(m_verbose) {
    cout << termColor("green");
    cout << "[" << m_iter_char << "/" << db_uptime << "]";
    cout << termColor("blue");
    cout << "[" << m_elapsed_time << "]: ";
    cout << var << " = " << sval << endl;
    cout << termColor();
  }
}

//----------------------------------------------------------------
// Procedure: printPosting()

void TS_MOOSApp::printPosting(string var, double dval, double db_uptime)
{
  if(m_verbose) {
    cout << termColor("green");
    cout << "[" << m_iter_char << "/" << db_uptime << "]";
    cout << termColor("blue");
    cout << "[" << m_elapsed_time << "]: ";
    cout << var << " = " << dval << endl;
    cout << termColor();
  }
}

//----------------------------------------------------------------
// Procedure: jumpToNextPosting()
//   Purpose: Go through the list of events and find the next un-poked
//            var-data pair and jump forward in time to the time at 
//            which the next unpoked var-data pair is ready.
//            The member variable "m_skip_time" is incremented to hold
//            the cumulative time jumped forward.

void TS_MOOSApp::jumpToNextPostingTime()
{
  // Find the smallest hypothetical skip time
  double skip_amt = -1;
  unsigned int i, vsize = m_pairs.size();
  for(i=0; i<vsize; i++) {
    if(!m_poked[i]) {
      double new_skip_amt = m_ptime[i] - m_elapsed_time;
      if((skip_amt == -1) || (new_skip_amt < skip_amt))
	skip_amt = new_skip_amt;
    }
  }
  
  if(skip_amt != -1)
    m_skip_time += skip_amt;
}


//----------------------------------------------------------------
// Procedure: handleReset()
//   Purpose: Reset the script to how it was at the initial launch.
//            HOWEVER, the PAUSE state is not reset. 

void TS_MOOSApp::handleReset()
{
  // Check if the number of allowed resets has been reached. Unlimited
  // resets are indicated by m_reset_forever=true.
  if(!m_reset_forever && (m_reset_count >= m_reset_max))
    return;
  m_reset_count++;
  
  // A reset should trigger an immediate status message publication
  m_status_needed = true;

  // The key indicator for a script-reset is setting m_start_time to
  // zero. The real work or resetting is done elsewhere.
  m_start_time = 0;
}
  
//----------------------------------------------------------------
// Procedure: handleRestart()
//   Purpose: Reset the script to how it was at the initial launch.

void TS_MOOSApp::handleRestart()
{
  m_reset_count = 0;
  
  // A reset should trigger an immediate status message publication
  m_status_needed = true;

  // The key indicator for a script-reset is setting m_start_time to
  // zero. The real work or resetting is done elsewhere.
  m_start_time = 0;
}
  
  
//----------------------------------------------------------------
// Procedure: postStatus

void TS_MOOSApp::postStatus()
{
  if((m_var_status == "") || (tolower(m_var_status) == "silent"))
    return;

  string status = "name=" + m_script_name;
  status += ", elapsed_time=" + doubleToString(m_elapsed_time,2);
  status += ", posted=" + uintToString(m_posted_count);
  
  int pending = (m_pairs.size() - m_posted_count);
  status += ", pending=" + uintToString(pending);

  status += ", paused=" + boolToString(m_paused);
  status += ", conditions_ok=" + boolToString(m_conditions_ok);
  status += ", time_warp=";
  status += doubleToStringX(m_time_warp.getValue());
  status += ", delay_start=";
  status += doubleToStringX(m_delay_start.getValue());
  status += ", delay_reset=";
  status += doubleToStringX(m_delay_reset.getValue());
  status += ", shuffle="     + boolToString(m_shuffle);
  status += ", upon_awake=" + m_upon_awake;
  
  string maxresets = "/any";
  if(!m_reset_forever)
    maxresets = "/" + uintToString(m_reset_max);
  status += ", resets=" + uintToString(m_reset_count) + maxresets;
  
  m_Comms.Notify(m_var_status, status);
  m_status_tstamp = m_utc_time;
}
  
  
//----------------------------------------------------------------
// Procedure: seedRandom

void TS_MOOSApp::seedRandom()
{
  unsigned long tseed = time(NULL)+1;
  unsigned long pid = (long)getpid()+1;
  unsigned long seed = (tseed%999999);
  seed = ((rand())*seed)%999999;
  seed = (seed*pid)%999999;
  srand(seed);
}


//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool TS_MOOSApp::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.GetKey();
  string sdata = msg.GetString();
  double ddata = msg.GetDouble();

  if(msg.IsDataType(MOOS_DOUBLE)) {
    return(m_info_buffer->setValue(key, ddata));
  }
  else if(msg.IsDataType(MOOS_STRING)) {
    return(m_info_buffer->setValue(key, sdata));
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: checkConditions()
//   Purpose: Determine if all the logic conditions in the vector
//            of conditions is met, given the snapshot of variable
//            values in the info_buffer.

bool TS_MOOSApp::checkConditions()
{
  if(!m_info_buffer) 
    return(false);

  unsigned int i, j, vsize, csize;

  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  csize = m_logic_conditions.size();
  for(i=0; i<csize; i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);

  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  vsize = all_vars.size();
  for(i=0; i<vsize; i++) {
    string varname = all_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);

    for(j=0; (j<csize)&&(ok_s); j++)
      m_logic_conditions[j].setVarVal(varname, s_result);
    for(j=0; (j<csize)&&(ok_d); j++)
      m_logic_conditions[j].setVarVal(varname, d_result);
  }

  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(i=0; i<csize; i++) {
    bool satisfied = m_logic_conditions[i].eval();
    if(!satisfied)
      return(false);
  }
  return(true);
}


//-----------------------------------------------------------
// Procedure: randomNumString
//   Purpose: Generate a string of random digits of given length

string TS_MOOSApp::randomNumString(unsigned int digits)
{
  string result;

  unsigned int i;
  for(i=0; i<digits; i++) {
    int  rand_ix    = rand() % 10;
    char char_digit = 48 + rand_ix;
    // Don't let a zero be the first number.
    if((rand_ix != 0) || (result != ""))
      result += char_digit;
  }
  return(result);
}



//-----------------------------------------------------------
// Procedure: handleMathExpr()
//   Purpose: Handle expressions such as: 
//            "magnitude={$[MAXMAG]*0.7}, type=alpha" 
//            "farenheit={{$[CELCIUS]*1.8}+32}"
//      Note: By the time this function is called, the macros will
//            have been expanded to their numerical values.

bool TS_MOOSApp::handleMathExpr(string& str)
{
  unsigned int len = str.length();

  // Part 1: Ensure that all the left/right braces are well matched
  unsigned int i, depth=0;
  bool braces_detected = false;
  for(i=0; i<len; i++) {
    char c = str.at(i);
    if(c == '{')
      depth++;
    else if(c == '}')
      depth--;
    if(depth < 0)
      return(false);
    if(depth > 0)
      braces_detected = true;
  }
  if(depth != 0)
    return(false);
  if(!braces_detected)
    return(false);

  // Part 2: find the deepest '{' and matching '}' occurance
  unsigned int max_lix = 0;
  unsigned int max_rix = 0;
  unsigned int max_depth = 0;
  depth = 0;
  for(i=0; i<len; i++) {
    char c = str.at(i);
    if(c == '{') {
      depth++;
      if(depth > max_depth) {
	max_lix = i;
	max_depth = depth;
      }
    }
    else if(c == '}') {
      if(depth == max_depth)
	max_rix = i;
      depth--;
    }
  }
  if(max_lix >= max_rix)  // should never be the case but check anyway.
    return(false);

  // Part 3: Find the substring, confirm it, and strip the braces.
  string expression = str.substr(max_lix, ((max_rix-max_lix)+1));
  unsigned int xlen = expression.length();
  if((expression.at(0) != '{') || (expression.at(xlen-1) != '}'))
    return(false);
  expression = stripBlankEnds(expression.substr(1, xlen-2));
  xlen = expression.length();
  
  // Part 4: Handle special cases where left component begins with '+' or '-'
  bool neg_left = false;
  if(expression.at(0) == '+')
    expression = expression.substr(1, xlen-1);
  else if(expression.at(0) == '-') {
    expression = expression.substr(1, xlen-1);
    neg_left = true;
  }

  // Part 5: Determine if multiplication, division, addition, subtraction
  char xtype = ' ';
  if(strContains(expression, '*'))
    xtype = '*';
  else if(strContains(expression, '/'))
    xtype = '/';
  else if(strContains(expression, '+'))
    xtype = '+';
  else if(strContains(expression, '-'))
    xtype = '-';
  if(xtype == ' ')
    return(false);

  // Part 6: Get the left and right components
  string left, right;
  if(xtype == '*') {
    left = stripBlankEnds(biteString(expression, '*'));
    right = stripBlankEnds(expression);
  }
  else if(xtype == '/') {
    left = stripBlankEnds(biteString(expression, '/'));
    right = stripBlankEnds(expression);
  }
  else if(xtype == '+') {
    left = stripBlankEnds(biteString(expression, '+'));
    right = stripBlankEnds(expression);
  }
  else if(xtype == '-') {
    left = stripBlankEnds(biteString(expression, '-'));
    right = stripBlankEnds(expression);
  }
  if(!isNumber(left) || !isNumber(right))
    return(false);

  // Part 7: Determine the mathematical result and replace
  double result = 0;
  double ldval = atof(left.c_str());
  if(neg_left)
    ldval *= -1;
  double rdval = atof(right.c_str());
  if(xtype == '*')
    result = ldval * rdval;
  else if(xtype == '/')
    result = ldval / rdval;
  else if(xtype == '+')
    result = ldval + rdval;
  else if(xtype == '-')
    result = ldval - rdval;
  string replacement = doubleToStringX(result, 6);
  
  string target = str.substr(max_lix, (max_rix-max_lix)+1);
  str = findReplace(str, target, replacement);

  return(true);
}



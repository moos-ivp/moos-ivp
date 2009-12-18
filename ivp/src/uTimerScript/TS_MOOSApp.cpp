/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.cpp                                  */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#include <cstdlib>
#include <iterator>
#include "TS_MOOSApp.h"
#include "MBUtils.h"

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
  m_elapsed_time  = 0;
  m_previous_time = -1;
  m_start_time    = 0;
  m_skip_time     = 0;
  m_pause_time    = 0;
  m_db_uptime     = 0;
  m_utc_time      = 0;
  m_paused        = false;
  m_conditions_ok = true;
  m_posted_count  = 0;
  m_posted_tcount = 0;
  m_reset_count   = 0;
  m_iteration     = 0; 

  // Default values for configuration parameters.
  m_reset_max      = -1;
  m_reset_time     = -1; // -1:none, 0:after-last, NUM:atNUM
  m_var_next_event = "TIMER_SCRIPT_NEXT";
  m_var_forward    = "TIMER_SCRIPT_FORWARD";
  m_var_pause      = "TIMER_SCRIPT_PAUSE";
  m_var_status     = "TIMER_SCRIPT_STATUS";
  m_var_reset      = "TIMER_SCRIPT_RESET";

  m_info_buffer    = new InfoBuffer;

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
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

    if((key == m_var_next_event) && (tolower(sval) == "next"))
      jumpToNextPostingTime();
    else if((key == m_var_forward) && !mstr && (dval>0))
      m_skip_time += dval;
    else if(key == m_var_reset) {
      string str = tolower(sval);
      if((str == "reset") || (str == "true"))
	handleReset();
    }
    else if(key == "DB_UPTIME")
      m_db_uptime = dval;
    else if(key == m_var_pause) {
      string pause_val = tolower(sval);
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
  if(m_iteration == 0) {
    sortEvents();
    expandIndexPairs();
    printScript();
  }
  m_conditions_ok = checkConditions();

  m_utc_time = MOOSTime();

  double curr_time = MOOSTime();
  if(m_start_time == 0)
    m_start_time = curr_time;  

  if(m_paused || !m_conditions_ok) {
    double delta_time = 0;
    if(m_previous_time != -1)
      delta_time = curr_time - m_previous_time;
    m_pause_time += delta_time;
  }
  // Make sure these three steps are done *before* any resets.
  m_previous_time = curr_time;
  m_elapsed_time = (curr_time - m_start_time) + m_skip_time - m_pause_time;
  postStatus();
  
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

  m_iteration++;
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
      else if(param == "reset_max") {
	string str = tolower(value);
	if((str == "any") || (str == "unlimited"))
	  m_reset_max = -1;
	else if(isNumber(value) && (atoi(value.c_str()) >= 0))
	  m_reset_max = atoi(value.c_str());
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
      else if((param == "jump_var") || (param == "jump_variable")) {
	if(!strContainsWhite(value))
	  m_var_next_event = value;
      }
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
      else if(param == "condition") {
	LogicCondition new_condition;
	bool ok = new_condition.setCondition(value);
	if(ok)
	  m_logic_conditions.push_back(new_condition);
	m_conditions_ok = true;
      }
    }
  }

  RegisterVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void TS_MOOSApp::RegisterVariables()
{
  m_Comms.Register(m_var_next_event, 0);
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
  double new_time_of_event = -1;

  vector<string> svector = parseStringQ(event_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "var")
      new_var = value;
    else if(param == "val") {
      if(strContains(value, "$$IDX")) {
	string idx_string = intToString(m_pairs.size());
	idx_string = padString(idx_string, 3);
	idx_string = findReplace(idx_string, ' ', '0');
	value = findReplace(value, "$$IDX", idx_string);
      }
      new_val = value;
    }
    else if(param == "time") {
      if(isNumber(value)) {
	double dval = atof(value.c_str());
	if(dval >= 0)
	  new_time_of_event = dval;
      }
      else if(strContains(value, ':')) {
	string left   = stripBlankEnds(biteString(value, ':'));
	string right  = stripBlankEnds(value);
	double dleft  = atof(left.c_str());
	double dright = atof(right.c_str());
	if(isNumber(left) && isNumber(right) && (dleft <= dright)) {
	  double delta = (100 * (dright - dleft));
	  if(delta < 2) 
	    new_time_of_event = dleft;
	  else {
	    int rand_int = rand() % ((int)(delta));
	    new_time_of_event = dleft + ((double)(rand_int)/100);
	  }
	}
      }
    }
  }
  
  if((new_var=="") || (new_val=="") || (new_time_of_event <0))
    return(false);

  VarDataPair new_pair(new_var, new_val, "auto");
  m_pairs.push_back(new_pair);
  m_ptime.push_back(new_time_of_event);
  m_poked.push_back(false);

  sortEvents();
  return(true);
}

//------------------------------------------------------------
// Procedure: expandIndexPairs
// 

void TS_MOOSApp::expandIndexPairs()
{
  unsigned int i, vsize = m_pairs.size();
  for(i=0; i<vsize; i++) {
    string value = m_pairs[i].get_sdata();
    if(strContains(value, "$$IDX")) {
      string idx_string = intToString(i);
      idx_string = padString(idx_string, 3);
      idx_string = findReplace(idx_string, ' ', '0');
      value = findReplace(value, "$$IDX", idx_string);
      m_pairs[i].set_sdata(value);
    }
  }
}
    
//------------------------------------------------------------
// Procedure: sortEvents
// 

void TS_MOOSApp::sortEvents()
{
  unsigned int count = 0;
  unsigned int i, vsize = m_pairs.size();
  vector<bool> sorted(vsize,false);
  
  vector<VarDataPair> new_pairs;
  vector<double>      new_ptime;
  vector<bool>        new_poked;

  while(count < vsize) {
    unsigned int oldest_index = 0;
    double       oldest_ptime = -1;
    for(i=0; i<vsize; i++) {
      if((m_ptime[i] > oldest_ptime) && !sorted[i]) {
	oldest_ptime = m_ptime[i];
	oldest_index = i;
      }
    }
    sorted[oldest_index] = true;
    new_pairs.push_back(m_pairs[oldest_index]);
    new_ptime.push_back(m_ptime[oldest_index]);
    new_poked.push_back(m_poked[oldest_index]);
    count++;
  }

  m_pairs = new_pairs;
  m_ptime = new_ptime;
  m_poked = new_poked;
  std::reverse(m_pairs.begin(), m_pairs.end());
  std::reverse(m_ptime.begin(), m_ptime.end());
  std::reverse(m_poked.begin(), m_poked.end());
}
    
//------------------------------------------------------------
// Procedure: printScript
// 

void TS_MOOSApp::printScript()
{
  unsigned int i, vsize = m_pairs.size();
  
  cout << "The Script: ========================================" << endl;
  cout << "Total Elements: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    string vdpair_str = m_pairs[i].getPrintable();
    double ptime  = m_ptime[i];
    bool   poked  = m_poked[i];

    cout << "[" << i << "] " << vdpair_str << ", TIME:" << ptime;
    cout << ", POSTED=" << boolToString(poked) << endl;
  }
  cout << "====================================================" << endl;
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
    // Condtions for posting: (1) enough elapsed time, (2) not already
    // poked, (3) poke time is not after reset-time if reset-time set.
    if((m_elapsed_time >= m_ptime[i]) && !m_poked[i] && 
       ((m_reset_time <= 0) || (m_ptime[i] <= m_reset_time))) {
      string variable = m_pairs[i].get_var();
      if(m_pairs[i].is_string()) {
	string sval = m_pairs[i].get_sdata();
	// Handle special case where MOOSDB_UPTIME *is* the post
	if(sval == "$$DBTIME")
	  m_Comms.Notify(variable, m_db_uptime);
	// Handle special case where MOOSDB_UPTIME is embedded in the post
	else if(strContains(sval, "$$DBTIME")) {
	  sval = findReplace(sval, "$$DBTIME", doubleToString(m_db_uptime, 2));
	  m_Comms.Notify(variable, sval);
	}
	// Handle special case where UTC_TIME *is* the post
	else if(sval == "$$UTCTIME")
	  m_Comms.Notify(variable, m_db_uptime);
	// Handle special case where UTC TIME is embedded in the post
	else if(strContains(sval, "$$UTCTIME")) {
	  sval = findReplace(sval, "$$UTCTIME", doubleToString(m_utc_time, 2));
	  m_Comms.Notify(variable, sval);
	}
	// Handle special case where COUNT *is* the post
	else if(sval == "$$COUNT")
	  m_Comms.Notify(variable, m_posted_tcount);
	// Handle special case where COUNT is embedded in the post
	else if(strContains(sval, "$$COUNT")) {
	  sval = findReplace(sval, "$$COUNT", intToString(m_posted_tcount));
	  m_Comms.Notify(variable, sval);
	}
	else
	  m_Comms.Notify(variable, sval);
      }
      else
	m_Comms.Notify(variable, m_pairs[i].get_ddata());
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
// Procedure: jumpToNextPosting()
//   Purpose: Go through the list of events and find the next un-poked
//            var-data pair and jump forward in time to the time at 
//            which the next unpoked var-data pair is ready.
//            The member variable "m_skip_time" is incremented to hold
//            the cumulative time jumped forward.

void TS_MOOSApp::jumpToNextPostingTime()
{
  double skip_amt = 0;
  unsigned int i, vsize = m_pairs.size();
  for(i=0; ((i<vsize)&&(skip_amt==0)); i++) {
    if(!m_poked[i])
      skip_amt = m_ptime[i] - m_elapsed_time;
  }
   
  m_skip_time += skip_amt;
}


//----------------------------------------------------------------
// Procedure: handleReset()
//   Purpose: Reset the script to how it was at the initial launch.
//            HOWEVER, the PAUSE state is not reset. 

void TS_MOOSApp::handleReset()
{
  // Check if the number of allowed resets has been reached. Unlimited
  // resets are indicated by (m_reset_max == -1).
  if((m_reset_max != -1) && (m_reset_count >= m_reset_max))
    return;

  m_reset_count++;

  m_elapsed_time  = 0;
  m_previous_time = -1;
  m_start_time    = 0;
  m_skip_time     = 0;
  m_pause_time    = 0;
  m_posted_count  = 0;

  // Mark all the posts as being unposted
  unsigned int i, vsize=m_poked.size();
  for(i=0; i<vsize; i++)
    m_poked[i] = false;
}
  
  
//----------------------------------------------------------------
// Procedure: postStatus

void TS_MOOSApp::postStatus()
{
  if((m_var_status == "") || (tolower(m_var_status) == "silent"))
    return;

  string status = "elapsed_time=" + doubleToString(m_elapsed_time,2);
  status += ", posted=" + intToString(m_posted_count);
  
  int pending = (m_pairs.size() - m_posted_count);
  status += ", pending=" + intToString(pending);

  status += ", paused=" + boolToString(m_paused);
  status += ", conditions_ok=" + boolToString(m_conditions_ok);
  
  string maxresets = "/any";
  if(m_reset_max != -1)
    maxresets = "/" + intToString(m_reset_max);
  status += ", resets=" + intToString(m_reset_count) + maxresets;
  
  m_Comms.Notify(m_var_status, status);
}
  
  
//----------------------------------------------------------------
// Procedure: seedRandom

void TS_MOOSApp::seedRandom()
{
  unsigned long tseed = time(NULL)+1;
#ifdef _WIN32
  unsigned long hostid = 0; 
	char hostname[256];
	if( gethostname(hostname, 256) == 0 ){
		hostent *host = gethostbyname(hostname);
		if(host != NULL){
			hostid = *(u_long *)host->h_addr_list[0];
		}
	}
	hostid += 1;
#else
  unsigned long hostid = gethostid()+1;
#endif
  unsigned long pid = (long)getpid()+1;
  unsigned long seed = (tseed%999999);
  seed = ((rand())*seed*hostid)%999999;
  seed = (seed*pid)%999999;
  srand(seed);
}


//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool TS_MOOSApp::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.m_sKey;
  string community = msg.m_sOriginatingCommunity;
  string sdata = msg.m_sVal;
  double ddata = msg.m_dfVal;
  char   mtype = msg.m_cDataType;

   if(mtype == MOOS_DOUBLE) {
    return(m_info_buffer->setValue(key, ddata));
  }
  else if(mtype == MOOS_STRING) {
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
// Procedure: addRandomVariable
//   Purpose: 

string TS_MOOSApp::addRandomVariable(string spec)
{
  string varname;
  string keyname;
  double minval=0;
  double maxval=1;
  bool   minval_set = false;
  bool   maxval_set = false;

  vector<string> svector = parseString(spec, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    if(left == "varname")
      varname = right;
    else if(left == "key")
      keyname = right;
    else if((left == "min") && isNumber(right)) {
      minval = atof(right.c_str());
      minval_set = true;
    }
    else if((left == "min") && isNumber(right)) {
      maxval = atof(right.c_str());
      maxval_set = true;
    }
    else 
      return("Bad parameter=value: " + left + "=" + right);
  }
  
  if(varname == "")
    return("Unset variable name");

  if(!minval_set)
    return("Lower value of the range not set");

  if(!maxval_set)
    return("Upper value of the range not set");
  
  if(minval > maxval)
    return("Minimum value greater than maximum value");

  unsigned int j, jsize = m_rand_vars.size();
  for(j=0; j<jsize; j++) {
    if(m_rand_vars[j].getVarName() == varname)
      return("Duplicate random variable");
  }
  
  RandomVariable rand_var;
  rand_var.setVarName(varname);
  if(keyname != "")
    rand_var.setKeyName(keyname);
  rand_var.setRange(minval, maxval);
  
  m_rand_vars.push_back(rand_var);
  return("");
}
  

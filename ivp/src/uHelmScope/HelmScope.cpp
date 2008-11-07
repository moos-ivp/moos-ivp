/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmScope.cpp                                        */
/*    DATE: Apr 12th 2008                                        */
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

#include <iostream>
#include <cstring>
#include "HelmScope.h"
#include "MBUtils.h"

#define BACKSPACE_ASCII 127
// number of seconds before checking for new moos vars (in all mode)
#define ALL_BLACKOUT 2 

using namespace std;

//------------------------------------------------------------
// Procedure: Constructor

HelmScope::HelmScope()
{    
  m_display_help       = false;
  m_paused             = true;
  m_display_truncate   = false;
  m_concise_bhv_list   = true;
  m_warning_count      = 0;

  // Default Settings for the Postings Report
  m_display_posts      = true;
  m_display_msgs_pc    = true;
  m_display_msgs_view  = true;
  m_display_msgs_pwt   = false;
  m_display_msgs_upd   = false;
  m_display_msgs_state = false;

  // Default Settings for the XMS Report
  m_display_xms        = true;
  m_display_virgins    = true;
  m_display_statevars  = true;
  // Do not ignore the variables specified in the .moos file
  // config block unless overridden from the command line
  m_ignore_filevars    = false;

  m_update_requested   = true;
  m_moosapp_iter       = 0;
  m_iteration_helm     = -1;
  m_iter_last_post     = -1;
  m_iter_next_post     = -1;

  // A history size of 0 indicates unlimited history kept
  m_history_size_max   = 2000;
  m_history_last_cut   = 0;
  
  m_db_uptime          = 0;
  m_max_time_loop      = 0;
  m_max_time_create    = 0;
  m_max_time_solve     = 0;

  m_last_iter_recd     = -1;
  m_last_iter_time     = 0;
  m_interval_samples_a = 5;
  m_interval_samples_b = 100;
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool HelmScope::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  // First scan the mail for the DB_UPTIME message to get an 
  // up-to-date value of DB uptime *before* handling other vars
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    if(msg.m_sKey == "DB_UPTIME")
      m_db_uptime = msg.m_dfVal;
  }
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    if(msg.m_sKey == "IVPHELM_DOMAIN")
      handleNewIvPDomain(msg.m_sVal);
    if(msg.m_sKey == "IVPHELM_SUMMARY")
      handleNewHelmSummary(msg.m_sVal);
    else if(msg.m_sKey == "IVPHELM_POSTINGS")
      handleNewHelmPostings(msg.m_sVal);
    else if(msg.m_sKey == "IVPHELM_STATEVARS")
      handleNewStateVars(msg.m_sVal);
  }

  // Update the values of all variables we have registered for.  
  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    updateVariable(msg);
  }

  handleNewIterXMS();
  return(true);
}


//------------------------------------------------------------
// Procedure: Iterate()

bool HelmScope::Iterate()
{
  if(m_display_help)
    printHelp();
  else
    printReport();

  // History pruning is done after report printing, and after new
  // helm info is received. Therefore no mutex should be needed
  pruneHistory();

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool HelmScope::OnConnectToServer()
{
    registerVariables();
    return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
//      do start up things here...
//      for example read from mission file...

bool HelmScope::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration("uHelmScope", sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = stripBlankEnds(toupper(sVarName));
    sLine    = stripBlankEnds(sLine);
    
    if(sVarName == "PAUSED")
      m_paused = (tolower(sLine) == "true");
    else if(sVarName == "HZ_MEMORY") {
      // Ensure that only two numbers are provided, both are greater
      // than zero and the second is greater than the first.
      vector<string> svector = parseString(sLine, ',');
      if(svector.size() == 2) { 
	svector[0] = stripBlankEnds(svector[0]);
	svector[1] = stripBlankEnds(svector[1]);
	if(isNumber(svector[0]) && isNumber(svector[1])) {
	  int a = atoi(svector[0].c_str());
	  int b = atoi(svector[1].c_str());
	  if((a >=1) && (b>a)) {
	    m_interval_samples_a = a;
	    m_interval_samples_b = b;
	  }
	}
      }
    }
    else if(sVarName == "DISPLAY_MOOS_SCOPE")
      m_display_xms = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_BHV_POSTS")
      m_display_posts = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_VIRGINS")
      m_display_virgins = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_STATEVARS")
      m_display_statevars = (tolower(sLine) == "true");
    else if(sVarName == "TRUNCATED_OUTPUT")
      m_display_truncate = (tolower(sLine) == "true");
    else if(sVarName == "BEHAVIORS_CONCISE")
      m_concise_bhv_list = (tolower(sLine) == "true");
    else if((sVarName == "VAR") && !m_ignore_filevars)
      addVariables(sLine, "user");
  }
    
  registerVariables();
    
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand
//   Purpose: Handle the console user's keyboard input.

void HelmScope::handleCommand(char c)
{
  switch(c) {
  case 'r':
  case 'R':
    m_paused = false;
    m_update_requested = true;
    break;
  case ' ':
    m_paused = true;
    m_iter_next_post = m_iteration_helm;
    m_update_requested = true;
    break;
  case 'h':
  case 'H':
    m_paused = true;
    m_display_help = true;
    break;
  case 'b':
  case 'B':
    m_concise_bhv_list = !m_concise_bhv_list;
    m_update_requested = true;
    break;
  case 's':
  case 'S':
    m_display_statevars = !m_display_statevars;
    m_update_requested = true;
    break;
  case 't':
  case 'T':
    m_display_truncate = !m_display_truncate;
    m_update_requested = true;
    break;
  case 'v':
  case 'V':
    m_display_virgins = !m_display_virgins;
    m_update_requested = true;
    break;
  case 'm':
    m_display_msgs_pwt = false;
    m_display_msgs_upd = false;
    m_display_msgs_state = false;
    m_update_requested = true;
    break;
  case 'M':
    m_display_msgs_pc   = false;
    m_display_msgs_view = false;
    m_update_requested  = true;
    break;
  case 'u':
  case 'U':
    m_display_msgs_pwt   = true;
    m_display_msgs_upd   = true;
    m_display_msgs_state = true;
    m_display_msgs_pc    = true;
    m_display_msgs_view  = true;
    m_update_requested   = true;
    break;
  case '[':
    m_paused = true;
    m_iter_next_post = m_iter_last_post - 1;
    m_update_requested = true;
    break;
  case ']':
    m_iter_next_post = m_iter_last_post + 1;
    m_update_requested = true;
    break;
  case '{':
    m_paused = true;
    m_iter_next_post = m_iter_last_post - 10;
    m_update_requested = true;
    break;
  case '}':
    m_iter_next_post = m_iter_last_post + 10;
    m_update_requested = true;
    break;
  case '(':
    m_paused = true;
    m_iter_next_post = m_iter_last_post - 100;
    m_update_requested = true;
    break;
  case ')':
    m_iter_next_post = m_iter_last_post + 100;
    m_update_requested = true;
    break;
  case '#':
    m_display_xms = !m_display_xms;
    m_update_requested = true;
    break;
  case '@':
    m_display_posts = !m_display_posts;
    m_update_requested = true;
    break;
  default:
    break;
  }
}
    
//------------------------------------------------------------
// Procedure: handleNewIvPDomain
//            [course,0,359,360] [speed,0,5,11]

void HelmScope::handleNewIvPDomain(const string& str)
{
  // This member variable stores the string representation of the
  // IvPDomain that will be displayed in the console output.
  m_ivpdomain = "";

  vector<string> svector = parseString(str, ':');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    m_ivpdomain += "[" + svector[i] + "] ";
  }
}


//------------------------------------------------------------
// Procedure: handleNewHelmSummary
//   Purpose: Parse the string that arrived in IVPHELM_SUMMARY.
//            This is a comma-separated list of var=value pairs.

void HelmScope::handleNewHelmSummary(const string& str)
{
  // Primary objective is to create and populate a new instance 
  // of IterBlockHelm and store it a map associated by the helm
  // index.
  IterBlockHelm hblock;

  string summary = stripBlankEnds(str);
  vector<string> svector = parseString(summary, ',');
  int vsize = svector.size();
  
  double time_loop   = 0;
  double time_create = 0;
  double time_solve  = 0;
  double time_utc    = 0;
  int    helm_iter   = 0;
  
  for(int i=0; i<vsize; i++) {
    vector<string> ivector = parseString(svector[i], '=');
    int isize = ivector.size();
    if(isize == 2) {
      string left  = stripBlankEnds(ivector[0]);
      string right = stripBlankEnds(ivector[1]);      
      if(left == "iter")
	helm_iter = atoi(right.c_str());
      else if(left == "ofnum")
	hblock.setCountIPF(atoi(right.c_str()));
      else if(left == "warnings")
	hblock.setWarnings(atoi(right.c_str()));
      else if(left == "utc_time")
	time_utc = atof(right.c_str());
      else if(left == "solve_time")
	time_solve = atof(right.c_str());
      else if(left == "create_time")
	time_create = atof(right.c_str());
      else if(left == "loop_time")
	time_loop = atof(right.c_str());
      else if(left == "var") {
	vector<string> pvector = parseString(right, ':');
	if(pvector.size() == 2) {
	  pvector[0] = stripBlankEnds(pvector[0]);
	  pvector[1] = stripBlankEnds(pvector[1]);
	  string var = pvector[0];
	  string val = pvector[1];
	  hblock.addDecVarVal(var, val);
	}
      }
      else if(left == "halted") {
	right = tolower(right);
	bool v = ((right=="true")||(right=="yes")||(right=="halted"));
	hblock.setHalted(v);
      }
      else if((left == "running_bhvs") && (right != "none")) 
	hblock.addRunningBHVs(right);
      else if((left == "active_bhvs")  && (right != "none"))
	hblock.addActiveBHVs(right);
      else if((left == "idle_bhvs")  && (right != "none"))
	hblock.addIdleBHVs(right);
      else if((left == "completed_bhvs") && (right != "none"))
	hblock.addCompletedBHVs(right);
    }
  }

  // 
  if(time_solve > m_max_time_solve) 
    m_max_time_solve = time_solve;
  if(time_loop > m_max_time_loop)
    m_max_time_loop = time_loop;
  if(time_create > m_max_time_create)
    m_max_time_create = time_create;

  hblock.setSolveTime(time_solve);
  hblock.setLoopTime(time_loop);
  hblock.setCreateTime(time_create);
  hblock.setIteration(helm_iter);
  hblock.setUTCTime(time_utc);

  // Begin Handle the interval time bookkeeping.
  // We store the helm time interval information across several
  // helm iterations. They are stored in one list of doubles. The
  // info provided by the helm is simply the time-stamp of the 
  // iteration. The actual interval is calculated here by comparing
  // agains the time stamp of the last iteration received.

  // Nothing to be done if this is the first helm iteration recd.
  if(m_last_iter_recd != -1) {
    // In case the index of the last iteration received is not 
    // simply one less than the current index. Unlikely. 
    int    iter_count   = helm_iter - m_last_iter_recd;
    double time_elapsed = time_utc  - m_last_iter_time;
    if(iter_count > 0) {
      double interval = time_elapsed / (double)(iter_count);
      m_helm_intervals.push_front(interval);
      int max_samples = m_interval_samples_a;
      if(m_interval_samples_b > max_samples)
	max_samples = m_interval_samples_b;
      if((int)(m_helm_intervals.size()) > max_samples)
	m_helm_intervals.pop_back();
    }
  }
  m_last_iter_recd = helm_iter;
  m_last_iter_time = time_utc;
  // End Handle the interval time bookkeeping

  if(helm_iter > m_iteration_helm)
    m_iteration_helm = helm_iter;

  // If not in paused mode, the currently received helm iteration
  // becomes the next iteration to post output.
  if(!m_paused)
    m_iter_next_post = helm_iter;
  
  // Finally, store the info for this iteration.
  m_blocks_helm[helm_iter] = hblock;
}

//------------------------------------------------------------
// Procedure: handleNewHelmPostings
//    
//    Note: This msg is generated in HelmIvP.cpp::postBehaviorMessages()
//    IVPHELM_POSTINGS="bhv_trail $@!$ 341 $@!$ foo=23 $@!$ bar=done"

void HelmScope::handleNewHelmPostings(const string& str)
{
  vector<string> svector = parseString(str, "$@!$");
  int i, vsize = svector.size();
  
  if(vsize < 2)
    return;

  string behavior  = stripBlankEnds(svector[0]);
  int    iteration = atoi(svector[1].c_str());
  
  if(iteration > m_iteration_helm)
    m_iteration_helm = iteration;
  
  map<int, IterBlockPosts>::iterator p;
  p = m_blocks_posts.find(iteration);
  if(p == m_blocks_posts.end()) {
    IterBlockPosts new_ibp;
    new_ibp.setIteration(iteration);
    m_blocks_posts[iteration] = new_ibp;
  }
  
  for(i=2; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    m_blocks_posts[iteration].addPosting(behavior, svector[i]);
  }
}

//------------------------------------------------------------
// Procedure: handleNewIterXMS
//    

void HelmScope::handleNewIterXMS()
{
  IterBlockXMS xblock;
  xblock.setIteration(m_iteration_helm);
  xblock.setVarNames(m_var_names);
  xblock.setVarVals(m_var_vals);
  xblock.setVarType(m_var_type);
  xblock.setVarSource(m_var_source);
  xblock.setVarTime(m_var_time);
  xblock.setVarCommunity(m_var_community);
  xblock.setVarCategory(m_var_category);
  
  m_blocks_xms[m_iteration_helm] = xblock;
}

//------------------------------------------------------------
// Procedure: handleNewStateVars
//    

void HelmScope::handleNewStateVars(const string& str)
{
  addVariables(str, "state");
}

//------------------------------------------------------------
// Procedure: registerVariables

void HelmScope::registerVariables()
{
  int vsize = m_var_names.size();
  for(int i=0; i<vsize; i++)
    m_Comms.Register(m_var_names[i], 0);

  m_Comms.Register("DB_UPTIME", 0);
  m_Comms.Register("IVPHELM_SUMMARY", 0);
  m_Comms.Register("IVPHELM_POSTINGS", 0);
  m_Comms.Register("IVPHELM_STATEVARS", 0);
  m_Comms.Register("IVPHELM_DOMAIN", 0);
}

//------------------------------------------------------------
// Procedure: pruneHistory()
//   Purpose: Prune the amount of stored information in the two
//            maps that keep info for each helm iteration.

void HelmScope::pruneHistory()
{  
  // When m_history_size is zero, this indicates that unlimited
  // history is kept. Potentially dangerous growth of memory, but
  // perhaps useful sometimes - so the user has this option. But
  // is not the default.
  if(m_history_size_max <= 0)
    return;

  // The size of the two maps *should* be the same since the helm
  // generates these two pieces of info once for each iteration. 
  // But since that is largely out the control of this MOOS App, 
  // we don't take it for granted. So we use a curr_history_size
  // that is the max of the two map sizes.
  int curr_history_size = m_blocks_helm.size();
  if((int)(m_blocks_posts.size()) > curr_history_size)
    curr_history_size = m_blocks_posts.size();
  
  // If the curr_history_size is within the bounds set by the 
  // user, nothing needs to be done - yet.
  if(curr_history_size <= m_history_size_max)
    return;
  
  // Determine the helm iteration K steps ago, where K is the
  // history_size_max set by the user. A reasonable assumption 
  // here is that there are K pieces of stored info in the two
  // maps during the last K iterations of the helm. If this 
  // assumption is wrong - unlikely - then perhaps less than 
  // the max amount of information is kept, which is a tolerable
  // state.
  int top_index = m_iteration_helm - m_history_size_max;

  // Now do the removals starting with the last index removed
  // on the previous call to this function. On the first call
  // to this function the iteration starts at zero.
  for(int i=m_history_last_cut; i<top_index; i++) {
    m_blocks_helm.erase(i);
    m_blocks_posts.erase(i);
  }
  
  // Keep track of where the prunine left off in this invocation
  // to help in the efficiency of the next invocation.
  m_history_last_cut = top_index;
}



//------------------------------------------------------------
// Procedure: addVariables

void HelmScope::addVariables(const string& line, const string& category)
{
  vector<string> svector = parseString(line, ',');
  int vsize = svector.size();
  
  for(int i=0; i<vsize; i++) {
    string var = stripBlankEnds(svector[i]);
    addVariable(var, category);
  }
}

//------------------------------------------------------------
// Procedure: addVariable

void HelmScope::addVariable(const string& varname, const string& category)
{
  // Check if the varname contains uHelmScope. Antler has the 
  // effect of artificially giving the process name as an 
  // argument to itself. This would have the effect of 
  // registering uXMS as variable to be scoped on. We assert 
  // here that we don't want that.
  if(strContains(varname, "uHelmScope"))
    return;
  
  // Return if the variable has already been added, but apply
  // the category.
  int vsize = m_var_names.size();
  for(int i=0; i<vsize; i++) {
    if(m_var_names[i] == varname) {
      if(category == "state") {
	if(strContains(m_var_category[i], "user"))
	  m_var_category[i] = "user-state";
	else
	  m_var_category[i] = "state";
      }
      else if(category == "user") {
	if(strContains(m_var_category[i], "state"))
	  m_var_category[i] = "user-state";
	else
	  m_var_category[i] = "user";
      }
      else
	m_var_category[i] = category;
      return;
    }
  }
  
  m_var_names.push_back(varname);
  m_var_vals.push_back("n/a");
  m_var_type.push_back("string");
  m_var_source.push_back(" n/a");
  m_var_time.push_back(" n/a");
  m_var_community.push_back(" n/a");
  m_var_category.push_back(category);

  // This may be redundant but no harm done
  m_Comms.Register(varname, 0);
}

//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void HelmScope::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.m_sKey;  
  double vtime   = m_db_uptime;

  string vtime_str = doubleToString(vtime, 2);
  vtime_str = dstringCompact(vtime_str);
  
  updateVarSource(varname, msg.m_sSrc);
  updateVarTime(varname, vtime_str);
  updateVarCommunity(varname, msg.m_sOriginatingCommunity);
  
  if(msg.m_cDataType == MOOS_STRING) {
    updateVarVal(varname, msg.m_sVal);
    updateVarType(varname, "string");
  }      
  else if(msg.m_cDataType == MOOS_DOUBLE) {
    updateVarVal(varname, doubleToString(msg.m_dfVal));
    updateVarType(varname, "double");
  }
  else if(msg.m_cDataType == MOOS_NOT_SET) {
    updateVarVal(varname, "n/a");
    updateVarSource(varname, "n/a");
    updateVarTime(varname, "n/a");
    updateVarCommunity(varname, "n/a");
    updateVarType(varname, "string");
  }
}

//------------------------------------------------------------
// Procedures - Update Functions

void HelmScope::updateVarVal(const string& varname, 
			     const string& val)
{
  for(unsigned int i=0; i<m_var_names.size(); i++) {
    if(m_var_names[i] == varname) {
      if(isNumber(val))
	m_var_vals[i] = dstringCompact(val);
      else
	m_var_vals[i] = val;
    }
  }
}

void HelmScope::updateVarType(const string& varname, 
			      const string& vtype)
{
  for(unsigned int i=0; i<m_var_names.size(); i++)
    if(m_var_names[i] == varname)
      m_var_type[i] = vtype;
}

void HelmScope::updateVarSource(const string& varname, 
				const string& vsource)
{
  for(unsigned int i=0; i<m_var_names.size(); i++)
    if(m_var_names[i] == varname)
      m_var_source[i] = vsource;
}

void HelmScope::updateVarTime(const string& varname, 
			      const string& vtime)
{
  for(unsigned int i=0; i<m_var_names.size(); i++)
    if(m_var_names[i] == varname)
      m_var_time[i] = vtime;
}

void HelmScope::updateVarCommunity(const string& varname, 
				   const string& vcommunity)
{
  for(unsigned int i=0; i<m_var_names.size(); i++)
    if(m_var_names[i] == varname)
      m_var_community[i] = vcommunity;
}

//------------------------------------------------------------
// Procedure: printHelp()

void HelmScope::printHelp()
{
  for(int j=0; j<2; j++)
    printf("\n");
  
  printf("KeyStroke  Function                                         \n");
  printf("---------  ---------------------------                      \n");
  printf("   Spc     Pause and Update information once - now          \n");
  printf("   r/R     Resume information refresh                       \n");
  printf("   h/H     Show this Help msg - 'r' to resume               \n");
  printf("   b/B     Toggle Show Idle/Completed Behavior Details      \n");
  printf("   t/T     Toggle truncation of column output               \n");
  printf("    m      Mask PWT_* UH_* STATE_* in Behavior-Posts Report \n");
  printf("    M      Mask PC_* VIEW_* in Behavior-Posts Report        \n");
  printf("   s/S     Toggle Behavior State Vars in MOOSDB-Scope Report\n");
  printf("   u/U     Unmask all variables in Behavior-Posts Report    \n");
  printf("   v/V     Toggle display of virgins in MOOSDB-Scope output \n");
  printf("   [/]     Display Iteration 1 step prev/forward            \n");
  printf("   {/}     Display Iteration 10 steps prev/forward          \n");
  printf("   (/)     Display Iteration 100 steps prev/forward         \n");
  printf("    #      Toggle Show the MOOSDB-Scope Report              \n");
  printf("    @      Toggle Show the Behavior-Posts Report            \n");
  printf("                                                            \n");
  printf("Hit 'r' to resume outputs, or SPACEBAR for a single update  \n");

  m_paused = true;
  m_display_help = false;
}

//------------------------------------------------------------
// Procedure: printReport()

void HelmScope::printReport()
{
  // If we're paused and no update was requested, plain and simple, 
  // do not print a new report.
  if(m_paused && !m_update_requested)
    return;

  if((m_iter_next_post == m_iter_last_post) && !m_update_requested)
    return;
  
  if((m_iter_next_post == -1) || (m_iter_next_post > m_iteration_helm))
    m_iter_next_post = m_iteration_helm;
  
  if(m_iter_next_post < m_history_last_cut)
    m_iter_next_post = m_history_last_cut;

  // We're willing to decare the update request satisfied as long
  //   as the index to be posted on this report is not -1. 
  // This is to ensure that if the scope comes up in paused mode, 
  //   it doesnt just come up displaying an empty set of records.
  if(m_iter_next_post == -1)
    m_iter_next_post = m_iteration_helm;
  if(m_iter_next_post != -1)
    m_update_requested = false;

  
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  printf("==============   uHelmScope Report  ============== (%d)\n",
	 m_moosapp_iter); 

  printHelmReport(m_iter_next_post);
  printf("\n\n");

  printf("#  MOOSDB-SCOPE ------------------------------------ ");
  printf("(Hit '#' to en/disable)\n");
  if(m_display_xms) {
    printDBReport(m_iter_next_post);
    printf("\n\n");
  }

  printf("@  BEHAVIOR-POSTS TO MOOSDB ----------------------- ");
  printf("(Hit '@' to en/disable)\n");
  if(m_display_posts)
    printPostingReport(m_iter_next_post);

  m_iter_last_post = m_iter_next_post;

  m_moosapp_iter++;
}


//------------------------------------------------------------
// Procedure: printHelmReport(int index)

void HelmScope::printHelmReport(int index)
{  
  unsigned int i, vsize;
  vector<string> svector, tvector;
  
  if(m_iteration_helm == -1) {
    printf("No IVPHELM_SUMMARY messages received. \n");
    return;
  }

  IterBlockHelm hblock = m_blocks_helm[index];

  int    iter = hblock.getIteration();
  int    ipfs = hblock.getCountIPF();

  double solve_time  = hblock.getSolveTime();
  double create_time = hblock.getCreateTime();
  double loop_time   = hblock.getLoopTime();
  //double utc_time    = hblock.getUTCTime();
  bool   halted      = hblock.getHalted();

  //--------------------------------------------
  double max_interval  = 0;
  double avg_samples_a = 0;
  double avg_samples_b = 0;
  double sum_samples_a = 0;
  double sum_samples_b = 0;
  int    num_samples_a = 0;
  int    num_samples_b = 0;
  int    count = 0;
  list<double>::iterator p;
  for(p=m_helm_intervals.begin(); p!=m_helm_intervals.end(); p++) {
    count++;
    double interval = *p;
    if(interval > max_interval)
      max_interval = interval;
    if(m_interval_samples_a >= count) {
      sum_samples_a += interval;
      num_samples_a++;
    }
    sum_samples_b += interval;
    num_samples_b++;
  }
  if(num_samples_a > 0)
    avg_samples_a = sum_samples_a / (double)(num_samples_a);
  if(num_samples_b > 0)
    avg_samples_b = sum_samples_b / (double)(num_samples_b);
  //---------------------------------------------

  printf("  Helm Iteration: %-5d ", iter);
  printf("  (hz=%.2f)(%d)", avg_samples_a, num_samples_a);
  printf("  (hz=%.2f)(%d)", avg_samples_b, num_samples_b);
  printf("  (hz=%.2f)(max)\n", max_interval);
  printf("  IvP functions:  %d\n", ipfs);
  printf("  SolveTime:      %.2f  ", solve_time);
  printf("  (max=%.2f)\n", m_max_time_solve);
  
  printf("  CreateTime:     %.2f  ", create_time);
  printf("  (max=%.2f)\n", m_max_time_create);
  
  printf("  LoopTime:       %.2f  ", loop_time);
  printf("  (max=%.2f)\n", m_max_time_loop);

  if(halted)
    printf("  Halted:         true    ");
  else
    printf("  Halted:         false   ");
  int wcnt = hblock.getWarnings();
  if(wcnt == 1)
    printf("(%d warning)\n", wcnt);
  else
    printf("(%d warnings)\n", wcnt);


  printf("Helm Decision: %s\n", m_ivpdomain.c_str());
  int vars = hblock.getDecVarCnt();
  for(int j=0; j<vars; j++) {
    string var = hblock.getDecVar(j);
    string val = hblock.getDecVal(j);
    printf("  %s = %s \n", var.c_str(), val.c_str());
  }

  svector = hblock.getActiveBHV();
  vsize = svector.size();
  printf("Behaviors Active: ---------- (%d)\n", vsize);
  for(i=0; i<vsize; i++) {
    printf("  %s\n", svector[i].c_str());
  }

  svector = hblock.getRunningBHV();
  vsize = svector.size();
  printf("Behaviors Running: --------- (%d)\n", vsize);
  for(i=0; i<vsize; i++)
    printf("  %s\n", svector[i].c_str());

  svector = hblock.getIdleBHV(m_concise_bhv_list);
  vsize = svector.size();
  printf("Behaviors Idle: ------------ (%d)\n", vsize);
  for(i=0; i<vsize; i++) {
    if((i==0) || (!m_concise_bhv_list))
      printf("  ");
    printf("%s", svector[i].c_str());
    if((m_concise_bhv_list) && (i < (vsize-1)))
      printf(", ");
    else
      printf("\n");
  }

  svector = hblock.getCompletedBHV(m_concise_bhv_list);
  vsize = svector.size();
  printf("Behaviors Completed: ------- (%d)\n", vsize);
  for(i=0; i<vsize; i++) {
    if((i==0) || (!m_concise_bhv_list))
      printf("  ");
    printf("%s", svector[i].c_str());
    if((m_concise_bhv_list) && (i < (vsize-1)))
      printf(", ");
    else
      printf("\n");
  }
}


//------------------------------------------------------------
// Procedure: printDBReport()

void HelmScope::printDBReport(int index)
{
  IterBlockXMS xblock;

  map<int, IterBlockXMS>::iterator p;

  int ix = index;
  p = m_blocks_xms.find(index);
  while((p == m_blocks_xms.end()) && (ix > 0)) {
    ix--;
    p = m_blocks_xms.find(ix);
  }

  if(p == m_blocks_xms.end()) {
    cout << "  --  No DB Report for " << index << endl;
    return;
  }
  else 
    xblock = p->second;
  
  vector<string> i_var_names  = xblock.getVarNames();
  vector<string> i_var_vals   = xblock.getVarVals();
  vector<string> i_var_source = xblock.getVarSource();
  vector<string> i_var_type   = xblock.getVarType();
  vector<string> i_var_time   = xblock.getVarTime();
  vector<string> i_var_community = xblock.getVarCommunity();
  vector<string> i_var_category = xblock.getVarCategory();

  printf("#\n");
  printf("#  %-18s", "VarName");
  
  printf("%-12s ", "Source");
  printf("%-9s", "Time");
  printf("%-11s", "Community");
  printf("VarValue\n");
  
  printf("#  %-18s", "----------------");
  printf("%-12s ", "-----------");
  printf("%-9s", "-------");
  printf("%-11s", "---------");
  printf("----------- \n");
  
  unsigned int vsize = m_var_names.size();
  for(unsigned int i=0; i<vsize; i++) {
    if((m_display_virgins || (i_var_vals[i]!="n/a")) &&
       (m_display_statevars || (strContains(i_var_category[i], "user")))) {

      // Output the Variable Name
      if(!strContains(i_var_category[i], "state"))
	printf("#  %-18s", i_var_names[i].c_str());
      else {
	string vname = i_var_names[i] + "*";
	printf("#  %-18s", vname.c_str());
      }
      
      // Output the Variable Source
      if(m_display_truncate) {
	string str = truncString(i_var_source[i], 12, "middle");
	printf("%-12s ", str.c_str());
      }
      else
	printf("%-12s ", i_var_source[i].c_str());
      
      // Output the Variable Time
      printf("%-9s", i_var_time[i].c_str());
      
      // Output the Variable Community
      if(m_display_truncate) {
	string str = truncString(i_var_community[i], 10);
	printf("%-11s", str.c_str());
      }
      else
	printf("%-11s", i_var_community[i].c_str());
      
      if(m_var_type[i] == "string") {
	if(m_var_vals[i] != "n/a") {
	  unsigned int max_len = 28;
	  if((m_display_truncate) && (i_var_vals[i].length() > max_len)) {
	    string str = truncString(i_var_vals[i], max_len);
	    printf("\"%s\"+", str.c_str());
	  }
	  else
	    printf("\"%s\"", i_var_vals[i].c_str());
	}
	else
	  printf("n/a");
      }
      else if(m_var_type[i] == "double")
	printf("%s", i_var_vals[i].c_str());
      printf("\n");		
    }
  }
}

//------------------------------------------------------------
// Procedure: printPostingReport(index)

void HelmScope::printPostingReport(int index)
{  
  printf("@\n");

  if(m_iteration_helm == -1) {
    printf("@  No IVPHELM_POSTINGS messages received. \n");
    return;
  }

  vector<string> postings;
  postings = m_blocks_posts[index].getPostings();
  unsigned int psize = postings.size();

  vector<string> behaviors;
  behaviors = m_blocks_posts[index].getBehaviors();

  printf("@  MOOS Variable     Value \n");
  
  string prev_behavior = "";
  for(unsigned int i=0; i<psize; i++) {
    bool ok_post = true;
    const char *pcstr = postings[i].c_str();
    if(!m_display_msgs_view && (!strncmp("VIEW_", pcstr, 5)))
      ok_post = false;
    else if(!m_display_msgs_state && (!strncmp("STATE_", pcstr, 6)))
      ok_post = false;
    else if(!m_display_msgs_pwt && (!strncmp("PWT_", pcstr, 4)))
      ok_post = false;
    else if(!m_display_msgs_upd && (!strncmp("UH_", pcstr, 3)))
      ok_post = false;
    else if(!m_display_msgs_pc &&  (!strncmp("PC_", pcstr, 3)))
      ok_post = false;
    
    if(ok_post) {
      if(behaviors[i] != prev_behavior)
	printf("@  -------------     -------  (BEHAVIOR=%s)\n", 
	       behaviors[i].c_str());
      prev_behavior = behaviors[i];
      vector<string> svector = chompString(postings[i].c_str(), '=');
      if(svector.size() == 2) {
	string var = stripBlankEnds(svector[0]);
	string val = stripBlankEnds(svector[1]);
	
	unsigned int var_max_len = 17;
	if((m_display_truncate) && (var.length() > var_max_len))
	  var = truncString(var, 17, "middle");
	printf("@  %-17s ", var.c_str());
	
	unsigned int val_max_len = 50;
	if((m_display_truncate) && (val.length() > val_max_len)) {
	  string str = truncString(val, val_max_len);
	  printf("%s +\n", str.c_str());
	}
	else
	  printf("%s\n",  val.c_str());
      }
    }
  }

  if(prev_behavior == "")
    printf("@  <empty>           <empty>\n"); 
}


#if 0
//------------------------------------------------------------
// Procedure: configureComms()

bool HelmScope::configureComms(string host, string port, 
			       int freq, string appname)
{
  m_sServerHost = host;
  m_sServerPort = port;
  m_lServerPort = atoi(port.c_str());
  //if(!CheckSetUp())
  //  return false;

  m_nCommsFreq = freq;

  //register a callback for On Connect
  m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect,this);

  //start the comms client....
  m_Comms.Run(m_sServerHost.c_str(),m_lServerPort,m_sAppName.c_str(),m_nCommsFreq);

  return(true);
}
#endif

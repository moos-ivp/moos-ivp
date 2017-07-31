/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmScope.cpp                                        */
/*    DATE: Apr 12th 2008                                        */
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

#include <iostream>
#include <cstring>
#include "HelmScope.h"
#include "MBUtils.h"
#include "HelmReportUtils.h"
#include "ColorParse.h"
#include "ACTable.h"

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Procedure: Constructor

HelmScope::HelmScope()
{ 
  m_display_mode       = "normal";

  m_paused             = true;
  m_display_truncate   = true;
  m_concise_bhv_list   = true;
  m_total_warning_cnt  = 0;

  // Default Settings for the Postings Report
  m_display_posts      = true;

  // Default Settings for the XMS Report
  m_display_xms        = true;
  m_display_virgins    = true;
  // Do not ignore the variables specified in the .moos file
  // config block unless overridden from the command line
  m_ignore_filevars    = false;

  m_update_pending     = true; 
  m_updates_throttled  = true;
  m_iteration_helm     = 0; 

  m_db_start_time      = 0; 
}

//------------------------------------------------------------
//  Proc: ConfigureComms
//  Note: Overload MOOSApp::ConfigureComms implementation which would
//        have grabbed port/host info from the .moos file instead

bool HelmScope::ConfigureComms()
{
  if((m_term_server_host == "") || (m_term_server_port == "")) 
    return(CMOOSApp::ConfigureComms());

  long lport = atol(m_term_server_port.c_str());
  CMOOSApp::SetServer(m_term_server_host.c_str(), lport);

  cout << "HelmScope::ConfigureComms(): Host/Port Configured Locally:" << endl;
  cout << "  m_sServerHost: " << m_sServerHost << endl;
  cout << "  m_lServErport: " << m_lServerPort << endl;

  //register a callback for On Connect
  m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect, this);
  
  //and one for the disconnect callback
  m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect, this);
  
  //start the comms client....
  if(m_sMOOSName.empty())
    m_sMOOSName = m_sAppName;
  
  m_nCommsFreq = 10;
  m_Comms.Run(m_sServerHost.c_str(),  m_lServerPort,
	      m_sMOOSName.c_str(),    m_nCommsFreq);
  
  return(true);
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool HelmScope::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  AppCastingMOOSApp::OnNewMail(NewMail);

  // First scan the mail for the DB_UPTIME message to get an 
  // up-to-date value of DB start time *before* handling other vars
  MOOSMSG_LIST::iterator p;
  if(m_db_start_time == 0) {
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      const CMOOSMsg &msg = *p;
      if(msg.GetKey() == "DB_UPTIME") {
	m_db_start_time = MOOSTime() - msg.GetDouble();
	m_community = msg.GetCommunity();
      }
    }
  }
  
  for(p = NewMail.begin(); p!=NewMail.end(); p++) { 
    CMOOSMsg &msg  = *p;
    string    key  = msg.GetKey();
    string    sval = msg.GetString();

    updateScopeEntries(msg);
    if(key == "IVPHELM_DOMAIN") 
      handleNewIvPDomain(sval); 
    else if(key == "IVPHELM_SUMMARY") 
      handleNewHelmSummary(sval); 
    else if(key == "IVPHELM_MODESET") 
      handleNewHelmModeSet(sval); 
    else if(key == "IVPHELM_STATEVARS") 
      addScopeVariables(sval); 
    else if(key == "IVPHELM_LIFE_EVENT") 
      m_life_event_history.addLifeEvent(sval);
    else if(key == "IVPHELM_STATE") 
      updateEngaged(sval);
    else if(key == "PHELMIVP_STATUS") 
      handleHelmStatusVar(sval);
  }

  return(true); 
}


//------------------------------------------------------------
// Procedure: Iterate()

bool HelmScope::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // If uHelmScope is just starting, in paused mode, give it a chance to 
  // read its mail before pausing. Treat first couple app iterations as if
  // an explicit update request is pending.
  if(m_iteration <= 2)
    m_update_pending = true;

  // Consider how long its been since our last report (of any kind)
  // May want to report less frequently if using a higher time warp.
  bool throttled = true;
  if(!m_updates_throttled) 
    throttled = false;
  else {
    double moos_elapsed_time = m_curr_time - m_last_report_time;
    double real_elapsed_time = moos_elapsed_time / m_time_warp;
    if(real_elapsed_time >= m_term_report_interval) {
      m_last_report_time = m_curr_time;
      throttled = false;
    }
  }

  // The throttling check (okto_report) is only applied in display modes
  // that support streaming, e.g., normal, life_events, warnings.

  if((m_display_mode == "help") && m_update_pending)
    printHelp();
  else if((m_display_mode == "modes") && m_update_pending)
    printModeSet();

  // Now apply the throttling criteria
  if(throttled)
    return(true);

  // If not throttled, display modes that support streaming
  if(m_display_mode == "warnings") {
    if(m_update_pending || !m_paused)
      printWarnings();
  }
  else if(m_display_mode == "life_events") {
    if(m_update_pending || !m_paused)
      printLifeEventHistory();
  }
  else if(m_display_mode == "normal") {
    if(m_update_pending || !m_paused)
      printReport();
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.

bool HelmScope::buildReport()
{
  return(false);
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
  AppCastingMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration("uHelmScope", sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = stripBlankEnds(toupper(sVarName));
    sLine    = stripBlankEnds(sLine);
    
    if(sVarName == "PAUSED") 
      m_paused = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_MOOS_SCOPE")
      m_display_xms = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_BHV_POSTS")
      m_display_posts = (tolower(sLine) == "true");
    else if(sVarName == "DISPLAY_VIRGINS")
      m_display_virgins = (tolower(sLine) == "true");
    else if(sVarName == "TRUNCATED_OUTPUT")
      m_display_truncate = (tolower(sLine) == "true");
    else if(sVarName == "BEHAVIORS_CONCISE")
      m_concise_bhv_list = (tolower(sLine) == "true");
    else if((sVarName == "VAR") && !m_ignore_filevars)
      addScopeVariables(sLine);
  }
    
  registerVariables();

  Notify("IVPHELM_REJOURNAL", 1);
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand
//   Purpose: Handle the console user's keyboard input.

void HelmScope::handleCommand(char c)
{
  switch(c) {
  case 'd':
  case 'D':
    m_update_pending = true;
    m_display_mode = "normal";
    break;
  case 'r':
    m_paused = false;
    m_update_pending = true;
    m_updates_throttled = true;
    break;
  case 'R':
    m_paused = false;
    m_update_pending = true;
    m_updates_throttled = false;
    break;
  case ' ':
    m_paused = true;
    m_update_pending = true;
    break;
  case 'h':
  case 'H':
    m_paused = true;
    m_display_mode = "help";
    m_update_pending = true;
    break;
  case 'l':
  case 'L':
    m_display_mode = "life_events";
    m_update_pending = true;
    break;
  case 'm':
  case 'M':
    m_paused = true;
    m_display_mode = "modes";
    m_update_pending = true;
    break;
  case 'w':
  case 'W':
    m_display_mode = "warnings";
    m_update_pending = true;
    break;
  case 'b':
  case 'B':
    m_concise_bhv_list = !m_concise_bhv_list;
    m_update_pending = true;
    break;
  case '`':
  case 't':
  case 'T':
    m_display_truncate = !m_display_truncate;
    m_update_pending = true;
    break;
  case 'v':
  case 'V':
    m_display_virgins = !m_display_virgins;
    m_update_pending = true;
    break;
  case '#':
    m_display_xms = !m_display_xms;
    m_update_pending = true;
    break;
  case '@':
    m_display_posts = !m_display_posts;
    m_update_pending = true;
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
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
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
  // Build off the previous helm report.
  HelmReport helm_report = string2HelmReport(str, m_helm_report);
  m_helm_report = helm_report;
}

//------------------------------------------------------------
// Procedure: handleNewHelmModeSet
//    
//    Note: This msg is generated in HelmIvP.cpp::OnStartUp()
//    IVPHELM_MODESET=
//       "---,ACTIVE#---,INACTIVE#ACTIVE,SURVEYING#ACTIVE,RETURNING"

void HelmScope::handleNewHelmModeSet(const string& str)
{
  //cout << "Handling New Helm MODESET: " << endl;
  //cout << "  " << str << endl;
  vector<string> svector = parseString(str, '#');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string parent = stripBlankEnds(biteString(svector[i], ','));
    string child  = stripBlankEnds(svector[i]);
    if(child == "") {
      StringTree new_tree;
      new_tree.setKey(parent);
      m_mode_trees.push_back(new_tree);
    }
    else {
      int index = m_mode_trees.size()-1;
      //cout << "[" << i << "] parent:" << parent << "  child:" << child << endl;
      m_mode_trees[index].addParChild(parent, child);
    }
  }
}

//------------------------------------------------------------
// Procedure: updateScopeEntries

void HelmScope::updateScopeEntries(const CMOOSMsg& msg)
{
  string varname = msg.GetKey();

  ScopeEntry entry;

  if(msg.IsString()) {
    entry.setType("string");
    entry.setValue(msg.GetString());
  }
  else if(msg.IsDouble()) {
    entry.setType("double");
    entry.setValue(doubleToStringX(msg.GetDouble()));
  }

  string srcaux = msg.GetSourceAux();
  if(srcaux != "") {
    entry.setSrcAux(msg.GetSourceAux());
    // Should be something like: "23:wpt_survey"
    vector<string> svector = parseString(srcaux, ':');
    unsigned int vsize = svector.size();
    if(vsize == 2) {
      entry.setIteration(svector[0]);
      entry.setNameBHV(svector[1]); 
      m_observed_behaviors.insert(svector[1]);
    }
  }      

  entry.setSource(msg.GetSource());
  entry.setTime(doubleToString((msg.GetTime()-m_db_start_time),2));
  entry.setCommunity(msg.GetCommunity());

  if(varname == "BHV_WARNING") {
    m_bhv_warnings.push_back(entry);
    if(m_bhv_warnings.size() > 40)
      m_bhv_warnings.pop_front();
  }

  m_map_posts[varname] = entry;

}

//------------------------------------------------------------
// Procedure: registerVariables

void HelmScope::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_UPTIME", 0);
  Register("IVPHELM_SUMMARY", 0);
  Register("IVPHELM_STATEVARS", 0);
  Register("IVPHELM_DOMAIN", 0);
  Register("IVPHELM_MODESET", 0);
  Register("IVPHELM_STATE", 0);
  Register("IVPHELM_LIFE_EVENT", 0);
  Register("PHELMIVP_STATUS", 0);
}

//------------------------------------------------------------
// Procedure: handleHelmStatusVar
//   Purpose: Examine the MOOSDB generated variable PHELMIVP_STATUS
//            for new publications by the Helm so we can register for
//            everything the Helm is publishing

void HelmScope::handleHelmStatusVar(const string& sval)
{  
  vector<string> svector = parseStringQ(sval, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    
    if(left == "Publishing") {
      string pub_vars = stripQuotes(right);
      vector<string> kvector = parseString(pub_vars, ',');
      unsigned int k, ksize = kvector.size();
      for(k=0; k<ksize; k++) {
	if(m_set_helmvars.count(kvector[k]) == 0) {
	  m_set_helmvars.insert(kvector[k]);
	  m_Comms.Register(kvector[k], 0);
	}
      }
    } 
  }
}

//------------------------------------------------------------
// Procedure: addScopeVariables

void HelmScope::addScopeVariables(const string& line)
{
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  
  for(i=0; i<vsize; i++) {
    string var = stripBlankEnds(svector[i]);
    addScopeVariable(var);
  }
}

//------------------------------------------------------------
// Procedure: addScopeVariable

void HelmScope::addScopeVariable(const string& varname)
{
  // Don't do anything if this variable has already been added.
  if(m_set_scopevars.count(varname) == 1)
    return;

  // Check if the varname contains uHelmScope. Antler has the 
  // effect of artificially giving the process name as an 
  // argument to itself. This would have the effect of 
  // registering uHelmScope as variable to be scoped on. We assert 
  // here that we don't want that.
  if(strContains(varname, "uHelmScope"))
    return;

  m_set_scopevars.insert(varname);

  cout << "Registering for: " << varname << endl;
  m_Comms.Register(varname, 0);

  // If not already an entry for this variable add empty placeholder
  if(m_map_posts.count(varname) == 0) {
    ScopeEntry entry;
    entry.setValue("n/a");
    entry.setType("string");
    entry.setSource(" n/a");
    entry.setSrcAux(" n/a");
    entry.setTime(" n/a");
    entry.setCommunity("");
    m_map_posts[varname] = entry;
  }
}

//------------------------------------------------------------
// Procedure: updateEngaged

void HelmScope::updateEngaged(const string& val)
{
  string eng_status = stripBlankEnds(val);
  if((eng_status == "DRIVE") || (eng_status == "PARK") ||
     (eng_status == "DISABLED")) {
    if(m_helm_engaged_primary != eng_status) {
      m_update_pending = true; 
      m_helm_engaged_primary = eng_status;
    }
  }
  else if((eng_status == "DRIVE+") || (eng_status == "PARK+") ||
	  (eng_status == "STANDBY")) {
    if(m_helm_engaged_standby != eng_status) {
      m_update_pending = true; 
      m_helm_engaged_standby = eng_status;
    }
  }
}

//------------------------------------------------------------
// Procedure: printHelp()

void HelmScope::printHelp()
{
  printf("\n\n");  
  printf("KeyStroke  Function                                         \n");
  printf("---------  ---------------------------                      \n");
  printf("Getting Help:                                               \n");
  printf("    h      Show this Help msg - 'r' to resume               \n");
  printf("                                                            \n");
  printf("Modifying the Refresh Mode:                                 \n");
  printf("   Spc     Refresh Mode: Pause (after updating once)        \n");
  printf("    r      Refresh Mode: Streaming (throttled)              \n");
  printf("    R      Refresh Mode: Streaming (unthrottled)            \n");
  printf("                                                            \n");
  printf("Modifying the Content Mode:                                 \n");
  printf("    d      Content Mode: Show normal reporting (default)    \n");
  printf("    w      Content Mode: Show behavior warnings             \n");
  printf("    l      Content Mode: Show life events                   \n");
  printf("    m      Content Mode: Show hierarchical mode structure   \n");
  printf("                                                            \n");
  printf("Modifying the Content Format or Filtering:                  \n");
  printf("    b      Toggle Show Idle/Completed Behavior Details      \n");
  printf("    `      Toggle truncation of column output               \n");
  printf("    v      Toggle display of virgins in MOOSDB-Scope output \n");
  printf("    #      Toggle Show the MOOSDB-Scope Report              \n");
  printf("    @      Toggle Show the Behavior-Posts Report            \n");
  printf("                                                            \n");
  printf("Hit 'r' to resume outputs, or SPACEBAR for a single update  \n");

  m_update_pending = false;
}

//------------------------------------------------------------
// Procedure: printModeSet()

void HelmScope::printModeSet()
{
  string modes = m_helm_report.getModeSummary();

  unsigned int j, lead_lines = 10;
  for(j=0; j<lead_lines; j++)
    printf("\n");
  printf("ModeSet Hierarchy: \n");
  printf("---------------------------------------------- \n");

  unsigned int t, tsize = m_mode_trees.size();

  for(t=0; t<tsize; t++) {
    vector<string> svector = m_mode_trees[t].getPrintableSet();
    unsigned int i, vsize = svector.size();
    if(vsize == 0)
      printf("Undefined ModeSet for this Helm Invocation   \n");
    for(i=0; i<vsize; i++) {
      printf("%s\n",  svector[i].c_str());
    }
    if(t < (tsize-1))
      printf("\n");
  }

  printf("---------------------------------------------- \n");
  printf("CURENT MODE(S): %s\n", modes.c_str());
  printf("                                                            \n");
  printf("Hit 'r' to resume outputs, or SPACEBAR for a single update  \n");

  m_update_pending = false;
  return;
}

//------------------------------------------------------------
// Procedure: printLifeEventHistory

void HelmScope::printLifeEventHistory()
{
  printf("\n\n\n\n\n\n\n\n\n\n");

  vector<string> history_lines = m_life_event_history.getReport();
  unsigned int i, vsize = history_lines.size();
  for(i=0; i<vsize; i++)
    printf("%s\n", history_lines[i].c_str());

  printf("                                                            \n");
  printf("Hit 'r' to resume outputs, or SPACEBAR for a single update  \n");

  m_update_pending = false;
  return;
}

//------------------------------------------------------------
// Procedure: printReport()

void HelmScope::printReport()
{
  string engaged_status = m_helm_engaged_standby;
  if(engaged_status != "")
    engaged_status += "/";
  engaged_status += m_helm_engaged_primary;
 
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  string community;
  community += termColor("reverseblue");
  community += "(" + m_community + ")" + termColor();
  printf("%s", community.c_str());

  string refresh_mode;
  if(m_paused) {
    refresh_mode += termColor("reversered");
    refresh_mode += "(PAUSED)" + termColor();
  }
  else {
    refresh_mode += termColor("reversegreen");
    refresh_mode += "(STREAMING)" + termColor();
  }
  printf("%s", refresh_mode.c_str());
       
  printf("===========   uHelmScope Report  ============= ");
  
  if((m_helm_engaged_standby == "DRIVE+") ||
     (m_helm_engaged_standby == "PARK+"))
    printf("%s", termColor("red").c_str());
  else
    printf("%s", termColor("green").c_str());    

  printf("%s ", engaged_status.c_str()); 
  printf("%s", termColor().c_str());
  printf("(%d)\n", m_iteration); 

  list<string> report_lines = m_helm_report.formattedSummary(m_curr_time,
							     m_concise_bhv_list);
  list<string>::const_iterator p;
  for(p=report_lines.begin(); p!=report_lines.end(); p++) {
    string line = *p;
    printf("%s\n", line.c_str());
  }
  
  printf("\n\n");

  printf("#  MOOSDB-SCOPE ------------------------------------ ");
  printf("(Hit '#' to en/disable)\n");
  if(m_display_xms) {
    printDBReport();
    printf("\n\n");
  }

  printf("@  BEHAVIOR-POSTS TO MOOSDB ----------------------- ");
  printf("(Hit '@' to en/disable)\n");
  if(m_display_posts)
    printPostingReport();

  m_update_pending = false;
}

//------------------------------------------------------------
// Procedure: printDBReport()

void HelmScope::printDBReport()
{
  printf("#\n");
  ACTable actab(5,2);
  actab.setLeftMargin("#  ");
  actab.setColumnJustify(2, "right");
  actab << "VarName | Source | Time | Commty | VarValue";
  if(m_display_truncate) {
    actab.setColumnMaxWidth(0, 18);
    actab.setColumnMaxWidth(1, 12);
    actab.setColumnMaxWidth(3, 10);
    actab.setColumnMaxWidth(4, 35);
  }
  actab.addHeaderLines(35);
    
  map<string, ScopeEntry>::const_iterator p;
  for(p=m_map_posts.begin(); p!=m_map_posts.end(); p++) {
    string varname = p->first;
    if(m_set_scopevars.count(varname) == 1) {
      string varval    = p->second.getValue();
      string vartype   = p->second.getType();
      string varsource = p->second.getSource();
      string vartime   = p->second.getTime();
      string community = p->second.getCommunity();
      if(m_display_virgins || (varval != "n/a")) {

	if((vartype == "string") && (varval != "n/a"))
	  varval = "\"" + varval + "\"";
	
	actab << varname << varsource << vartime << community << varval;
      }
    }
  }
  actab.print();
}

//------------------------------------------------------------
// Procedure: printPostingReport()

void HelmScope::printPostingReport()
{  
  
  printf("@\n");
  ACTable actab(4,2);
  actab.setLeftMargin("@  ");
  actab.setColumnJustify(2, "right");
  actab.setColumnNoPad(3);
  if(m_display_truncate) {
    actab.setColumnMaxWidth(0, 14);
    actab.setColumnMaxWidth(1, 12);
    actab.setColumnMaxWidth(3, 40);
  }

  actab << "MOOS Var | Behavior | Iter | Value";
  if(m_observed_behaviors.size() == 0)
    actab.addHeaderLines(40);
  
  set<string>::iterator p;  
  for(p=m_observed_behaviors.begin(); p!=m_observed_behaviors.end(); p++) {
 
    string bhv_name = *p;

    unsigned int bhv_count = 0;
    map<string, ScopeEntry>::const_iterator q;
    for(q=m_map_posts.begin(); q!=m_map_posts.end(); q++) {
      string var_name = q->first;
      string var_name_bhv  = q->second.getNameBHV();
      string var_iteration = q->second.getIteration();
      
      if((var_name_bhv == bhv_name) && (var_iteration != "")) {
	bhv_count++;
	if(bhv_count == 1) 
	  actab.addHeaderLines(40);

	string var_val    = q->second.getValue();
	//string var_type   = q->second.getType();
	//string var_source = q->second.getSource();
	//string var_time   = q->second.getTime();
	
	if(var_iteration == (uintToString(m_iteration_helm)))
	  var_iteration = ">" + var_iteration;
	else
	  var_iteration = " " + var_iteration;

	actab << var_name << var_name_bhv << var_iteration << var_val;
      }
    }
  }
  actab.print();
}

void HelmScope::printWarnings()
{  
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  string community;
  community += termColor("reverseblue");
  community += "(" + m_community + ")" + termColor();
  printf("%s", community.c_str());

  string refresh_mode;
  if(m_paused) {
    refresh_mode += termColor("reversered");
    refresh_mode += "(PAUSED)" + termColor();
  }
  else {
    refresh_mode += termColor("reversegreen");
    refresh_mode += "(STREAMING)" + termColor();
  }
  printf("%s", refresh_mode.c_str());
       
  printf("===============   uHelmScope Report (Warnings) ============= ");
  printf("(%d)\n", m_iteration); 

  ACTable actab(3,3);
  actab << "Behavior | Iter | Value";
  actab.addHeaderLines();
  if(m_display_truncate) {
    actab.setColumnMaxWidth(0,22);
    actab.setColumnMaxWidth(2,45);
  }

  list<ScopeEntry>::const_iterator q;
  for(q=m_bhv_warnings.begin(); q!=m_bhv_warnings.end(); q++) {
    ScopeEntry entry = *q;
    string var_name_bhv  = entry.getNameBHV();
    string var_iteration = entry.getIteration();
    
    if(var_iteration != "") {
      string var_val    = entry.getValue();
      if(var_iteration == (uintToString(m_iteration_helm)))
	var_iteration = ">" + var_iteration;
      else
	var_iteration = " " + var_iteration;

      actab << var_name_bhv << var_iteration << var_val;
    }
  }
  
  actab.print();
  if(m_bhv_warnings.size() == 0)
    printf(" No Behavior Warnings Noted \n");

  m_update_pending = false;
}









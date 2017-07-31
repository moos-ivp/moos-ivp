/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProcessWatch.cpp                                     */
/*    DATE: May 27th 2007 (MINUS-07)                             */
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
#include <iterator>
#include "ProcessWatch.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//------------------------------------------------------------
// Constructor

ProcessWatch::ProcessWatch()
{
  // m_watch_all_db true means all processes detected on incoming
  // db_clients mail will be added to the watch list (unless the
  // the process is on the exclude list)
  m_watch_all_db = true;

  // m_watch_all_antler true means all processes detected on the
  // antler configuration block in the mission file will be added
  // to the the watch list (unless the process is on the exclude list)
  m_watch_all_antler = true;

  // m_min_wait = -1 means proc_watch_summary postings will only
  // occur when the value of the posting changes. Change to a non-
  // negative value to allow subscribers to get a heartbeat
  // sense for the uProcessWatch process itself.
  m_min_wait = -1;

  // Number of seconds (realtime) since uProcessWatch start after 
  // which a process noted to be missing from the DBCLIENT list will
  // be noted to be gone. (Sometimes the process is still starting)
  m_noted_gone_wait = 10;

  // Allow retractions = -1 means always allow retractions. 
  // Otherwise retractions allowed only if elapsed_time is LESS than 
  // the m_allow_retractions time.
  m_allow_retractions = -1;

  // Indicates whether the summary (overall status) has changed 
  m_proc_watch_summary_changed = false;
  m_last_posting_time = 0;
}


//------------------------------------------------------------
// Procedure: OnNewMail

bool ProcessWatch::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string    key = msg.GetKey();
    
    if(key == "DB_CLIENTS") {
      m_db_clients = msg.GetString();
      handleMailNewDBClients();
    }
    else if(strEnds(key, "_STATUS"))
      handleMailStatusUpdate(msg.GetString());    
    else if(key == "EXITED_NORMALLY")
      m_excused_list.push_back(msg.GetString());
    else
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer

bool ProcessWatch::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void ProcessWatch::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_CLIENTS", 0);
  Register("EXITED_NORMALLY", 0);
}


//------------------------------------------------------------
// Procedure: Iterate

bool ProcessWatch::Iterate()
{
  AppCastingMOOSApp::Iterate();

  bool post_based_on_time = false;
  if(m_min_wait >= 0) {
    double moos_elapsed_time = m_curr_time - m_last_posting_time;
    double real_elapsed_time = moos_elapsed_time;
    if(m_time_warp > 0)
      real_elapsed_time = moos_elapsed_time / m_time_warp;
    if(real_elapsed_time > m_min_wait)
      post_based_on_time = true;
  }
  
  if(m_proc_watch_summary_changed || post_based_on_time) {
    AppCastingMOOSApp::Notify(postVar("PROC_WATCH_SUMMARY"), m_proc_watch_summary);
    m_last_posting_time = m_curr_time;
  }

  if(m_proc_watch_summary_changed) {
    checkForIndividualUpdates();
    postFullSummary();
    m_map_alive_prev = m_map_alive;
  }
  
  m_proc_watch_summary_changed = false;

  AppCastingMOOSApp::PostReport();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
  
bool ProcessWatch::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  Notify("PROC_WATCH_TIME_WARP", GetMOOSTimeWarp());

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::iterator p;
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string orig = *p;
    string line = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    if(param == "watch")
      handleConfigWatchList(value);
    else if(param == "nowatch") 
      handleConfigExcludeList(value);
    else if(param == "watch_all") {
      string lvalue = tolower(value);
      if(lvalue == "true") {
	m_watch_all_antler = true;
	m_watch_all_db = true;
      }
      else if(lvalue == "false") {
	m_watch_all_antler = false;
	m_watch_all_db     = false;
      }
      else if(lvalue == "antler") {
	m_watch_all_antler = true;
	m_watch_all_db     = false;
      }
      else if((lvalue == "dbclients") || (lvalue == "db_clients")) {
	m_watch_all_antler = false;
	m_watch_all_db     = true;
      }
      else 
	reportConfigWarning("Invalide WATCH_ALL value:" + value);
    }
    else if(param == "summary_wait") {
      if(isNumber(value))
	m_min_wait = atof(value.c_str());
      else
	reportConfigWarning("SUMMARY_WAIT must be numerical value");
    }
    else if(param == "allow_retractions") {
      if(tolower(value) == "true")
	m_allow_retractions = -1;
      else if(tolower(value) == "false")
	m_allow_retractions = 0;
      else if(isNumber(value))
	m_allow_retractions = atof(value.c_str());
      else
	reportConfigWarning("Param ALLOW_RETRACTIONS must be Boolean or numerial");
    }
    else if(param == "post_mapping")
      handlePostMapping(value);
    else 
      reportUnhandledConfigWarning(orig);
  }
  
  if(m_watch_all_antler == true)
    populateAntlerList();
  
  registerVariables();

  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport
//
//  Summary: All Present
//  Antler List: pHelmIvP, pLogger, uSimMarine, uFldShoreBroker
// 
//                                          Current   Max
//  ProcName    Watch Reason    Status      CPU Load  CPU Load
//  --------    ------------    ------      --------  --------                   
//  pHelmIvP    DB_CLIENTS      OK              0.12      0.33           
//  pLogger     DB_CLIENTS      OK             11.99     12.11            
//  uSimMarine  DB_CLIENTS      OK              9.08     14.94            

//  ProcName    Watch Reason    Status                                 
//  --------    ------------    ------                           
//  pHelmIvP    DB/WL/ANT       OK                               
//  pLogger     DB/ANT          OK                              
//  pApples     DB/ANT          OK(1)                              
//  pFooBar     ANT             MISSING

bool ProcessWatch::buildReport()
{
  m_msgs << "Summary: " << m_proc_watch_summary << endl << endl;

  // Part 1: Produce a list of apps named in the Antler List:
  // Antler List: pHelmIvP, pLogger, uSimMarine, uFldShoreBroker
  //              uFldNodeComms, pMarinePID, pHostInfo

  if(m_watch_all_antler == false) 
    m_msgs << "Antler List: (Configured with WATCH_ALL = false)" << endl;
  else {
    m_msgs << "Antler List: ";
    set<string>::iterator p;
    string alist;
    for(p=m_set_antler_clients.begin(); p!=m_set_antler_clients.end(); p++) {
      string client = *p;
      if(alist != "")
	alist += ",";
      alist += client;
      if(alist.length() > 43) {
	m_msgs << alist << endl;
	alist = "";
	m_msgs << "             ";
      }
    }
    if(alist != "") 
      m_msgs << alist << endl;
  }
  m_msgs << endl;


  ACTable actab(5,3);
  // In case we have an obscenely long MOOS app name.
  actab.setColumnMaxWidth(0,30);

  actab << "         |              |        | Current  | Max      ";
  actab << "ProcName | Watch Reason | Status | CPU Load | CPU Load ";
  actab.addHeaderLines();

  map<string, bool>::iterator p;
  for(p=m_map_alive.begin(); p!=m_map_alive.end(); p++) {
    string proc_name = p->first;
    bool   proc_here = p->second;

    actab << proc_name;

    string reason;
    if(m_watch_all_antler && m_set_antler_clients.count(proc_name))
      reason += "ANT ";
    else
      reason += "    ";

    if(processIncluded(proc_name))
      reason += "WATCH ";
    else
      reason += "      ";
    
    if(m_watch_all_db && m_set_db_clients.count(proc_name))
      reason += "DB";
    else
      reason += "  ";
    actab.addCell(reason);
    
    if(proc_here)
      actab.addCell("OK", "reversegreen");
    else {
      if(vectorContains(m_excused_list, proc_name))
	actab.addCell("EXCUSED", "reverseblue");
      else
	actab.addCell("MISSING", "reversered");
    }

    actab << doubleToString(m_map_now_cpuload[proc_name], 2);
    actab << doubleToString(m_map_max_cpuload[proc_name], 2);
  }
  m_msgs << actab.getFormattedString();

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailNewDBClients

void ProcessWatch::handleMailNewDBClients()
{
  // Phase 1: Possibly expand watchlist given current DB_CLIENTS
  if(m_watch_all_db) {
    vector<string> svector = parseString(m_db_clients, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      addToWatchList(svector[i]);
      m_set_db_clients.insert(svector[i]);
    }
  }

  // Phase 2: Check items in watchlist against current DB_CLIENTS
  string prev_summary = m_proc_watch_summary;
  m_proc_watch_summary = "All Present";  

  double connect_time = (m_curr_time - m_start_time) / m_time_warp;

  unsigned int j, jsize = m_watch_list.size();
  for(j=0; j<jsize; j++) {
    string proc = m_watch_list[j];
    bool alive = isAlive(proc);
    
    if(!alive) {
      if(connect_time > m_noted_gone_wait) {
	if(!vectorContains(m_excused_list, proc)) {
	  if(m_map_alive[proc])
	    procNotedGone(proc);
	  if(m_proc_watch_summary == "All Present")
	    m_proc_watch_summary = "AWOL: " + proc;
	  else
	    m_proc_watch_summary += "," + proc;
	}
	else { // proc IS excused and newly gone
	  if(m_map_alive[proc])
	    procNotedExcused(proc);
	}	  
      }
    }
    else {
      if(!m_map_alive[proc]) 
	procNotedHere(proc);
    }
  }

  if(prev_summary != m_proc_watch_summary)
    m_proc_watch_summary_changed = true;  
}

//------------------------------------------------------------
// Procedure: handleMailStatusUpdate

void ProcessWatch::handleMailStatusUpdate(string status)
{
  string procname = tokStringParse(status, "MOOSName", ',', '=');
  string cpuload  = tokStringParse(status, "cpuload",  ',', '=');
 
  if(procname == "") {
    reportRunWarning("Unhandled STATUS update: Missing MOOSName.");
    return;
  }

  if(cpuload == "") {
    reportRunWarning("Unhandled STATUS update: Missing cpuload for " + procname);
    return;
  }

  double d_cpuload = atof(cpuload.c_str());

  m_map_now_cpuload[procname] = d_cpuload;
  if(d_cpuload > m_map_max_cpuload[procname])
    m_map_max_cpuload[procname] = d_cpuload;
}

//-----------------------------------------------------------------
// Procedure: isAlive
//   Purpose: Check the given process name against the current list
//            of processes from DB_CLIENTS

bool ProcessWatch::isAlive(string procname)
{
  procname = stripBlankEnds(procname);

  vector<string> svector = parseString(m_db_clients, ',');
  unsigned int i, vsize = svector.size();

  for(i=0; i<vsize; i++)
    if(procname == stripBlankEnds(svector[i]))
      return(true);

  return(false);
}
  

//-----------------------------------------------------------------
// Procedure: handleConfigWatchList
//   Purpose: Allow for handling a comma-separated list of names
//            on a watchlist configuration

bool ProcessWatch::handleConfigWatchList(string pnames)
{
  vector<string> svector = parseString(pnames, ',');
  unsigned int i, vsize = svector.size();

  if(vsize == 0) {
    reportConfigWarning("Empty list provided to WATCH config param");
    return(false);
  }

  bool handled = true;
  for(i=0; i<vsize; i++) {
    bool ok = handleConfigWatchItem(svector[i]);
    if(!ok)
      handled = false;
  }
  return(handled);
}


//-----------------------------------------------------------------
// Procedure: handleConfigWatchItem
//      Note: A step toward defining the watch policy. 
//            A given procname may define a process or process prefix.
//            If it's not a prefix, the process will immediately be 
//              added to the watch list regardless of any other watch
//              policy setting. 
//            If it's a prefix, this is noted later when a potential
//              process is being considered for addition to the watch
//              list.


bool ProcessWatch::handleConfigWatchItem(string procname)
{
  procname = stripBlankEnds(procname);
  if((procname == "")  || strContainsWhite(procname)) {
    string msg = "Bad or Null procname on the WATCH list: [" + procname + "]";
    reportConfigWarning(msg);
    return(false);
  }

  bool prefix = false; 
  unsigned int len = procname.length();

  if((len > 0) && (procname.at(len-1) == '*')) {
    prefix = true;
    procname = procname.substr(0, len-1);
  }
  
  // Check to see if this process requests a dedicated posting
  // A procname of say "pFooBar:FOO_PRESENT" will request a separate
  // MOOS variable FOO_PRESENT report on the presence or absense
  // of the pFooBar process.
  string proc = biteStringX(procname, ':');
  string post = procname;
  procname = proc;
  if((!prefix) && (post != ""))
    m_map_proc_post[proc] = post;

  // Check to see if the process name is already present
  unsigned int i, vsize = m_include_list.size();
  for(i=0; i<vsize; i++) {
    if(m_include_list[i] == procname) {
      m_include_list_prefix[i] = m_include_list_prefix[i] || prefix;
      return(false);
    }
  }

  // If not - add the new item to the include list
  m_include_list.push_back(procname);
  m_include_list_prefix.push_back(prefix);

  // If prefix==false then this specifies an actual varname, not a
  // pattern, so varname should be added now to the watchlist
  if(!prefix) {
    bool added = addToWatchList(procname);
    if(added)
      procNotedHere(procname);
  }
  return(true);
}


//-----------------------------------------------------------------
// Procedure: handleConfigExcludeList
//   Purpose: Allow for handling a comma-separated list of names
//            on a watchlist configuration

bool ProcessWatch::handleConfigExcludeList(string pnames)
{
  vector<string> svector = parseString(pnames, ',');
  unsigned int i, vsize = svector.size();

  if(vsize == 0) {
    reportConfigWarning("Empty list provided to NOWATCH config param");
    return(false);
  }

  bool handled = true;
  for(i=0; i<vsize; i++) {
    bool ok = handleConfigExcludeItem(svector[i]);
    if(!ok) 
      handled = false;
  }
  return(handled);
}


//-----------------------------------------------------------------
// Procedure: handleConfigExcludeItem
//      Note: A step toward defining the watch policy. Only relevant 
//              when all processes are being watched by default. This is
//              a way to exclude certain processes from that default.
//      Note: A given procname may define a process or process prefix.
//      Note: If a process is both excluded AND included the inclusion
//            takes precedent. 

bool ProcessWatch::handleConfigExcludeItem(string procname)
{
  procname = stripBlankEnds(procname);
  if((procname == "")  || strContainsWhite(procname)) {
    string msg = "Bad or Null procname on NOWATCH list: [" + procname + "]";
    reportConfigWarning(msg);
    return(false);
  }

  bool prefix = false; 
  unsigned int len = procname.length();

  if((len > 0) && (procname.at(len-1) == '*')) {
    prefix = true;
    procname = procname.substr(0, len-1);
  }
  
  // Check to see if the process name is already present
  unsigned int i, vsize = m_exclude_list.size();
  for(i=0; i<vsize; i++) {
    if(m_exclude_list[i] == procname) {
      m_exclude_list_prefix[i] = m_exclude_list_prefix[i] || prefix;
      return(false);
    }
  }

  // If not - add the new item to the include list
  m_exclude_list.push_back(procname);
  m_exclude_list_prefix.push_back(prefix);
  return(true);
}


//-----------------------------------------------------------------
// Procedure: addToWatchList
//      Note: Given a proc name and the user config policy for whether 
//            or not a process should be on the watch list, potentially
//            add the process to the watch list.

bool ProcessWatch::addToWatchList(string procname)
{
  procname = stripBlankEnds(procname);
  if((procname == "") || (procname == "uProcessWatch"))
    return(false);

  // If already on the watch list, dont do anything
  if(vectorContains(m_watch_list, procname))
    return(false);

  // Is this process explicitly excluded?
  bool explicitly_excluded = processExcluded(procname);

  // Is this process explicitly included?
  bool explicitly_included = processIncluded(procname);

  if(explicitly_included || !explicitly_excluded) {
    m_watch_list.push_back(procname);

    Register(toupper(procname) + "_STATUS", 0);

    return(true);
  }
  return(false);
}


//-----------------------------------------------------------------
// Procedure: checkForIndividualUpdates
//      Note: 

void ProcessWatch::checkForIndividualUpdates()
{
  map<string, string>::iterator p;
  for(p=m_map_proc_post.begin(); p!=m_map_proc_post.end(); p++) {
    string proc  = p->first;
    string proc_post = p->second;

    bool   alive_prev = m_map_alive_prev[proc];
    bool   alive_curr = m_map_alive[proc];
    if(alive_prev != alive_curr)
      Notify(proc_post, boolToString(alive_curr));
  }
}

//-----------------------------------------------------------------
// Procedure: postFullSummary
//      Note: 

void ProcessWatch::postFullSummary()
{
  string full_summary;
  unsigned int i, vsize = m_watch_list.size();

  for(i=0; i<vsize; i++) {
    string proc = m_watch_list[i];
    unsigned int here_amt = m_map_noted_here[proc];
    unsigned int gone_amt = m_map_noted_gone[proc];

    string msg = proc + "(" + uintToString(here_amt) + "/";
    msg += uintToString(gone_amt) + ")";
    
    if(i > 0)
      full_summary += ",";
    full_summary += msg;
  }

  if(full_summary == "")
    full_summary = "No processes to watch";
  
  Notify(postVar("PROC_WATCH_FULL_SUMMARY"), full_summary);
}



//-----------------------------------------------------------------
// Procedure: procNotedHere

void ProcessWatch::procNotedHere(string procname)
{
  procname = stripBlankEnds(procname);
  
  m_map_noted_here[procname]++;
  m_map_alive[procname] = true;

  string msg;
  if(m_map_noted_here[procname] <= 1)
    msg += "Noted to be present: [" + procname + "]";
  else {
    msg += "Resurrected: [" + procname + "]";

    bool allow_this_retraction = false;
    if(m_allow_retractions < 0)
      allow_this_retraction = true;
    else {
      double moos_elapsed_time = m_curr_time - m_last_posting_time;
      double real_elapsed_time = moos_elapsed_time;
      if(m_time_warp > 0)
	real_elapsed_time = moos_elapsed_time / m_time_warp;
      if(real_elapsed_time <= m_allow_retractions) 
	allow_this_retraction = true;
    }
    
    // Retraction String must match exactly. See XYZ below.
    if(allow_this_retraction) 
      retractRunWarning("Process [" + procname + "] is missing.");
  }

  Notify(postVar("PROC_WATCH_EVENT"), msg);
  reportEvent(msg);
}

//-----------------------------------------------------------------
// Procedure: procNotedGone

void ProcessWatch::procNotedGone(string procname)
{
  procname = stripBlankEnds(procname);
  
  m_map_noted_gone[procname]++;
  m_map_alive[procname] = false;

  // Run Warning must match exactly when/if retracting. See XYZ above.
  string msg = "Process [" + procname + "] is missing.";
  Notify(postVar("PROC_WATCH_EVENT"), msg);

  reportEvent("PROC_WATCH_EVENT: " + msg);
  reportRunWarning(msg);
}

//-----------------------------------------------------------------
// Procedure: procNotedExcused

void ProcessWatch::procNotedExcused(string procname)
{
  procname = stripBlankEnds(procname);
  
  m_map_noted_gone[procname]++;
  m_map_alive[procname] = false;

  // Run Warning must match exactly when/if retracting. See XYZ above.
  string msg = "Process [" + procname + "] is gone but excused.";
  Notify(postVar("PROC_WATCH_EVENT"), msg);

  reportEvent("PROC_WATCH_EVENT: " + msg);
}


//-----------------------------------------------------------------
// Procedure: handlePostMapping

void ProcessWatch::handlePostMapping(string mapping)
{
  string left  = biteStringX(mapping, ',');
  string right = mapping;

  if(!strContainsWhite(right))
    m_map_chgpost[left] = right;
}

//-----------------------------------------------------------------
// Procedure: postVar

string ProcessWatch::postVar(string varname)
{
  map<string, string>::iterator p = m_map_chgpost.find(varname);
  if(p == m_map_chgpost.end())
    return(varname);
  else
    return(p->second);
}

//-----------------------------------------------------------------
// Procedure: populateAntlerList

void ProcessWatch::populateAntlerList()
{
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration("ANTLER", sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = tolower(biteStringX(sLine, '='));
    string value = stripBlankEnds(sLine);
    
    if(param == "run") {
      string app = biteStringX(value, '@');

      // Handle case where app is launched under an alias name with ~ alias
      biteStringX(value, '~');
      if(value != "")
	app = value;

      if((app != "") && (app != "uProcessWatch") && !strBegins(app, "MOOSDB")) {
	m_set_antler_clients.insert(app);
	bool added = addToWatchList(app);
	if(added)
	  procNotedHere(app);
      }
    }
  }
}

//-----------------------------------------------------------------
// Procedure: processExcluded
//   Purpose: Determine if this process is to be excluded based on 
//            NOWATCH params. Either by an explicit match or by a
//            pattern match.

bool ProcessWatch::processExcluded(const string& procname)
{
  bool excluded = false;
  unsigned int i, vsize = m_exclude_list.size();
  for(i=0; i<vsize; i++) {
    if(procname == m_exclude_list[i])
      excluded = true;
    else 
      if(m_exclude_list_prefix[i]  && 
	 strBegins(procname, m_exclude_list[i]))
	excluded = true;
  }
  return(excluded);
}

//-----------------------------------------------------------------
// Procedure: processIncluded
//   Purpose: Determine if this process is included based on the 
//            WATCH params. Either by an explicit match or by a
//            pattern match.

bool ProcessWatch::processIncluded(const string& procname)
{
  // Is this process explicitly included?
  bool included = false;
  unsigned int i, vsize = m_include_list.size();
  for(i=0; i<vsize; i++) {
    if(procname == m_include_list[i])
      included = true;
    else       
      if(m_include_list_prefix[i]  && 
	 strBegins(procname, m_include_list[i]))
	included = true;
  }
  return(included);
}
  









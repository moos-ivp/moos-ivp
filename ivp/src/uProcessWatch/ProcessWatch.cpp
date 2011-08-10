/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProcessWatch.cpp                                     */
/*    DATE: May 27th 2007 (MINUS-07)                             */
/*          Aug 7th  2011 (overhaul, Waterloo)                   */
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
#include <iterator>
#include "ProcessWatch.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

ProcessWatch::ProcessWatch()
{
  // m_watch_all true means all processes detected on incoming
  // db_clients mail will be added to the watch list (unless the
  // the process is on the exclude list)
  m_watch_all = true;

  // m_min_wait = -1 means proc_watch_summary postings will only
  // occur when the value of the posting changes. Change to a non-
  // negative value to allow subscribers to get a heartbeat
  // sense for the uProcessWatch process itself.
  m_min_wait  = -1;

  m_proc_watch_summary_changed = false;
  m_last_posting_time = 0;
}


//------------------------------------------------------------
// Procedure: OnNewMail

bool ProcessWatch::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.GetKey() == "DB_CLIENTS") {
      string new_db_clients = msg.GetString();
      if(new_db_clients != m_db_clients) {
	m_db_clients = new_db_clients;
	// Invoke on EACH piece of new mail in case a proc chgs
	// status twice in one iteration. Want to note that in
	// a proc_watch_event outgoing message.
	handleNewDBClients();
      }
    }
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer

bool ProcessWatch::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));

  m_Comms.Register("DB_CLIENTS", 0);
  return(true);
}

//------------------------------------------------------------
// Procedure: handleNewDBClients

void ProcessWatch::handleNewDBClients()
{
  // Phase 1: Possibly expand watchlist given current DB_CLIENTS
  vector<string> svector = parseString(m_db_clients, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++)
    addToWatchList(svector[i]);
  

  // Phase 2: Check items in watchlist against current DB_CLIENTS
  string prev_summary = m_proc_watch_summary;
  m_proc_watch_summary = "All Present";  
  vsize = m_watch_list.size();
  for(i=0; i<vsize; i++) {
    string proc = m_watch_list[i];
    bool alive = isAlive(proc);
    
    if(!alive) {
      if(m_map_alive[proc])
	procNotedGone(proc);

      if(m_proc_watch_summary == "All Present")
	m_proc_watch_summary = "AWOL: " + proc;
      else
	m_proc_watch_summary += "," + proc;
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
// Procedure: Iterate

bool ProcessWatch::Iterate()
{
  double current_time = MOOSTime();
  double elapsed_time = current_time - m_last_posting_time;

  bool post_based_on_time = false;
  if((m_min_wait >= 0) && (elapsed_time > m_min_wait))
    post_based_on_time = true;

  //cout << "min_wait:" << m_min_wait << endl;
  //cout << "elapsed:" << elapsed_time << endl;

  if(m_proc_watch_summary_changed || post_based_on_time) {
    m_Comms.Notify(postVar("PROC_WATCH_SUMMARY"), m_proc_watch_summary);
    MOOSTrace("PROC_WATCH_SUMMARY: %s\n", m_proc_watch_summary.c_str()); 
    m_last_posting_time = current_time;
  }

  if(m_proc_watch_summary_changed) {
    checkForIndividualUpdates();
    postFullSummary();
    m_map_alive_prev = m_map_alive;
  }
  
  m_proc_watch_summary_changed = false;

  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
  
bool ProcessWatch::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine = *p;
    string param = tolower(biteStringX(sLine, '='));
    string value = stripBlankEnds(sLine);
    
    if(param == "watch")
      augmentIncludeList(value);
    else if(param == "nowatch")
      augmentExcludeList(value);
    else if(param == "watch_all")
      setBooleanOnString(m_watch_all, value);
    else if((param == "summary_wait") && isNumber(value))
      m_min_wait = atof(value.c_str());
    else if(param == "post_mapping")
      handlePostMapping(value);
  }
  
  return(true);
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
// Procedure: augmentToIncludeList
//   Purpose: Allow for handling a comma-separated list of names
//            on a watchlist configuration

void ProcessWatch::augmentIncludeList(string pnames)
{
  vector<string> svector = parseString(pnames, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) 
    addToIncludeList(svector[i]);
}


//-----------------------------------------------------------------
// Procedure: addToIncludeList
//      Note: A step toward defining the watch policy. 
//            A given procname may define a process or process prefix.
//            If it's not a prefix, the process will immediately be 
//              added to the watch list regardless of any other watch
//              policy setting. 
//            If it's a prefix, this is noted later when a potential
//              process is being considered for addition to the watch
//              list.


void ProcessWatch::addToIncludeList(string procname)
{
  procname = stripBlankEnds(procname);
  if(procname == "")
    return;

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
      return;
    }
  }

  // If not - add the new item to the include list
  m_include_list.push_back(procname);
  m_include_list_prefix.push_back(prefix);

  // If prefix==false then this specifies an actual varname, not a
  // pattern, so varname should be added now to the watchlist
  if(!prefix)
    m_watch_list.push_back(procname);
}


//-----------------------------------------------------------------
// Procedure: augmentToExcludeList
//   Purpose: Allow for handling a comma-separated list of names
//            on a watchlist configuration

void ProcessWatch::augmentExcludeList(string pnames)
{
  vector<string> svector = parseString(pnames, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) 
    addToExcludeList(svector[i]);
}


//-----------------------------------------------------------------
// Procedure: addToExcludeList
//      Note: A step toward defining the watch policy. Only relevant 
//              when all processes are being watched by default. This is
//              a way to exclude certain processes from that default.
//      Note: A given procname may define a process or process prefix.
//      Note: If a process is both excluded AND included the inclusion
//            takes precedent. 

void ProcessWatch::addToExcludeList(string procname)
{
  procname = stripBlankEnds(procname);
  if(procname == "")
    return;

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
      return;
    }
  }

  // If not - add the new item to the include list
  m_exclude_list.push_back(procname);
  m_exclude_list_prefix.push_back(prefix);
}


//-----------------------------------------------------------------
// Procedure: addToWatchList
//      Note: Given a proc name and the user config policy for whether 
//            or not a process should be on the watch list, potentially
//            add the process to the watch list.

void ProcessWatch::addToWatchList(string procname)
{
  procname = stripBlankEnds(procname);
  if(procname == "")
    return;

  // If already on the watch list, dont do anything
  if(vectorContains(m_watch_list, procname))
    return;

  // Is this process explicitly excluded?
  bool explicitly_excluded = false;
  unsigned int i, vsize = m_exclude_list.size();
  for(i=0; i<vsize; i++) {
    if(procname == m_exclude_list[i])
      explicitly_excluded = true;
    else 
      if(m_exclude_list_prefix[i]  && 
	 strBegins(procname, m_exclude_list[i]))
	explicitly_excluded = true;
  }

  // Is this process explicitly included?
  bool explicitly_included = false;
  vsize = m_include_list.size();
  for(i=0; i<vsize; i++) {
    if(procname == m_include_list[i])
      explicitly_included = true;
    else       
      if(m_include_list_prefix[i]  && 
	 strBegins(procname, m_include_list[i]))
	explicitly_included = true;
  }

  bool add_to_list = false;
  if(explicitly_included)
    add_to_list = true;
  else 
    if(m_watch_all && !explicitly_excluded)
      add_to_list = true;

  if(add_to_list)
    m_watch_list.push_back(procname);

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
      m_Comms.Notify(proc_post, boolToString(alive_curr));
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
  
  m_Comms.Notify(postVar("PROC_WATCH_FULL_SUMMARY"), full_summary);
}



//-----------------------------------------------------------------
// Procedure: procNotedHere

void ProcessWatch::procNotedHere(string procname)
{
  procname = stripBlankEnds(procname);
  
  m_map_noted_here[procname]++;
  m_map_alive[procname] = true;

  string msg = "Process [" + procname + "]";
  if(m_map_noted_here[procname] <= 1)
    msg += " is noted to be present.";
  else
    msg += " is resurected!!!";

  m_Comms.Notify(postVar("PROC_WATCH_EVENT"), msg);
  MOOSTrace("PROC_WATCH_EVENT: %s\n", msg.c_str());
}

//-----------------------------------------------------------------
// Procedure: procNotedGone

void ProcessWatch::procNotedGone(string procname)
{
  procname = stripBlankEnds(procname);
  
  m_map_noted_gone[procname]++;
  m_map_alive[procname] = false;

  string msg = "Process [" + procname + "] has died!!!!";
  m_Comms.Notify(postVar("PROC_WATCH_EVENT"), msg);
  MOOSTrace("PROC_WATCH_EVENT: %s\n", msg.c_str()); 
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

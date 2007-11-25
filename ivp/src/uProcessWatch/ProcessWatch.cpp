/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ProcessWatch.cpp                                     */
/*    DATE: May 27th 2007 (MINUS-07)                             */
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
// Procedure: OnNewMail

bool ProcessWatch::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
	
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.m_sKey == "DB_CLIENTS") {
      m_db_clients = msg.m_sVal;
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
// Procedure: Iterate

bool ProcessWatch::Iterate()
{
  m_awol_procs = "All Present";
  
  int vsize = m_watchlist.size();
    
  for(int i=0; i<vsize; i++) {
    bool alive = isAlive(m_watchlist[i], m_prefix_match[i]);
    
    if(!alive) {
      if(m_isalive[i]) {
	string msg = "Process [" + m_watchlist[i] + "] has died!!!!";
	m_Comms.Notify("PROC_WATCH_EVENT", msg);
	MOOSTrace("PROC_WATCH_EVENT: %s\n", msg.c_str());
      }
      m_isalive[i] = false;

      if(m_awol_procs == "All Present")
	m_awol_procs = "AWOL: " + m_watchlist[i];
      else
	m_awol_procs += "," + m_watchlist[i];
    }
    else {
      if(!m_isalive[i]) {
	string msg = "Process [" + m_watchlist[i] + "] is resurected!!!";
	m_Comms.Notify("PROC_WATCH_EVENT", msg);
	MOOSTrace("PROC_WATCH_EVENT: %s\n", msg.c_str());
      }
      m_isalive[i] = true;
    }
  }

  m_Comms.Notify("PROC_WATCH_SUMMARY", m_awol_procs);

  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
  
bool ProcessWatch::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(sLine == "") {
      MOOSDebugWrite("null WATCH process");
      return(false);
    }
    
    if(MOOSStrCmp(sVarName, "WATCH")) {
      addToWatchList(sLine);
    }
  }
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: isAlive

bool ProcessWatch::isAlive(string g_procname, bool g_prefix_match)
{
  vector<string> svector = parseString(m_db_clients, ',');
  int vsize = svector.size();

  g_procname = stripBlankEnds(g_procname);

  for(int i=0; i<vsize; i++) {
    string i_procname = stripBlankEnds(svector[i]);
    if(g_procname == i_procname)
      return(true);
    if(g_prefix_match && strContains(i_procname, g_procname)) {
      return(true);
    }
  }
  return(false);
}
  

//-----------------------------------------------------------------
// Procedure: addToWatchList

void ProcessWatch::addToWatchList(string g_procname)
{
  string proc = stripBlankEnds(g_procname);
  bool prefix = false; 
  int  len    = proc.length();

  if((len > 0) && (proc.at(len-1) == '*')) {
    prefix = true;
    proc.at(len-1) = '\0';
  }
  
  // Check to see if the process name is already present
  int vsize = m_watchlist.size();
  for(int i=0; i<vsize; i++)
    if(m_watchlist[i] == proc)
      return;

  // If not - add the new item to the watch list
  m_watchlist.push_back(proc);
  m_prefix_match.push_back(prefix);
  m_isalive.push_back(false);
}

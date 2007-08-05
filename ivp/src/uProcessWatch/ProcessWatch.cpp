/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ProcessWatch.cpp                                     */
/*    DATE: May 27th 2007  (MINUS-07)                            */
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
  
  int vsize = m_procname.size();
    
  for(int i=0; i<vsize; i++) {
    bool alive = isAlive(m_procname[i]);
    
    if(!alive) {
      if(m_isalive[i]) {
	string msg = "Process [" + m_procname[i] + "] has died!!!!";
	m_Comms.Notify("PROC_WATCH_EVENT", msg);
	MOOSTrace("PROC_WATCH_EVENT: %s\n", msg.c_str());
      }
      m_isalive[i] = false;

      if(m_awol_procs == "All Present")
	m_awol_procs = "AWOL: " + m_procname[i];
      else
	m_awol_procs += "," + m_procname[i];
    }
    else {
      if(!m_isalive[i]) {
	string msg = "Process [" + m_procname[i] + "] is resurected!!!";
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
      m_procname.push_back(sLine);
      m_isalive.push_back(false);
      m_reported.push_back(false);
    }
  }
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: isAlive

bool ProcessWatch::isAlive(string g_procname)
{
  vector<string> svector = parseString(m_db_clients, ',');
  int vsize = svector.size();

  g_procname = stripBlankEnds(g_procname);

  for(int i=0; i<vsize; i++) {
    string i_procname = stripBlankEnds(svector[i]);
    if(g_procname == i_procname) {
      return(true);
    }
  }
   
  return(false);
}
  

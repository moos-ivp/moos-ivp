/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WatchDog.cpp                                         */
/*    DATE: December 9th 2004                                    */
/*****************************************************************/

#include <iterator>
#include "WatchDog.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Procedure: Constructor

WatchDog::WatchDog()
{
}

//------------------------------------------------------------
// Procedure: Destructor

WatchDog::~WatchDog()
{
}

//------------------------------------------------------------
// Procedure: OnNewMail

bool WatchDog::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
	
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.m_sKey == "DB_CLIENTS") {
      db_clients = msg.m_sVal;
    }
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer

bool WatchDog::OnConnectToServer()
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

bool WatchDog::Iterate()
{
  int vsize = m_procname.size();
  
  for(int i=0; i<vsize; i++) {
    bool alive = isAlive(m_procname[i]);

    if(!alive) {
      string msg = "Process [" + m_procname[i] + "] has died!!!!";
      m_Comms.Notify("WATCHDOG_SAYS", msg);
      m_isalive[i] = false;
    }
    else {
      if(m_isalive[i]==false) {
	string msg = "Process [" + m_procname[i] + "] is resurected!!!";
	m_Comms.Notify("WATCHDOG_SAYS", msg);
	m_isalive[i] = true;
      }
    }		     
    
  }
  
  
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
  
bool WatchDog::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "WATCH")) {
      bool ok = parseAddItem(sLine);
      if(!ok) {
	MOOSDebugWrite("Illegal item syntax(2)");
	return(false);
      }
    }
  }
    
  return(true);
}

//-----------------------------------------------------------------
// Procedure: parseAddItem(string)

bool WatchDog::parseAddItem(string item)
{
  item = stripBlankEnds(item);
  vector<string> svector = parseString(item, ',');
  if(svector.size() != 3) {
    MOOSDebugWrite("Illegal item syntax");
    return(false);
  }
  
  for(int i=0; i<svector.size(); i++)
    svector[i] = tolower(stripBlankEnds(svector[i]));
  
  // Handle New Process Name
  if(svector[0] == "") {
    MOOSDebugWrite("Illegal procname item syntax");
    return(false);
  }
  string procname = svector[0];
  
  // Handle New Variable Name
  if(svector[1] == "") {
    MOOSDebugWrite("Illegal varname item syntax");
    return(false);
  }
  string varname = svector[1];
  
  // Handle New Variable Value
  if(svector[2] == "") {
    MOOSDebugWrite("Illegal varname item syntax");
    return(false);
  }
  
  string vartype = "string";
  string svalue  = "";
  double dvalue  = 0;
  if(isNumber(svector[2])) {
    vartype = "double";
    dvalue = atof(svector[2].c_str());
  }
  else {
    svalue = stripBlankEnds(svector[2]);
    if(isQuoted(svalue))
      svalue = stripQuotes(svalue);
  }

  m_procname.push_back(procname);
  m_varname.push_back(varname);
  m_vartype.push_back(vartype);
  m_varsval.push_back(svalue);
  m_vardval.push_back(dvalue);
  m_isalive.push_back(true);

  return(true);
}



//-----------------------------------------------------------------
// Procedure: isAlive

bool WatchDog::isAlive(string g_procname)
{
  vector<string> svector = parseString(db_clients, ',');
  int vsize = svector.size();

  g_procname = stripBlankEnds(g_procname);

  for(int i=0; i<vsize; i++) {
    string i_procname = stripBlankEnds(svector[i]);
    if(g_procname == i_procname)
      return(true);
  }
   
  return(false);
}
  

/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XMS.cpp                                              */
/*    DATE: May 27th 2007                                        */
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
#include "XMS.h"
#include "MBUtils.h"

#define BACKSPACE_ASCII 127
// number of seconds before checking for new moos vars (in all mode)
#define ALL_BLACKOUT 2 

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Procedure: Constructor

XMS::XMS(string server_host, long int server_port)
{    
  m_display_source    = false;
  m_display_time      = false;
  m_display_community = false;
  m_display_help      = false;
  
  m_ignore_vars       = false;
  
  m_paused            = true;
  m_update_requested  = true;
  m_iteration         = 0;
  
  m_display_virgins   = true;
  m_display_empty_strings = true;
  m_db_start_time     = 0;
  
  m_filter            = "";
  
  m_display_all       = false;
  m_last_all_refresh  = 0;
  m_paused            = true;

  m_sServerHost       = server_host; 
  m_lServerPort       = server_port;

  m_configure_comms_locally = false;
}

//------------------------------------------------------------
//  Proc: ConfigureComms
//  Note: Overload MOOSApp::ConfigureComms implementation which would
//        have grabbed port/host info from the .moos file instead

bool XMS::ConfigureComms()
{
  cout << "XMS::ConfigureComms:" << endl;
  cout << "  m_sServerHost: " << m_sServerHost << endl;
  cout << "  m_lServErport: " << m_lServerPort << endl;

  if(!m_configure_comms_locally) 
    return(CMOOSApp::ConfigureComms());

  //cout << "**Doing things locally. " << endl;

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

bool XMS::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  MOOSMSG_LIST::reverse_iterator p;

  // First, if m_db_start_time is not set, scan the mail for the
  // DB_UPTIME mail from the MOOSDB and set. ONCE.
  if(m_db_start_time == 0) {
    for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
      CMOOSMsg &msg = *p;
      if(msg.m_sKey == "DB_UPTIME") 
	m_db_start_time = MOOSTime() - msg.m_dfVal;
    }
  }
  
  // Update the values of all variables we have registered for.  
  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    updateVariable(msg);
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: Iterate()

bool XMS::Iterate()
{
  // see if any new variables have been posted (running in show all mode)
  double moostime = MOOSTime();
  if(m_display_all && ((m_last_all_refresh + ALL_BLACKOUT) < moostime)) {
    refreshAllVarsList();
    m_last_all_refresh = moostime;
  }
  
  if(m_display_help)
    printHelp();
  else
    printReport();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool XMS::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
//      do start up things here...
//      for example read from mission file...

bool XMS::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  //m_MissionReader.GetConfiguration(GetAppName(), sParams);
  m_MissionReader.GetConfiguration("uXMS", sParams);

  // tes 2.21.08 - variables will be shown in the order they appear in the config file
  sParams.reverse();
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = stripBlankEnds(toupper(sVarName));
    sLine    = stripBlankEnds(sLine);
    
    if(sVarName == "PAUSED")
      m_paused = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_VIRGINS")
      m_display_virgins = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_EMPTY_STRINGS")
      m_display_empty_strings = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_SOURCE")
      m_display_source = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_TIME")
      m_display_time = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_COMMUNITY")
      m_display_community = (tolower(sLine) == "true");
    
    if(sVarName == "DISPLAY_ALL")
      m_display_all = (tolower(sLine) == "true");
    
    if(!(m_ignore_vars || m_display_all) && MOOSStrCmp(sVarName, "VAR"))
      addVariables(sLine);
  }
  
  // setup for display all
  if(m_display_all)
    refreshAllVarsList();
  else
    registerVariables();
  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand

void XMS::handleCommand(char c)
{
  switch(c) {
  case 's':
    m_display_source = false;
    m_update_requested = true;
    break;
  case 'S':
    m_display_source = true;
    m_update_requested = true;
    break;
  case 't':
    m_display_time = false;
    m_update_requested = true;
    break;
  case 'T':
    m_display_time = true;
    m_update_requested = true;
    break;
  case 'v':
    m_display_virgins = false;
    m_update_requested  = true;
    break;
  case 'V':
    m_display_virgins = true;
    m_update_requested = true;
    break;
  case 'e':
    m_display_empty_strings = false;
    m_update_requested  = true;
    break;
  case 'E':
    m_display_empty_strings = true;
    m_update_requested = true;
    break;
  case 'c':
    m_display_community = false;
    m_update_requested = true;
    break;
  case 'C':
    m_display_community = true;
    m_update_requested = true;
    break;
  case 'p':
  case 'P':
    m_paused = true;
    break;
  case 'r':
  case 'R':
    m_paused = false;
    break;
  case ' ':
    m_paused = true;
  case 'u':
  case 'U':
    m_update_requested = true;
    break;
  case 'h':
  case 'H':
    m_display_help = true;
    break;
    
    // turn filtering on
  case '/':
    m_paused = true;
    
    printf("%c", c);
    cin >> m_filter;
    
    m_update_requested = true;
    break;
    
    // clear filtering
  case '?':
    m_update_requested = true;
    m_filter = "";
    break;
    
    // turn show all variables mode on
  case 'A':
    m_display_all = true;
    m_update_requested = true;
    
    // save the original startup variable configuration
    // there may a more elegant way to do this
    orig_var_names = var_names;
    orig_var_vals = var_vals;
    orig_var_type = var_type;
    orig_var_source = var_source;
    orig_var_time = var_time;
    orig_var_community = var_community;
    
    break;
    
    // turn show all variables mode off
  case 'a':
    if(m_display_all) {
      m_display_all = false;
      m_update_requested = true;
      
      // restore the variable list at startup
      var_names = orig_var_names;
      var_vals = orig_var_vals;
      var_type = orig_var_type;
      var_source = orig_var_source;
      var_time = orig_var_time;
      var_community = orig_var_community;
    }
    
  default:
    break;
  }
}


//------------------------------------------------------------
// Procedure: addVariables

void XMS::addVariables(string line)
{
  line = stripBlankEnds(line);
  vector<string> svector = parseString(line, ',');
  int vsize = svector.size();
  
  for(int i=0; i<vsize; i++) {
    string var = stripBlankEnds(svector[i]);
    addVariable(var);
  }
}

//------------------------------------------------------------
// Procedure: addVariable

bool XMS::addVariable(string varname)
{
  // Check if the varname contains uXMS. Antler has the effect of
  // artificially giving the process name as an argument to itself.
  // This would have the effect of registering uXMS as variable to
  // be scoped on. We assert here that we don't want that.
  if(strContains(varname, "uXMS"))
    return(false);
  
  // Simply return false if the variable has already been added.
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++)
    if(var_names[i] == varname)
      return(false);
  
  var_names.push_back(varname);
  var_vals.push_back("n/a");
  var_type.push_back("string");
  var_source.push_back(" n/a");
  var_time.push_back(" n/a");
  var_community.push_back(" n/a");
  // added a new variable, return true
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables

void XMS::registerVariables()
{
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++) {
    m_Comms.Register(var_names[i], 0);
    // tes - commented out to minimize startup text (for low bandwidth)
    // cout << "Registering for: [" << var_names[i] << "]" << endl;
  }
  m_Comms.Register("DB_UPTIME", 0);
}

//------------------------------------------------------------
// Procedures - Update Functions

void XMS::updateVarVal(string varname, string val)
{
  if(isNumber(val))
    val = dstringCompact(val);
  for(unsigned int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_vals[i] = val;
}

void XMS::updateVarType(string varname, string vtype)
{
  for(unsigned int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_type[i] = vtype;
}

void XMS::updateVarSource(string varname, string vsource)
{
  for(unsigned int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_source[i] = vsource;
}

void XMS::updateVarTime(string varname, string vtime)
{
  for(unsigned int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_time[i] = vtime;
}

void XMS::updateVarCommunity(string varname, string vcommunity)
{
  for(unsigned int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_community[i] = vcommunity;
}

//------------------------------------------------------------
// Procedure: printHelp()

void XMS::printHelp()
{
  for(int j=0; j<2; j++)
    printf("\n");
  
  printf("KeyStroke    Function                             \n");
  printf("---------    ---------------------------          \n");
  printf("    s        Surpress Source of variables         \n");
  printf("    S        Display Source of variables          \n");
  printf("    t        Surpress Time of variables           \n");
  printf("    T        Display Time of variables            \n");
  printf("    c        Surpress Community of variables      \n");
  printf("    C        Display Community of variables       \n");
  printf("    v        Surpress virgin variables            \n");
  printf("    V        Display virgin variables             \n");
  printf("    e        Surpress empty strings               \n");
  printf("    E        Display empty strings                \n");
  printf("    /        Begin entering a filter string       \n");
  printf("    ?        Clear current filter                 \n");    
  printf("    a        Revert to variables shown at startup \n");
  printf("    A        Display all variables in the database\n");
  printf("   u/U       Update information once - now        \n");
  printf("   p/P       Pause information refresh            \n");
  printf("   r/R       resume information refresh           \n");
  printf("   h/H       Show this Help msg - 'R' to resume   \n");
  
  m_paused = true;
  m_display_help = false;
}

//------------------------------------------------------------
// Procedure: printReport()

void XMS::printReport()
{
  if(m_paused && !m_update_requested)
    return;
  m_update_requested = false;
  m_iteration++;
  
  for(int j=0; j<5; j++)
    printf("\n");
  
  printf("  %-22s", "VarName");
  
  if(m_display_source)
    printf("%-12s", "(S)ource");
  else
    printf(" (S) ");
  if(m_display_time)
    printf("%-12s", "(T)ime");
  else
    printf(" (T) ");
  if(m_display_community)
    printf("%-14s", "(C)ommunity");
  else
    printf(" (C) ");
  printf("VarValue\n");
  
  printf("  %-22s", "----------------");
  
  if(m_display_source)
    printf("%-12s", "----------");
  else
    printf(" --- ");
  
  if(m_display_time)
    printf("%-12s", "----------");
  else
    printf(" --- ");
  
  if(m_display_community)
    printf("%-14s", "----------");
  else
    printf(" --- ");
  printf(" ----------- (%d)\n", m_iteration);
  
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++) {
    
    if(((m_display_virgins) || (var_vals[i] != "n/a")) &&
       ((m_display_empty_strings) || (var_vals[i] != "")) &&
       MOOSStrCmp(m_filter, var_names[i].substr(0, m_filter.length()))) {
      
      printf("  %-22s ", var_names[i].c_str());
      
      if(m_display_source)
	printf("%-12s", var_source[i].c_str());
      else
	printf("     ");
      
      if(m_display_time)
	printf("%-12s", var_time[i].c_str());
      else
	printf("     ");
      
      if(m_display_community)
	printf("%-14s", var_community[i].c_str());
      else
	printf("     ");
      
      if(var_type[i] == "string") {
	if(var_vals[i] != "n/a") {
	  printf("\"%s\"", var_vals[i].c_str());
	}
	else
	  printf("n/a");
      }
      else if(var_type[i] == "double")
	printf("%s", var_vals[i].c_str());
      printf("\n");		
    }
  }
  
  // provide information for current filter(s)
  bool newline = false;
  if(m_filter != "") {
    printf("  -- filter: %s -- ", m_filter.c_str() );
    newline = true;
  }
  if(m_display_all) {
    printf("  -- displaying all variables --");
    newline = true;
  }
  if (newline)
    printf("\n");
}


//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void XMS::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.m_sKey;  
  double vtime   = msg.GetTime() - m_db_start_time;

  //double vtime = msg.GetTime() - GetAppStartTime();
  //vtime = MOOSTime() - GetAppStartTime();;
  //vtime = MOOSTime();
  
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
// Procedure: updateVariable
// tes 2.23.08
//
// finds all variables in the MOOS database and adds the ones
// we do not yet know about
// the mechanism and code for fetching all moos vars is
// closely related to that used for wildcard logging in pLogger
void XMS::refreshAllVarsList()
{
  MOOSMSG_LIST mail;
  if(m_Comms.ServerRequest("VAR_SUMMARY",mail)) {
    string ss(mail.begin()->GetString());
    while(!ss.empty()) {
      string sVar = MOOSChomp(ss);
      
      bool discard = false;
      
      // we assert here that we do not want _STATUS variables
      // displayed as part of all
      // perhaps we should make this a configuration option
      if (sVar.length() > 7) {
	if (MOOSStrCmp(sVar.substr(sVar.length()-7, 7), "_STATUS"))
	  discard = true;
      }
      
      if(!discard) {
	if(addVariable(sVar))
	  m_Comms.Register(sVar, 0);
      }   
    }
  }
}

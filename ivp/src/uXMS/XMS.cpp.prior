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

using namespace std;

//------------------------------------------------------------
// Procedure: Constructor

XMS::XMS()
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
  m_db_uptime         = 0;
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool XMS::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  // First scan the mail for the DB_UPTIME message to get an 
  // up-to-date value of DB uptime *before* handling other vars
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    if(msg.m_sKey == "DB_UPTIME")
      m_db_uptime = msg.m_dfVal;
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
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = stripBlankEnds(toupper(sVarName));
    sLine    = stripBlankEnds(sLine);
    int len  = sLine.length();
    
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

    if(!m_ignore_vars && MOOSStrCmp(sVarName, "VAR"))
      addVariables(sLine);
  }
  
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
  case 'u':
  case 'U':
    m_update_requested = true;
    break;
  case 'h':
  case 'H':
    m_display_help = true;
    break;
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

void XMS::addVariable(string varname)
{
  // Check if the varname contains uXMS. Antler has the effect of
  // artificially giving the process name as an argument to itself.
  // This would have the effect of registering uXMS as variable to
  // be scoped on. We assert here that we don't want that.
  if(strContains(varname, "uXMS"))
     return;

  // Simply return if the variable has already been added.
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++)
    if(var_names[i] == varname)
      return;

  var_names.push_back(varname);
  var_vals.push_back("n/a");
  var_type.push_back("string");
  var_source.push_back(" n/a");
  var_time.push_back(" n/a");
  var_community.push_back(" n/a");
}


//------------------------------------------------------------
// Procedure: registerVariables

void XMS::registerVariables()
{
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++) {
    m_Comms.Register(var_names[i], 0);
    cout << "Registering for: [" << var_names[i] << "]" << endl;
  }

  m_Comms.Register("DB_UPTIME", 0);
}

//------------------------------------------------------------
// Procedures - Update Functions

void XMS::updateVarVal(string varname, string val)
{
  if(isNumber(val))
    val = dstringCompact(val);
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_vals[i] = val;
}

void XMS::updateVarType(string varname, string vtype)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_type[i] = vtype;
}

void XMS::updateVarSource(string varname, string vsource)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_source[i] = vsource;
}

void XMS::updateVarTime(string varname, string vtime)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_time[i] = vtime;
}

void XMS::updateVarCommunity(string varname, string vcommunity)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_community[i] = vcommunity;
}

//------------------------------------------------------------
// Procedure: printHelp()

void XMS::printHelp()
{
  for(int j=0; j<2; j++)
    printf("\n");

  printf("KeyStroke    Function                            \n");
  printf("---------    ---------------------------         \n");
  printf("    s        Surpress Source of variables        \n");
  printf("    S        Display Source of variables         \n");
  printf("    t        Surpress Time of variables          \n");
  printf("    T        Display Time of variables           \n");
  printf("    c        Surpress Community of variables     \n");
  printf("    C        Display Community of variables      \n");
  printf("    v        Surpress virgin variables           \n");
  printf("    V        Display virgin variables            \n");
  printf("    e        Surpress empty strings              \n");
  printf("    E        Display empty strings               \n");
  printf("   u/U       Update information once - now       \n");
  printf("   p/P       Pause information refresh           \n");
  printf("   r/R       resume information refresh          \n");
  printf("   h/H       Show this Help msg - 'R' to resume  \n");
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
       ((m_display_empty_strings) || (var_vals[i] != ""))) {
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
}


//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void XMS::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.m_sKey;
  addVariable(varname);  // No action if already exists.
  
  double vtime = m_db_uptime;

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




/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TMS.cpp                                              */
/*    DATE: Mar 23rd 2006                                        */
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
#include "TMS.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Procedure: Constructor

TMS::TMS()
{    
  display_help      = false;
  display_source    = false;
  display_time      = false;
  display_community = false;
  display_virgins   = true;
  display_allvars   = false;
  display_allvars_toggle = false;
  display_allvars_pending_val = false;

  config_list_empty = true;
  paused            = false;
  limit_string      = false;
  limit_string_amt  = 50;
  iteration         = 0;
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool TMS::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  // Update the values of all variables we have registered for.  

  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.

  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    updateVariable(msg);
  }

  return(true);
}


//------------------------------------------------------------
// Procedure: Iterate()

bool TMS::Iterate()
{    
  iteration++;

  if(!display_allvars && config_list_empty)
    display_allvars_toggle = true;

  if(display_allvars_toggle) {
    display_allvars = display_allvars_pending_val;
    display_allvars_toggle = false;
  }

  if(display_allvars) {
    MOOSMSG_LIST::reverse_iterator p;
    MOOSMSG_LIST InMail;   
    if(m_Comms.ServerRequest("ALL",InMail)) {
      for(p = InMail.rbegin(); p != InMail.rend(); p++) {
	CMOOSMsg &msg = *p;
	updateVariable(msg);
      }
    }
  }

  if(display_help)
    printHelp();
  else
    printReport();

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool TMS::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp
//      do start up things here...
//      for example read from mission file...

bool TMS::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    int len  = sLine.length();
    
    if(MOOSStrCmp(sVarName, "VAR")) {
      addVariable(sLine, true);
    }
  }
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand

void TMS::handleCommand(char c)
{
  switch(c) {
  case 'a':
    display_allvars_toggle = true;
    display_allvars_pending_val = false;
    break;
  case 'A':
    display_allvars_toggle = true;
    display_allvars_pending_val = true;
    break;
  case 's':
    display_source = false;
    break;
  case 'S':
    display_source = true;
    break;
  case 't':
    display_time = false;
    break;
  case 'T':
    display_time = true;
    break;
  case 'c':
    display_community = false;
    break;
  case 'C':
    display_community = true;
    break;
  case 'v':
    display_virgins = false;
    break;
  case 'V':
    display_virgins = true;
    break;
  case 'r':
  case 'R':
  case 'g':
  case 'G':
    paused = false;
    display_help = false;
    break;
  case 'p':
  case 'P':
    paused = true;
    break;
  case 'h':
  case 'H':
    display_help = true;
    break;
  case 'l':
    limit_string = false;
    break;
  case 'L':
    limit_string = true;
    break;
  case '[':
    limit_string_amt -= 5;
    if(limit_string_amt < 20)
      limit_string_amt = 20;
    break;
  case ']':
    limit_string_amt += 5;
    if(limit_string_amt > 1500)
      limit_string_amt = 1500;
    break;
  default:
    break;
  } 
}


//------------------------------------------------------------
// Procedure: addVariable

void TMS::addVariable(string varname, bool config)
{
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++)
    if(var_names[i] == varname) {
      if(config)
	var_config[i] = true;
      return;
    }
  
  config_list_empty = config_list_empty || config;

  var_names.push_back(varname);
  var_config.push_back(config);
  var_vals.push_back("n/a");
  var_type.push_back("string");
  var_source.push_back(" n/a");
  var_time.push_back(" n/a");
  var_community.push_back(" n/a");
}

//------------------------------------------------------------
// Procedure: registerVariables

void TMS::registerVariables()
{
  int vsize = var_names.size();
  for(int i=0; i<vsize; i++)
    m_Comms.Register(var_names[i], 0);
}

//------------------------------------------------------------
// Procedures - Update Functions

void TMS::updateVarVal(string varname, string val)
{
  if(isNumber(val))
    val = dstringCompact(val);
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_vals[i] = val;
}

void TMS::updateVarConfig(string varname, bool bval)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_config[i] = bval;
}

void TMS::updateVarType(string varname, string vtype)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_type[i] = vtype;
}

void TMS::updateVarSource(string varname, string vsource)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_source[i] = vsource;
}

void TMS::updateVarTime(string varname, string vtime)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_time[i] = vtime;
}

void TMS::updateVarCommunity(string varname, string vcommunity)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == varname)
      var_community[i] = vcommunity;
}

//------------------------------------------------------------
// Procedure: printHelp()

void TMS::printHelp()
{
  if(paused)
    return;

  for(int j=0; j<2; j++)
    printf("\n");

  printf("KeyStroke    Function                            \n");
  printf("---------    ---------------------------         \n");
  printf("    a        Display subset variables            \n");
  printf("    A        Display all variables               \n");
  printf("    s        Surpress Source of variables        \n");
  printf("    S        Display Source of variables         \n");
  printf("    t        Surpress Time of variables          \n");
  printf("    T        Display Time of variables           \n");
  printf("    c        Surpress Community of variables     \n");
  printf("    C        Display Community of variables      \n");
  printf("    v        Surpress Virgin variables           \n");
  printf("    V        Display Virgin variables            \n");
  printf("   p/P       Pause information refresh           \n");
  printf("   r/R       resume information refresh          \n");
  printf("    l        Show Limited string length          \n");
  printf("    L        Show Unlimited string length        \n");
  printf("    [        Reduce string length limit by 5     \n");
  printf("    ]        Increase string length limit by 5   \n");
  printf("   h/H       Show this Help msg - 'R' to resume  \n");
  paused = true;
}

//------------------------------------------------------------
// Procedure: printReport()

void TMS::printReport()
{
  if(paused)
    return;

  for(int j=0; j<5; j++)
    printf("\n");

  printf("  %-22s", "VarName");

  if(display_source)
    printf("%-12s", "(S)ource");
  else
    printf(" (S) ");
  if(display_time)
    printf("%-12s", "(T)ime");
  else
    printf(" (T) ");
  if(display_community)
    printf("%-14s", "(C)ommunity");
  else
    printf(" (C) ");
  printf("VarValue\n");

  printf("  %-22s", "----------------");

  if(display_source)
    printf("%-12s", "----------");
  else
    printf(" --- ");

  if(display_time)
    printf("%-12s", "----------");
  else
    printf(" --- ");

  if(display_community)
    printf("%-14s", "----------");
  else
    printf(" --- ");
  printf(" ----------- (%d)\n", iteration);

  int vsize = var_names.size();
  for(int i=0; i<vsize; i++) {
    if(display_allvars || var_config[i]) {
      if(display_virgins || var_vals[i]!="n/a") {
	printf("  %-22s ", var_names[i].c_str());
	
	if(display_source)
	  printf("%-12s", var_source[i].c_str());
	else
	  printf("     ");
	
	if(display_time)
	  printf("%-12s", var_time[i].c_str());
	else
	  printf("     ");
	
	if(display_community)
	  printf("%-14s", var_community[i].c_str());
	else
	  printf("     ");
	
	if(var_type[i] == "string") {
	  if(var_vals[i] != "n/a") {
	    if(!limit_string) 
	      printf("\"%s\"", var_vals[i].c_str());
	    else {
	      char buff[1000];
	      strncpy(buff, var_vals[i].c_str(), limit_string_amt);
	      buff[limit_string_amt] = '\0';
	      printf("\"%s\"", buff);
	    }
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
}

//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void TMS::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.m_sKey;
  addVariable(varname);  // No action if already exists.

  //if(isConfigVar(varname))
  //  updateVarConfig(varname, false);

  double vtime = msg.GetTime() - GetAppStartTime();

  vtime = MOOSTime() - GetAppStartTime();;

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
// Procedure: isConfigVar()

bool TMS::isConfigVar(string str)
{
  for(int i=0; i<var_names.size(); i++)
    if(var_names[i] == str)
      return(var_config[i]);
  return(false);
}




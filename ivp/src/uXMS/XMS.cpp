/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XMS.cpp                                              */
/*    DATE: May 27th 2007                                        */
/*****************************************************************/

#include <iostream>
#include "XMS.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Procedure: Constructor

XMS::XMS()
{    
  display_source    = false;
  display_time      = false;
  display_community = false;
  display_help      = false;

  paused            = true;
  update_requested  = true;
  iteration         = 0;
}

//------------------------------------------------------------
// Procedure: OnNewMail()

bool XMS::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool XMS::Iterate()
{    
  if(display_help)
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
        
    if(MOOSStrCmp(sVarName, "VAR"))
      addVariable(sLine);
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
    display_source = false;
    update_requested = true;
    break;
  case 'S':
    display_source = true;
    update_requested = true;
    break;
  case 't':
    display_time = false;
    update_requested = true;
    break;
  case 'T':
    display_time = true;
    update_requested = true;
    break;
  case 'c':
    display_community = false;
    update_requested = true;
    break;
  case 'C':
    display_community = true;
    update_requested = true;
    break;
  case 'p':
  case 'P':
    paused = true;
    break;
  case 'r':
  case 'R':
    paused = false;
    break;
  case ' ':
  case 'u':
  case 'U':
    update_requested = true;
    break;
  case 'h':
  case 'H':
    display_help = true;
    break;
  default:
    break;
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
  printf("   u/U       Update information once - now       \n");
  printf("   p/P       Pause information refresh           \n");
  printf("   r/R       resume information refresh          \n");
  printf("   h/H       Show this Help msg - 'R' to resume  \n");
  paused = true;
  display_help = false;
}

//------------------------------------------------------------
// Procedure: printReport()

void XMS::printReport()
{
  if(paused && !update_requested)
    return;
  update_requested = false;
  iteration++;

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


//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void XMS::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.m_sKey;
  addVariable(varname);  // No action if already exists.
  
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



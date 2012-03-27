/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "ColorParse.h"

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
  m_display_source     = false;
  m_display_aux_source = false;
  m_display_time       = false;
  m_display_community  = false;
  m_display_own_community = true;
  m_display_help       = false;
  m_displayed_help     = false;
  
  m_ignore_file_vars   = false;
  
  m_update_requested  = true;
  m_scope_event       = true;
  m_history_event     = true;
  m_refresh_mode      = "events";


  m_iterations        = 0;
  m_term_report_interval = 0.6;   // Terminal report interval seconds
  m_time_warp         = 1;
  m_curr_time         = 0;
  m_last_report_time  = 0;      // Timestamp of last terminal report

  m_trunc_data        = 0;
  m_trunc_data_start  = 50;
  m_display_virgins   = true;
  m_display_null_strings = true;
  m_db_start_time     = 0;
  
  m_filter            = "";
  m_history_length    = 200;
  m_history_mode      = false;
  m_report_histvar    = true;
  
  m_display_all       = false;
  m_last_all_refresh  = 0;

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
  MOOSMSG_LIST::iterator p;

  // First, if m_db_start_time is not set, scan the mail for the
  // DB_UPTIME mail from the MOOSDB and set. ONCE.
  if(m_db_start_time == 0) {
    for(p = NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      if(msg.GetKey() == "DB_UPTIME") {
	m_db_start_time = MOOSTime() - msg.GetDouble();
	m_community = msg.GetCommunity();
	m_Comms.UnRegister("DB_UPTIME");
      }
    }
  }
  
  // Update the values of all variables we have registered for.  
  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.
  for(p = NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    if((key.length() >= 7) && (key.substr(key.length()-7,7) == "_STATUS")) {
      if(m_src_map[key] != "") {
	m_src_map[key] = msg.GetString();
      }
    }

    updateVariable(msg);
    if((key != "DB_UPTIME") && (key.length() > 7) &&
       (key.substr(key.length()-7, 7) != "_STATUS"))
      m_scope_event = true;
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: Iterate()

bool XMS::Iterate()
{
  // Part 1: Update the key local state variables.
  m_iterations++;
  m_curr_time = MOOSTime();

  // Part 2: If in the show-all mode, perhaps update new variables
  // Don't want to do this too often. And want to consider the timewarp.
  if(m_display_all) {
    double time_since_last_all_refresh = m_curr_time - m_last_all_refresh;
    if(m_time_warp > 0)
      time_since_last_all_refresh = time_since_last_all_refresh / m_time_warp;
    
    if(time_since_last_all_refresh > ALL_BLACKOUT) {
      refreshAllVarsList();
      cacheColorMap();
      m_last_all_refresh = m_curr_time;
    }
  }
  
  refreshProcVarsList();

  // Consider how long its been since our last report (regular or history)
  // Want to report less frequently if using a higher time warp.
  bool print_report = false;
  double moos_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = moos_elapsed_time / m_time_warp;

  if(real_elapsed_time >= m_term_report_interval) {
    m_last_report_time = m_curr_time;
    print_report = true;
  }

  if(m_display_help)
    printHelp();
  else if(m_history_mode && print_report)
    printHistoryReport();
  else if(print_report)
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
  STRING_LIST sParams;
  //m_MissionReader.GetConfiguration(GetAppName(), sParams);
  m_MissionReader.GetConfiguration("uXMS", sParams);

  // Variables shown in the order they appear in the config file
  sParams.reverse();
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string line  = *p;
    string param = biteString(line, '=');
    string value = stripQuotes(stripBlankEnds(line));
    param = stripBlankEnds(toupper(param));
    
    if(param == "REFRESH_MODE") {
      string str = tolower(value);
      if((str=="paused") || (str=="events") || (str=="streaming"))
	m_refresh_mode = str;
    }

    else if(param == "CONTENT_MODE") {
      string str = tolower(value);
      if(str=="scoping")
	m_history_mode = false;
      else if(str=="history")
	m_history_mode = true;
    }

    // Depricated PAUSED
    else if(param == "PAUSED") {
      string str = tolower(value);
      if(str == "true")
	m_refresh_mode = "paused";
	else
	m_refresh_mode = "events";
  }
  
    else if(param == "HISTORY_VAR") 
      setHistoryVar(value);

    else if(param == "DISPLAY_VIRGINS")
      m_display_virgins = (tolower(value) == "true");
    
    else if(param == "DISPLAY_EMPTY_STRINGS")
      m_display_null_strings = (tolower(value) == "true");
    
    else if(param == "DISPLAY_SOURCE")
      m_display_source = (tolower(value) == "true");
    
    else if(param == "DISPLAY_TIME")
      m_display_time = (tolower(value) == "true");
    
    else if(param == "DISPLAY_COMMUNITY")
      m_display_community = (tolower(value) == "true");
    
    else if(param == "DISPLAY_ALL")
      m_display_all = (tolower(value) == "true");
    
    else if(param == "COLORMAP") {
      string left  = stripBlankEnds(biteString(value, ','));
      string right = stripBlankEnds(value); 
      setColorMapping(left, right);
    }
    else if(param == "TERM_REPORT_INTERVAL")
      return(setTermReportInterval(value));

    else if(!(m_ignore_file_vars || m_display_all) && (param == "VAR"))
      addVariables(value);
  }
  
  // setup for display all
  if(m_display_all)
    refreshAllVarsList();
  else
    registerVariables();
  
  m_time_warp = GetMOOSTimeWarp();
  cacheColorMap();
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
  case 'x':
  case 'X':
    m_display_aux_source = !m_display_aux_source;
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
  case 'm':
  case 'M':
    m_display_own_community = !m_display_own_community;
    m_update_requested = true;
    break;
  case 'n':
    m_display_null_strings = false;
    m_update_requested  = true;
    break;
  case 'N':
    m_display_null_strings = true;
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
    m_refresh_mode = "paused";
    m_update_requested = true;
    break;
  case 'j':
    m_report_histvar = false;
    m_update_requested = true;
    break;
  case 'J':
    m_report_histvar = true;
    m_update_requested = true;
    break;
  case 'z':
    if(m_history_var != "") {
      m_update_requested = true;
      m_history_mode = !m_history_mode;
    }
    break;
  case 'w':
    if(m_history_var != "") {
      m_update_requested = true;
      m_history_mode = true;
    }
    break;
  case 'W':
    m_update_requested = true;
    m_history_mode = false;
    break;
  case 'r':
  case 'R':
    m_refresh_mode = "streaming";
    break;
  case 'e':
  case 'E':
    m_refresh_mode = "events";
    m_update_requested = true;
    break;
  case '<':
    if(m_history_length >= 10)
      m_history_length -= 5;
    else
      m_history_length = 5;
    m_update_requested = true;
    break;
  case '>':
    m_history_length += 5;
    if(m_history_length > 100)
      m_history_length = 100;
    m_update_requested = true;
    break;
  case '}':
    m_trunc_data       *= 1.2;
    m_trunc_data_start = m_trunc_data;
    m_update_requested = true;
    break;
  case '{':
    m_trunc_data       *= 0.8;
    if((m_trunc_data > 0) && (m_trunc_data < 15))
      m_trunc_data = 15;
    m_trunc_data_start = m_trunc_data;
    m_update_requested = true;
    break;
  case ' ':
    m_refresh_mode = "paused";
    m_update_requested = true;
    break;
  case 'u':
  case 'U':
    m_update_requested = true;
    break;
  case 'h':
  case 'H':
    if(!m_displayed_help)
      m_display_help = true;
    else
      m_update_requested = true;
    break;
  case '`':
    if(m_trunc_data == 0)
      m_trunc_data = m_trunc_data_start;
    else {
      m_trunc_data_start = m_trunc_data;
      m_trunc_data = 0;
    }
    m_update_requested = true;
    break;
    
    // turn filtering on
  case '/':
    m_refresh_mode = "paused";
    
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
    m_orig_var_names  = m_var_names;
    m_orig_var_vals   = m_var_vals;
    m_orig_var_type   = m_var_type;
    m_orig_var_source = m_var_source;
    m_orig_var_srcaux = m_var_srcaux;
    m_orig_var_time   = m_var_time;
    m_orig_var_community = m_var_community;
    m_orig_var_community = m_var_color;

    break;
    
    // turn show all variables mode off
  case 'a':
    if(m_display_all) {
      m_display_all = false;
      m_update_requested = true;
      
      // restore the variable list at startup
      m_var_names  = m_orig_var_names;
      m_var_vals   = m_orig_var_vals;
      m_var_type   = m_orig_var_type;
      m_var_source = m_orig_var_source;
      m_var_srcaux = m_orig_var_srcaux;
      m_var_time   = m_orig_var_time;
      m_var_community = m_orig_var_community;
      m_var_color  = m_orig_var_color;
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
  unsigned int i, vsize = svector.size();
  
  for(i=0; i<vsize; i++) {
    string var = stripBlankEnds(svector[i]);
    addVariable(var);
  }
}

//------------------------------------------------------------
// Procedure: addVariable
//      Note: Returns TRUE if this invocation results in a new 
//            variable added to the scope list.

bool XMS::addVariable(string varname, bool histvar)
{
  // Check if the varname contains uXMS. Antler has the effect of
  // artificially giving the process name as an argument to itself.
  // This would have the effect of registering uXMS as variable to
  // be scoped on. We assert here that we don't want that.
  if(strContains(varname, "uXMS") || strContainsWhite(varname))
    return(false);
  
  // Handle if this var is a "history" variable
  if(histvar || (m_var_names.size() == 0)) {
    m_history_var = varname;
    // If currently no variables scoped, hist_mode=true
    if(m_var_names.size() == 0)
      m_history_mode = true;
  }    

  // Simply return true if the variable has already been added.
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      return(false);
  
  m_var_names.push_back(varname);
  m_var_vals.push_back("n/a");
  m_var_type.push_back("string");
  m_var_source.push_back(" n/a");
  m_var_srcaux.push_back(" n/a");
  m_var_time.push_back(" n/a");
  m_var_community.push_back(" n/a");
  m_var_color.push_back("");
  
  // If not a history variable, then go back to being in scope
  // mode by default.
  if(!histvar)
    m_history_mode = false;

  return(true);
}

//------------------------------------------------------------
// Procedure: addSource

bool XMS::addSource(string source)
{
  source = toupper(source) + "_STATUS";
  m_src_map[source] = "empty";

  return(true);
}

//------------------------------------------------------------
// Procedure: setHistoryVar

void XMS::setHistoryVar(string varname)
{
  if(strContainsWhite(varname))
    return;

  // If there are currently no variables being scoped, put
  // into the history mode
  if(m_var_names.size() == 0)
    m_history_mode = true;

  m_history_var = varname;
}

//------------------------------------------------------------
// Procedure: setFilter

void XMS::setFilter(string str)
{
  if(strContainsWhite(str))
    return;

  if(m_filter != str)
    m_update_requested = true;
  
  if(str != "") {
    m_filter = str;
    m_display_all = true;
  }
  else {
    m_filter = "";
    m_display_all = false;
  }
}

//------------------------------------------------------------
// Procedure: setTruncData

void XMS::setTruncData(double v)
{
  m_trunc_data = vclip(v, 0, 1000);
  m_trunc_data_start = m_trunc_data;
}

//------------------------------------------------------------
// Procedure: setTermReportInterval

bool XMS::setTermReportInterval(string str)
{
  if(!isNumber(str))
    return(false);

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
  return(true);
}

//------------------------------------------------------------
// Procedure: setColorMapping

void XMS::setColorMapping(string str, string color)
{
  if((color!="red") && (color!="green") && (color!="blue") &&
     (color!="cyan") && (color!="magenta") && (color!="any"))
    return;

  if(color == "any") {
    if(!colorTaken("blue"))
      color = "blue";
    else if(!colorTaken("green"))
      color = "green";
    else if(!colorTaken("magenta"))
      color = "magenta";
    else if(!colorTaken("cyan"))
      color = "cyan";
    else if(!colorTaken("red"))
      color = "red";
    else
      return;
  }
  
  m_color_map[str] = color;
}

//------------------------------------------------------------
// Procedure: getColorMapping
//      Note: Returns a color mapping (if it exists) for the given
//            string. Typically the string is a MOOS variable name,
//            but it could also be a substring of a MOOS variable.

string XMS::getColorMapping(string substr)
{
  map<string, string>::iterator p;
  for(p=m_color_map.begin(); p!=m_color_map.end(); p++) {
    string str = p->first;
    if(strContains(substr, str))
      return(p->second);
  }
  return("");
}

//------------------------------------------------------------
// Procedure: colorTaken

bool XMS::colorTaken(std::string color)
{
  color = tolower(color);
  map<string, string>::iterator p;
  for(p=m_color_map.begin(); p!=m_color_map.end(); p++) {
    if(color == p->second)
      return(true);
  }
  return(false);
}

//------------------------------------------------------------
// Procedure: registerVariables

void XMS::registerVariables()
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    m_Comms.Register(m_var_names[i], 0);
  
  map<string, string>::iterator p;
  for(p=m_src_map.begin(); p!=m_src_map.end(); p++) {
    string reg_str = p->first;
    m_Comms.Register(reg_str, 1.0);
  }

  if(m_history_var != "")
    m_Comms.Register(m_history_var, 0);
  
  m_Comms.Register("DB_UPTIME", 0);
}

//------------------------------------------------------------
// Procedures - Update Functions

void XMS::updateVarVal(string varname, string val)
{
  if(isNumber(val))
    val = dstringCompact(val);

  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_vals[i] = val;
}

void XMS::updateVarType(string varname, string vtype)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_type[i] = vtype;
}

void XMS::updateVarSource(string varname, string vsource)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_source[i] = vsource;
}

void XMS::updateVarSourceAux(string varname, string vsource_aux)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_srcaux[i] = vsource_aux;
}

void XMS::updateVarTime(string varname, string vtime)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_time[i] = vtime;
}

void XMS::updateVarCommunity(string varname, string vcommunity)
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++)
    if(m_var_names[i] == varname)
      m_var_community[i] = vcommunity;
}

void XMS::updateHistory(string entry, string source, double htime)
{
  if(!m_history_list.empty() && !m_history_sources.empty() &&
     (entry  == m_history_list.front()) &&
     (source == m_history_sources.front())) {
    m_history_counts.front()++;
    m_history_times.front() = htime;
    return;
  }
  
  m_history_list.push_front(entry);
  m_history_counts.push_front(1);
  m_history_sources.push_front(source);
  m_history_times.push_front(htime);

  while(m_history_list.size() > m_history_length) {
    m_history_list.pop_back();
    m_history_counts.pop_back();
    m_history_sources.pop_back();
    m_history_times.pop_back();
  }
}

//------------------------------------------------------------
// Procedure: printHelp()

void XMS::printHelp()
{
  string refstr = termColor("reversered") + "HELP" + termColor();
  string mode_str = "(MODE = " + refstr + ")";   

  m_displayed_help = true;
  printf("\n\n");
  
  printf("KeyStroke    Function         %s      \n", mode_str.c_str());
  printf("---------    ---------------------------          \n");
  printf("    s        Suppress Source of variables         \n");
  printf("    S        Display Source of variables          \n");
  printf("    t        Suppress Time of variables           \n");
  printf("    T        Display Time of variables            \n");
  printf("    c        Suppress Community of variables      \n");
  printf("    C        Display Community of variables       \n");
  printf("    v        Suppress virgin variables            \n");
  printf("    V        Display virgin variables             \n");
  printf("   m/M       Toggle display own community at top  \n");
  printf("    n        Suppress null/empty strings          \n");
  printf("    N        Display null/empty strings           \n");
  printf("    w        Show Variable History if enabled     \n");
  printf("    W        Hide Variable History                \n");
  printf("    x        Show Auxilliary Source if non-empty  \n");
  printf("    X        Hide Auxilliary Source               \n");
  printf("   z/Z       Toggle the Variable History mode     \n");
  printf("    j        Truncate Hist Variable in Hist Report\n");
  printf("    J        Show Hist Variable in Hist Report    \n");
  printf("  > or <     Show More or Less Variable History   \n");
  printf("    /        Begin entering a filter string       \n");
  printf("    `        Toggle Data Field truncation         \n");
  printf("    ?        Clear current filter                 \n");    
  printf("    a        Revert to variables shown at startup \n");
  printf("    A        Display all variables in the database\n");
  printf(" u/U/SPC     Update infor once-now, then Pause    \n");
  printf("   p/P       Pause information refresh            \n");
  printf("   r/R       Resume information refresh           \n");
  printf("   e/E       Information refresh is event-driven  \n");
  printf("   h/H       Show this Help msg - 'R' to resume   \n");
  
  m_refresh_mode = "paused";
  m_display_help = false;
}

//------------------------------------------------------------
// Procedure: printReport()

void XMS::printReport()
{
  string refstr = toupper(m_refresh_mode) + termColor();
  if(m_refresh_mode == "paused") 
    refstr = termColor("reversered") + refstr;
  else
    refstr = termColor("reversegreen") + refstr;
  string mode_str = "(MODE = SCOPE:" + refstr + ")";   

  bool do_the_report = false;
  if((m_refresh_mode == "paused") && m_update_requested)
    do_the_report = true;
  if(m_refresh_mode == "streaming")
    do_the_report = true;
  if((m_refresh_mode == "events") && 
     (m_scope_event || m_update_requested))
    do_the_report = true;

  if(!do_the_report)
    return;

  m_displayed_help = false;

  m_scope_event = false;
  m_update_requested = false;
  
  printf("\n\n\n\n\n");
  
  string varname_str = "VarName";
  if(m_display_own_community && (m_community != "")) {
    varname_str += termColor("reverseblue");
    varname_str += "(" +  m_community + ")" + termColor();
    printf("  %-33s", varname_str.c_str());
  }
  else
    printf("  %-22s", varname_str.c_str());

  
  if(m_display_source)
    printf("%-16s", "(S)ource");
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
  printf("VarValue %s\n", mode_str.c_str());
  
  printf("  %-22s", "----------------");
  
  if(m_display_source)
    printf("%-16s", "----------");
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
  printf(" ----------- (%d)\n", m_iterations);
  
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) {
    
    if(((m_display_virgins) || (m_var_vals[i] != "n/a")) &&
       ((m_display_null_strings) || (m_var_vals[i] != "")) &&
       MOOSStrCmp(m_filter, m_var_names[i].substr(0, m_filter.length()))) {
      
      string vcolor = m_var_color[i];
      if(vcolor != "")
	printf("%s", termColor(vcolor).c_str());
      
      printf("  %-22s ", m_var_names[i].c_str());

      if(m_display_source) {
	if((!m_display_aux_source) || (m_var_srcaux[i]=="")) {
	  string sstr = truncString(m_var_source[i], 14, "middle");
	  printf("%-16s", sstr.c_str());
	}
	else {
	  string sstr = truncString(m_var_srcaux[i], 14, "middle");
	  sstr = "[" + sstr + "]";
	  printf("%-16s", sstr.c_str());
	}
      }
      else
	printf("     ");
      
      if(m_display_time)
	printf("%-12s", m_var_time[i].c_str());
      else
	printf("     ");
      
      if(m_display_community) {
	string comm_str = truncString(m_var_community[i], 14, "middle");
	printf("%-14s", comm_str.c_str());
      }
      else
	printf("     ");
      
      if(m_var_type[i] == "string") {
	if(m_var_vals[i] != "n/a") {
	  if(m_trunc_data > 0) {
	    string tstr = truncString(m_var_vals[i], m_trunc_data);
	    if(tstr.length() < m_var_vals[i].length())
	      printf("\"%s...\"", tstr.c_str());
	    else
	      printf("\"%s\"", tstr.c_str());
	  }
	  else
	    printf("\"%s\"", m_var_vals[i].c_str());	    
	}
	else
	  printf("n/a");	
      }
      else if(m_var_type[i] == "double")
	printf("%s", m_var_vals[i].c_str());
      
      if(vcolor != "")
	printf("%s", termColor().c_str());
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
  if(newline)
    printf("\n");
}


//------------------------------------------------------------
// Procedure: printHistoryReport()

void XMS::printHistoryReport()
{
  string refstr = toupper(m_refresh_mode) + termColor();
  if(m_refresh_mode == "paused") 
    refstr = termColor("reversered") + refstr;
  else
    refstr = termColor("reversegreen") + refstr;
  string mode_str = "(MODE = HISTORY:" + refstr + ")";   

  unsigned int varlen = m_history_var.length();
  if(varlen < 11)
    varlen = 11;

  unsigned int max_srclen = 12;
  if(m_display_source) {
    // Find length of longest source string
    list<string>::iterator p;
    for(p=m_history_sources.begin(); p != m_history_sources.end(); p++) {
      if(p->length() > max_srclen)
	max_srclen = p->length();
    }
  }

  bool do_the_report = false;
  if((m_refresh_mode == "paused") && m_update_requested)
    do_the_report = true;
  if(m_refresh_mode == "streaming")
    do_the_report = true;
  if((m_refresh_mode == "events") && 
     (m_history_event || m_update_requested))
    do_the_report = true;

  if(!do_the_report)
    return;

  m_displayed_help = false;

  m_history_event = false;
  m_update_requested = false;
  
  printf("\n\n\n\n\n");
  
  string var_hdr = padString("VariableName", varlen, false);
  if(m_report_histvar)
    printf("  %s", var_hdr.c_str());
  
  string src_hdr = padString("(S)ource", max_srclen, false);
  if(m_display_source)
    printf("  %s", src_hdr.c_str());
  else
    printf(" (S) ");

  if(m_display_time)
    printf("  %-12s", "(T)ime");
  else
    printf(" (T) ");

  printf(" VarValue %s\n", mode_str.c_str());
  
  string var_bar = padString("------------", varlen, false);
  if(m_report_histvar)
    printf("  %s", var_bar.c_str());
  
  string src_bar = padString("-------------", max_srclen, false);
  if(m_display_source)
    printf("  %s", src_bar.c_str());
  else
    printf(" --- ");
  
  if(m_display_time)
    printf("  %-12s", "----------");
  else
    printf(" --- ");
  
  printf(" ----------- (%d)\n", m_iterations);
  
  list<string> hist_list    = m_history_list;
  list<string> hist_sources = m_history_sources;
  list<int>    hist_counts  = m_history_counts;
  list<double> hist_times   = m_history_times;

  while(hist_list.size() > 0) {    
    string entry  = hist_list.back();
    string source = hist_sources.back();
    int    count  = hist_counts.back();
    string htime  = doubleToString(hist_times.back(), 2);
    
    if(m_report_histvar) {
      string var = padString(m_history_var, varlen, false);
      printf("  %s", var.c_str());
    }

    if(m_display_source) {
      source = padString(source, max_srclen, false);
      printf("  %s", source.c_str());
    }
    else
      printf("     ");
    
    if(m_display_time)
      printf("  %-12s", htime.c_str());
    else
      printf("     ");
    
    printf(" (%d) %s", count, entry.c_str());
    printf("\n");		

    hist_list.pop_back();
    hist_sources.pop_back();
    hist_counts.pop_back();
    hist_times.pop_back();
  }
}


//------------------------------------------------------------
// Procedure: updateVariable
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void XMS::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.GetKey();  
  double vtime   = msg.GetTime() - m_db_start_time;

  string vtime_str = doubleToString(vtime, 2);
  vtime_str = dstringCompact(vtime_str);

  updateVarSource(varname, msg.GetSource());
  updateVarSourceAux(varname, msg.GetSourceAux());

  updateVarTime(varname, vtime_str);
  updateVarCommunity(varname, msg.GetCommunity());
  
  if(varname == m_history_var) {
    m_history_event = true;
    if(msg.IsString()) {
      string entry = ("\"" + msg.GetString() + "\""); 
      updateHistory(entry, msg.GetSource(), vtime);
    }
    else if(msg.IsDouble()) {
      string entry = dstringCompact(doubleToString(msg.GetDouble(),6));
      updateHistory(entry, msg.GetSource(), vtime);
    }
  }

  if(msg.IsString()) {
    updateVarVal(varname, msg.GetString());
    updateVarType(varname, "string");
  }      
  else if(msg.IsDouble()) {
    updateVarVal(varname, doubleToString(msg.GetDouble()));
    updateVarType(varname, "double");
  }
  else if(msg.IsDataType(MOOS_NOT_SET)) {
    updateVarVal(varname, "n/a");
    updateVarSource(varname, "n/a");
    updateVarTime(varname, "n/a");
    updateVarCommunity(varname, "n/a");
    updateVarType(varname, "string");
  }
}

//------------------------------------------------------------
// Procedure: refreshAllVarsList
// tes 2.23.08
//
// finds all variables in the MOOS database and adds the ones we do
// not yet know about the mechanism and code for fetching all moos
// vars is closely related to that used for wildcard logging in
// pLogger

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
      if(sVar.length() > 7) {
	if(MOOSStrCmp(sVar.substr(sVar.length()-7, 7), "_STATUS"))
	  discard = true;
      }
      
      if(!discard) {
	if(addVariable(sVar))
	  m_Comms.Register(sVar, 0);
      }   
    }
  }
}

//------------------------------------------------------------
// Procedure: refreshProcVarsList
//      Note: Examines the list of sources-to-scope, and for a given
//            source pFooBar, it looks for PFOOBAR_STATUS and parses
//            it to know which variables pFooBar is publishing. Each
//            of those variables is then added onto the scope list.

void XMS::refreshProcVarsList()
{
  bool new_vars_added = false;
  map<string, string>::iterator p;
  for(p=m_src_map.begin(); p!=m_src_map.end(); p++) {
    string status_var = p->first;
    string status_str = p->second;
    status_str = findReplace(status_str, "Subscribing", ",Subscribing");
    vector<string> svector = parseStringQ(status_str, ',');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      string left  = stripBlankEnds(biteString(svector[i], '='));
      string right = stripBlankEnds(svector[i]);
      if(left == "Publishing") {
	right = stripQuotes(right);
	vector<string> jvector = parseString(right, ',');
	unsigned int j, jsize = jvector.size();
	for(j=0; j<jsize; j++) {
	  string varname = jvector[j];
	  if((varname != status_var) && addVariable(varname)) {
	    new_vars_added = true;
	    m_Comms.Register(varname, 0);
	  }
	}
      }
    }
  }
  cacheColorMap();
}

//------------------------------------------------------------
// Procedure: cacheColorMap

void XMS::cacheColorMap()
{
  unsigned int i, vsize = m_var_names.size();
  for(i=0; i<vsize; i++) {
    string v1 = getColorMapping(m_var_names[i]);
    string v2 = getColorMapping(m_var_source[i]);
    string v3 = getColorMapping(m_var_community[i]);
    if(v1 != "")
      m_var_color[i] = v1;
    else if(v2 != "")
      m_var_color[i] = v2;
    else if(v3 != "")
      m_var_color[i] = v3;
  }
}


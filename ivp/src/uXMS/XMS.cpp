/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XMS.cpp                                              */
/*    DATE: May 27th 2007                                        */
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
#include "XMS.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "TermUtils.h"
#include "ACTable.h"

// number of seconds before checking for new moos vars (in all mode)
#define ALL_BLACKOUT 1

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Procedure: Constructor

XMS::XMS()
{    
  m_display_source     = false;
  m_display_aux_source = false;
  m_display_time       = false;
  m_display_community  = false;
  m_display_own_community = true;
  m_display_hist_var   = true;
  
  m_ignore_file_vars   = false;
  
  m_update_requested  = true;
  m_history_event     = true;
  m_refresh_mode      = "events";
  m_content_mode      = "scoping";
  m_content_mode_prev = "scoping";

  m_trunc_data        = 0;
  m_trunc_data_start  = 20;
  m_display_virgins   = true;
  m_db_start_time     = 0;
  
  m_filter            = "";
  m_history_length    = 40;
  
  m_display_all       = false;
  m_display_all_requested = false;
  m_display_all_requested = true;
  m_last_all_refresh  = 0;

  m_max_proc_name_len  = 12;
  m_variable_id_srcs   = 0;
  m_proc_watch_count   = 0;
  m_proc_watch_summary = "not running";

  m_suppress_appcasts  = false;
  m_help_is_showing    = false;
}

//------------------------------------------------------------
//  Proc: ConfigureComms
//  Note: Overload MOOSApp::ConfigureComms implementation which would
//        have grabbed port/host info from the .moos file instead

bool XMS::ConfigureComms()
{
  if((m_term_server_host == "") || (m_term_server_port == "")) 
    return(CMOOSApp::ConfigureComms());

  long lport = atol(m_term_server_port.c_str());
  CMOOSApp::SetServer(m_term_server_host.c_str(), lport);

  cout << "XMS::ConfigureComms(): Host/Port Configured Locally:" << endl;
  cout << "  m_sServerHost: " << m_sServerHost << endl;
  cout << "  m_lServErport: " << m_lServerPort << endl;

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
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  // First, if m_db_start_time is not set, scan the mail for the
  // DB_UPTIME mail from the MOOSDB and set. ONCE.
  if(m_db_start_time == 0) {
    for(p = NewMail.begin(); p!=NewMail.end(); p++) {
      //      p->Trace();
      CMOOSMsg &msg = *p;
      if(msg.GetKey() == "DB_UPTIME") {
	m_db_start_time = MOOSTime() - msg.GetDouble();
	m_community = msg.GetCommunity();
      }
    }
  }
  
  // Update the values of all variables we have registered for.  
  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.

  for(p = NewMail.begin(); p!=NewMail.end(); p++) {
    //p->Trace();
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    if((key.length() >= 7) && (key.substr(key.length()-7,7) == "_STATUS")) {
      if(m_map_src_status.count(key) == 1) {
	m_map_src_status[key] = msg.GetString();
      }
    }

    else if(key == "PROC_WATCH_SUMMARY") {
      m_proc_watch_count++;
      m_proc_watch_summary = msg.GetString();
    }

    else if(key == "DB_CLIENTS") 
      updateDBClients(msg.GetString());
    
    // A check is made in updateVariable() to ensure the given variable
    // is indeed on the scope list.
    if(m_display_all) {
      if(m_display_all_really)
	addVariable(key);
      else if (!strEnds(key, "_STATUS") && (key != "APPCAST") && 
	       (key != "DB_RWSUMMARY") && (key != "DB_VARSUMMARY"))
	addVariable(key);
    }
    updateVariable(msg);
  }
  return(true);
}


//------------------------------------------------------------
// Procedure: Iterate()

bool XMS::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_display_all && !m_display_all_requested) {
    Register("*","*",0);
    m_display_all_requested = true;
  }
  
  refreshProcVarsList();
  
  string term_directive;
  if(m_refresh_mode == "paused")
    term_directive = "noterm";

  if((m_refresh_mode == "events") && !m_update_requested)
    term_directive = "noterm";

  if(m_update_requested || (m_refresh_mode=="streaming"))
    term_directive = "doterm";

  if(m_suppress_appcasts)
    term_directive += "noappcast";

  AppCastingMOOSApp::PostReport(term_directive);
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
  string app_name = m_app_name_noindex;
  if(app_name == "")
    app_name = GetAppName();

  string directives  = "must_have_moosblock=false,";
  directives += "must_have_community=false,";
  directives += "alt_config_block_name=" + app_name;
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  // If there is no configuration block it is assumed that this session
  // was meant for the terminal only, and no appcast requests will be
  // honored, no appcast postings made.

  //m_MissionReader.GetConfiguration(app_name, sParams);
  if(!m_MissionReader.GetConfiguration(app_name, sParams)) 
    m_suppress_appcasts = true;

  // Variables shown in the order they appear in the config file
  sParams.reverse();
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = stripQuotes(line);

    bool handled = false;
    if(param == "REFRESH_MODE") 
      handled = setRefreshMode(value);

    else if(param == "CONTENT_MODE") 
      handled = setContentMode(value);

    // Depricated PAUSED
    else if(param == "PAUSED") {
      handled = true;
      if(tolower(value) == "true")
	m_refresh_mode = "paused";
      else if (tolower(value) == "false")
	m_refresh_mode = "events";
      else
	handled = false;
      string str = "PAUSED parameter deprecated, use REFRESH_MODE instead";
      reportUnhandledConfigWarning(str);
    }
  
    else if((param == "HISTORY_VAR") && !strContainsWhite(value)) {
      handled = addVariable(value, true); // true means itsa history variable
      handled = true;
    }

    else if(param == "DISPLAY_VIRGINS")
      handled = setBooleanOnString(m_display_virgins, value);
    
    else if((param == "TRUNC_DATA") && isBoolean(value)) {
      handled = true;
      if(tolower(value) == "true")
	m_trunc_data = m_trunc_data_start;
    }
    
    else if(param == "SOURCE")
      handled = addSources(value);
    else if(param == "DISPLAY_SOURCE")
      handled = setBooleanOnString(m_display_source, value);

    else if(param == "DISPLAY_HISTORY_VAR")
      handled = setBooleanOnString(m_display_hist_var, value);
    
    else if((param == "DISPLAY_AUX_SOURCE") && isBoolean(value)) {
      handled = true;
      m_display_source = (tolower(value) == "true");
      m_display_aux_source = (tolower(value) == "true");
    }

    else if(param == "DISPLAY_TIME")
      handled = setBooleanOnString(m_display_time, value);
    
    else if(param == "DISPLAY_COMMUNITY")
      handled = setBooleanOnString(m_display_community, value);
    
    else if((param == "DISPLAY_ALL") && isBoolean(value)) {
      handled = true;
      if(!m_ignore_file_vars)
	m_display_all = (tolower(value) == "true");
    }    
    else if((param == "DISPLAY_ALLL") && isBoolean(value)) {
      handled = true;
      if(!m_ignore_file_vars) {
	m_display_all = (tolower(value) == "true");
	m_display_all_really = (tolower(value) == "true");
      }
    }    
    else if((param == "COLORMAP") || (param == "COLOR_MAP"))
      handled = setColorMappingsPairs(value);

    else if(param == "VAR") {
      if(m_ignore_file_vars)
	handled = true;
      else
	handled = addVariables(value);
    } 

    if(!handled)
      reportUnhandledConfigWarning(orig);    
  }
  
  // setup for display all
  if(m_display_all) {
    Register("*","*",0);
    m_display_all_requested = true;
  }
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
  case 'S':
    m_display_source = !m_display_source;
    m_update_requested = true;
    break;
  case 'x':
  case 'X':
    m_display_aux_source = !m_display_aux_source;
    m_update_requested = true;
    break;
  case 't':
  case 'T':
    m_display_time = !m_display_time;
    m_update_requested = true;
    break;
  case 'v':
  case 'V':
    m_display_virgins = !m_display_virgins;
    m_update_requested = true;
    break;
  case 'd':
  case 'D':
    setContentMode("scoping", true);
    m_update_requested = true;
    break;
  case 'c':
  case 'C':
    m_display_community = !m_display_community;
    m_update_requested = true;
    break;
  case 'j':
  case 'J':
    m_display_hist_var = !m_display_hist_var;
    if(m_content_mode == "history")
      m_update_requested = true;
    break;
  case 'p':
  case 'P':
    setContentMode("procs", true);
    m_update_requested = true;
    break;
  case 'z':
  case 'Z':
    if(m_history_var != "") {
      m_update_requested = true;
      setContentMode("history", true);
    }
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
    setContentMode("help", true);
    m_update_requested = true;
    break;
  case '-': 
    m_refresh_mode = "paused";
    handleSelectMaskSubSources();
    setContentMode("scoping");
    m_update_requested = true;
    break;
  case '+': 
    {
      string saved_refresh_mode = m_refresh_mode;
      m_refresh_mode = "paused";
      handleSelectMaskAddSources();
      //setContentMode("scoping");
      m_update_requested = true;
      m_refresh_mode = saved_refresh_mode;
    }
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
    m_map_orig_var_entries = m_map_var_entries;
    break;
    
    // turn show all variables mode off
  case 'a':
    if(m_display_all) {
      m_display_all = false;
      m_update_requested = true;
      
      // restore the variable list at startup
      m_map_var_entries = m_map_orig_var_entries;
    }
    
  default:
    break;
  }
}


//------------------------------------------------------------
// Procedure: addVariables

bool XMS::addVariables(string line)
{
  bool ok = true;
  line = stripBlankEnds(line);
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  
  for(i=0; i<vsize; i++) {
    string var = stripBlankEnds(svector[i]);
    ok = ok && addVariable(var);
  }
  return(ok);
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
  if(histvar) {
    // Do not overwrite if already set (command line takes precedent)
    if(m_history_var == "") {
      m_history_var = varname;
      // If currently no variables scoped, hist_mode=true
      if(m_map_var_entries.size() == 0)
	setContentMode("history");
    }    
  }
  
  // Simply return true if the variable has already been added.
  if(m_map_var_entries.count(varname) != 0)
    return(true);
  
  ScopeEntry entry;
  entry.setValue("n/a");
  entry.setType("string");
  entry.setSource(" n/a");
  entry.setSrcAux(" n/a");
  entry.setTime(" n/a");
  entry.setCommunity("");
  
  m_map_var_entries[varname] = entry;

  return(true);
}

//------------------------------------------------------------
// Procedure: addSources

bool XMS::addSources(string sources)
{
  sources = stripBlankEnds(sources);
  vector<string> svector = parseString(sources, ',');
  unsigned int i, vsize = svector.size();

  if(vsize == 0)
    return(false);

  bool ok = true;
  for(i=0; i<vsize; i++) 
    ok = ok && addSource(svector[i]);

  return(ok);
}

//------------------------------------------------------------
// Procedure: addSource

bool XMS::addSource(string source)
{
  if(strContainsWhite(source) || (source == ""))
    return(false);

  source = toupper(source) + "_STATUS";
  m_map_src_status[source] = "empty";

  return(true);
}

//------------------------------------------------------------
// Procedure: setContentMode

bool XMS::setContentMode(string str, bool toggle_on_match)
{
  str = tolower(str);
  if((str != "scoping") && (str != "procs") && 
     (str != "history") && (str != "help"))
    return(false);

  m_update_requested = true;

  // If the requested content mode is the same as the present content
  // mode, this may be interpreted as a request to toggle back to the
  // previous content mode.
  if(str == m_content_mode) {
    if(toggle_on_match) {
      string tmp = m_content_mode;
      m_content_mode = m_content_mode_prev;
      m_content_mode_prev = tmp;
    }
    return(true);
  }
  
  m_content_mode_prev = m_content_mode;
  m_content_mode = str;
  return(true);
}

//------------------------------------------------------------
// Procedure: setRefreshMode

bool XMS::setRefreshMode(string str)
{
  str = tolower(str);
  if((str != "events") && (str != "paused") && (str != "streaming"))
    return(false);
  
  m_refresh_mode = str;
  return(true);
}

//------------------------------------------------------------
// Procedure: setDisplayColumns
//   Example: str = "source,time,aux"

void XMS::setDisplayColumns(string str)
{
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string field = stripBlankEnds(tolower(svector[i]));
    if(field == "source")
      setDispSource(true);
    else if(field == "time")
      setDispTime(true);
    else if(field == "community")
      setDispCommunity(true);
    else if(field == "aux")
      setDispAuxSource(true);
  }  
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

void XMS::setTruncData(string val)
{
  double dval = atoi(val.c_str());

  m_trunc_data = vclip(dval, 10, 1000);
  m_trunc_data_start = m_trunc_data;
}

//------------------------------------------------------------
// Procedure: setTermReportInterval
//      Note: m_term_report_interval is set at the AppCastingMOOSApp
//            level and normally not accessible in this way. But this
//            method is provided to allow a hook from the commandline.

void XMS::setTermReportInterval(string str)
{
  if(!isNumber(str))
    return;

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
}

//------------------------------------------------------------
// Procedure: setColorMapping

bool XMS::setColorMapping(string str, string color)
{
  color = tolower(color);
  if((color!="red") && (color!="green") && (color!="blue") &&
     (color!="cyan") && (color!="magenta") && (color!="any"))
    return(false);

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
      return(true);
  }
  
  m_color_map[str] = color;
  return(true);
}

//------------------------------------------------------------
// Procedure: setColorMappingsAny
//   Example: str = "pHelmIvP,NAV_X,NAV_Y"

bool XMS::setColorMappingsAny(string str)
{
  bool ok = true;
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize  = svector.size();
  for(i=0; i<vsize; i++) {
    string key = stripBlankEnds(svector[i]);
    ok = ok && setColorMapping(key, "any");
  }
  return(ok);
}

//------------------------------------------------------------
// Procedure: setColorMappingsPairs
//   Example: str = "IVPHELM_SUMMARY,blue,BHV_WARNING,red

bool XMS::setColorMappingsPairs(string str)
{
  bool ok = true;
  while(str != "") {
    string vname = biteStringX(str, ',');
    string color = biteStringX(str, ',');
    if(color == "")
      color = "any";
    ok = ok && setColorMapping(vname, color);
  }
  return(ok);
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
  AppCastingMOOSApp::RegisterVariables();
  map<string,ScopeEntry>::iterator p;
  for(p=m_map_var_entries.begin(); p!=m_map_var_entries.end(); p++) {
    string varname = p->first;
    Register(varname, 0);
  }

  map<string,string>::iterator p2;
  for(p2=m_map_src_status.begin(); p2!=m_map_src_status.end(); p2++) {
    string reg_str = p2->first;
    Register(reg_str, 1.0);
  }

  if(m_history_var != "")
    Register(m_history_var, 0);

  Register("DB_UPTIME", 0);
}

//------------------------------------------------------------
// Procedures: updateVarEntry
//       Note: Returns true if the value/type/source etc has changed.

bool XMS::updateVarEntry(string varname, const ScopeEntry& new_entry)
{
  if(m_map_var_entries.count(varname) == 0) {
    m_map_var_entries[varname] = new_entry;
    return(true);
  }

  // Handle the Source information
  const string& source = new_entry.getSource();
  m_map_src_update[source] = m_curr_time;
  updateSourceInfo(source);

  const ScopeEntry& entry = m_map_var_entries[varname];
  if(entry.isEqual(new_entry)) 
    return(false);

  m_map_var_entries[varname] = new_entry;
  return(true);
}

//------------------------------------------------------------
// Procedures: updateSourceInfo
//   Purposes: Create a new ID for the new source
//             Update the max string length across sources

//   Fixed IDs    (0)  MOOSDB
//                (1)  pHelmIvP
//   Variable IDs (a)  pMarinePID
// 

void XMS::updateSourceInfo(string new_src)
{
  if(m_map_src_id.count(new_src) != 0)
    return;

  if(strBegins(new_src, "MOOSDB")) {
    m_map_id_src["0"] = new_src;
    m_map_src_id[new_src] = "0";
  }
  else if(new_src == "pHelmIvP") {
    m_map_id_src["1"] = new_src;
    m_map_src_id[new_src] = "1";
  }
  else {
    char c = 97 + (int)(m_variable_id_srcs);  // 'a' + cnt
    string id(1,c);
    m_map_id_src[id] = new_src;
    m_map_src_id[new_src] = id;
    m_variable_id_srcs++;
  }

  if(new_src.length() > m_max_proc_name_len)
    m_max_proc_name_len = new_src.length();
}

//------------------------------------------------------------
// Procedures: updateHistory
 
void XMS::updateHistory(string entry, string source, 
			string srcaux, double htime)
{
  // First check if this is a duplicate from previous posting
  if(!m_history_list.empty()    && 
     !m_history_sources.empty() &&
     !m_history_sources_aux.empty()) {
    if((entry  == m_history_list.front())    &&
       (source == m_history_sources.front()) &&
       (srcaux == m_history_sources_aux.front())) {
      m_history_counts.front()++;
      m_history_times.front() = htime;
      return;
    }
  }
  
  m_history_list.push_front(entry);
  m_history_counts.push_front(1);
  m_history_sources.push_front(source);
  m_history_sources_aux.push_front(srcaux);
  m_history_times.push_front(htime);
  
  while(m_history_list.size() > m_history_length) {
    m_history_list.pop_back();
    m_history_counts.pop_back();
    m_history_sources.pop_back();
    m_history_sources_aux.pop_back();
    m_history_times.pop_back();
  }
}

//------------------------------------------------------------
// Procedure: buildReport
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.

bool XMS::buildReport()
{
  m_update_requested = true;
  bool generated = false;
  if(m_content_mode == "help") {
    if(!m_help_is_showing) {
      generated = printHelp();
      m_help_is_showing = true;
    }
  }
  else
    m_help_is_showing = false;

  if(m_content_mode == "history")
    generated = printHistoryReport();

  if(m_content_mode == "scoping")
    generated = printReport();
  
  if(m_content_mode == "procs")
    generated = printProcsReport();

  return(generated);
}

//------------------------------------------------------------
// Procedure: printHelp()

bool XMS::printHelp()
{
  if(!m_update_requested)
    return(false);

  ACTable actab(1);
  
  string refstr = termColor("reversered") + "HELP" + termColor();
  string mode_str = "(" + refstr + ")";   
  
  actab.addCell("KeyStroke    Function         " + mode_str);
  actab.addCell("---------    ---------------------------             ");
  actab.addCell("    s        Toggle show source of variables         ");
  actab.addCell("    t        Toggle show time of variables           ");
  actab.addCell("    c        Toggle show community of variables      ");
  actab.addCell("    v        Toggle show virgin variables            ");
  actab.addCell("    x        Toggle show Auxilliary Src if non-empty ");
  actab.addCell("    d        Content Mode: Scoping Normal            ");
  actab.addCell("    h        Content Mode: Help. Hit 'R' to resume   ");
  actab.addCell("    p        Content Mode: Processes Info            ");
  actab.addCell("    z        Content Mode: Variable History          ");
  actab.addCell("  > or <     Show More or Less Variable History      ");
  actab.addCell("  } or {     Show More or Less Truncated VarValue    ");
  actab.addCell("    /        Begin entering a filter string          ");
  actab.addCell("    `        Toggle Data Field truncation            ");
  actab.addCell("    ?        Clear current filter                    ");    
  actab.addCell("    a        Revert to variables shown at startup    ");
  actab.addCell("    A        Display all variables in the database   ");
  actab.addCell("   u/SPC     Refresh Mode: Update then Pause         ");
  actab.addCell("    r        Refresh Mode: Streaming                 ");
  actab.addCell("    e        Refresh Mode: Event-driven refresh      ");
  
  m_msgs << actab.getFormattedString();
  //m_refresh_mode = "paused";
  m_update_requested = false;

  return(true);
}

//------------------------------------------------------------
// Procedure: printReport()

bool XMS::printReport()
{  
  m_update_requested = false;

  ACTable actab(5,2);
  
  // No need to pad out the rightmost column. Leads to unnecessary line 
  // wrap-arounds if there is even one long entry in this column.
  actab.setColumnNoPad(4); 

  string refstr = toupper(m_refresh_mode) + termColor();
  if(m_refresh_mode == "paused") 
    refstr = termColor("reversered") + refstr;
  else
    refstr = termColor("reversegreen") + refstr;
  string mode_str = "(SCOPING:" + refstr + ")";   

  string src_header  = m_display_source ? "(S)ource" : "(S)";
  string time_header = m_display_time   ? "(T)ime"   : "(T)";
  string comm_header = m_display_community ? "(C)ommunity" : "(C)";

  actab << "VarName" << src_header << time_header << comm_header;
  actab << "VarValue " + mode_str;
  actab.addHeaderLines(25);

  unsigned int now_trunc_data = m_trunc_data;
  if(m_trunc_data > 0) {
    if(!m_display_community) 
      now_trunc_data += 8;
    if(!m_display_time) 
      now_trunc_data += 5;
    if(!m_display_source) 
      now_trunc_data += 10;
  }

  // For each variable on the watch list
  map<string,ScopeEntry>::iterator p;
  for(p=m_map_var_entries.begin(); p!=m_map_var_entries.end(); p++) {
    string varname   = p->first;
    ScopeEntry entry = p->second;
    string varval    = entry.getValue();
    string varsrc    = entry.getSource();
    
    // We know that APPCAST message can be long and contain CRLF's so we
    // just truncate it to the source of the appcast.
    if(varname == "APPCAST") {
      string new_varval = biteString(varval, '!');
      varval = new_varval;
    }

    bool dispvar = true;
    if((varval == "n/a") && !m_display_virgins)
      dispvar = false;
    if(dispvar && m_mask_sub_sources.count(varsrc))
      dispvar = false;
    if(dispvar && ! MOOSStrCmp(m_filter, varname.substr(0, m_filter.length())))
      dispvar = false;

    if(dispvar) {
      string vcolor = m_map_var_entries_color[varname];
      actab.addCell(varname, vcolor);

      // Handle the Source field.
      string varsrc;
      if(m_display_source) {
	varsrc = m_map_var_entries[varname].getSource();
	if(m_display_aux_source) {
	  string varsrc_aux = m_map_var_entries[varname].getSrcAux();
	  if(varsrc_aux != "")
	    varsrc = "[" + varsrc_aux + "]";
	}
	varsrc = truncString(varsrc, 16, "middle");
      }
      actab.addCell(varsrc, vcolor);

      // Handle the Time field
      if(m_display_time)
	actab.addCell(m_map_var_entries[varname].getTime(), vcolor);
      else
	actab.addCell("");
      
      // Handle the Community field
      if(m_display_community) {
	string community = m_map_var_entries[varname].getCommunity();
	string comm_str = truncString(community, 14, "middle");
	actab.addCell(comm_str, vcolor);
      }
      else
	actab.addCell("");
      
      // Handle the VarValue field
      string vartype = m_map_var_entries[varname].getType();
      string valcell;
      if(vartype == "string") {
	if(varval != "n/a") {
	  if(m_trunc_data > 0) {
	    string tstr = truncString(varval, now_trunc_data);
	    if(tstr.length() < varval.length())
	      valcell = "\"" + tstr + "...\"";
	    else
	      valcell = "\"" + tstr + "\"";
	  }
	  else
	    valcell = "\"" + varval + "\"";
	}
	else
	  valcell= "n/a";	
      }
      else if(vartype == "double")
	valcell = varval;

      actab.addCell(valcell, vcolor);
    }
  }
  
  m_msgs << actab.getFormattedString();

  // provide information for current filter(s)
  bool newline = false;
  if(m_filter != "") {
    m_msgs << endl << ("  -- filter: " + m_filter + " -- ");
    newline = true;
  }
  if(m_display_all) {
    m_msgs << endl << "  -- displaying all variables --";
    newline = true;
  }
  if(newline)
    m_msgs << endl;

  return(true);
}

//------------------------------------------------------------
// Procedure: printHistoryReport()

bool XMS::printHistoryReport()
{
  bool do_the_report = false;
  if((m_refresh_mode == "paused") && m_update_requested)
    do_the_report = true;
  if(m_refresh_mode == "streaming")
    do_the_report = true;
  if((m_refresh_mode == "events") && 
     (m_history_event || m_update_requested))
    do_the_report = true;

  if(!do_the_report)
    return(false);

  m_history_event = false;
  m_update_requested = false;

  ACTable actab(4,2);

  string refstr = toupper(m_refresh_mode) + termColor();
  if(m_refresh_mode == "paused") 
    refstr = termColor("reversered") + refstr;
  else
    refstr = termColor("reversegreen") + refstr;
  string mode_str = "(HISTORY:" + refstr + ")";   

  string src_header  = m_display_source ? "(S)ource" : "(S)";
  string time_header = m_display_time   ? "(T)ime"   : "(T)";
  string var_header  = m_display_hist_var ? "(J)VarName" : "(J)";

  actab << var_header << src_header << time_header;
  actab << "VarValue " + mode_str;
  actab.addHeaderLines(25);

  list<string> hist_list    = m_history_list;
  list<string> hist_sources = m_history_sources;
  list<string> hist_sources_aux = m_history_sources_aux;
  list<int>    hist_counts  = m_history_counts;
  list<double> hist_times   = m_history_times;
  
  while(hist_list.size() > 0) {    
    string varval = hist_list.back();
    string source = hist_sources.back();
    string srcaux = hist_sources_aux.back();
    int    count  = hist_counts.back();
    string htime  = doubleToString(hist_times.back(), 2);
 
    if(m_display_hist_var)
      actab << m_history_var;
    else
      actab << "";
    
    // Handle the Source field
    if(m_display_source) {
      if(m_display_aux_source && (srcaux != ""))
	actab << ("[" + srcaux + "]");
      else
	actab << source;
    }
    else
      actab << "";
    
    // Handle the Time field
    if(m_display_time)
      actab << htime;
    else
      actab << "";
    
    // Handle the VarValue field
    if(m_trunc_data > 0) {
      string tstr = truncString(varval, m_trunc_data);
      if(tstr.length() < varval.length())
	tstr += "...";
      varval = tstr;
    }

    varval = "(" + intToString(count) + ") " + varval; 
    actab << varval;

    hist_list.pop_back();
    hist_sources.pop_back();
    hist_counts.pop_back();
    hist_times.pop_back();
  }

  m_msgs << actab.getFormattedString();

  return(true);
}

//------------------------------------------------------------
// Procedure: printProcsReport()
//
//   uProcessWatch: Not Running
//   uProcessWatch: All-Present (12)
//   uProcessWatch: Awol: pFoobar, uBananas

//   ID     Process Name       Mail     Client 
//   --     ------------       -------  ------             
//   0      MOOSDB             0.12     0.04             
//   1      pHelmIvP           0.27     0.04             
//   2      pLogger            0.12     0.04             
//   A      pMarinePID         0.04     0.04             
//   B      iActuationKF       0.04     0.04             
//   C      iGPS               0.15     0.04
//   D      iCompass           0.22     0.04
//
//   Action: 1bc                

bool XMS::printProcsReport()
{
  // Part 1: Determine if it is actually appropriate to DO the report
  if((m_refresh_mode == "paused") && !m_update_requested)
    return(false);

  // Part 2: Determine the mode string at the top of the report
  string refstr = toupper(m_refresh_mode) + termColor();
  if(m_refresh_mode == "paused") 
    refstr = termColor("reversered") + refstr;
  else
    refstr = termColor("reversegreen") + refstr;
  string mode_str = "(PROCESSES:" + refstr + ")";   

  // Now that we've decided to do the report, set some status vars
  m_update_requested = false;
  
  m_msgs << padString(mode_str, 78) << endl;
  
  // Part 4: Handle the Proc-Watch information
  string psummary = m_proc_watch_summary;
  if(strContains(psummary, "AWOL"))
    psummary = termColor("reversered") + psummary + termColor();
  
  m_msgs << "uProcessWatch(" << m_proc_watch_count << "): " << psummary << endl;

  ACTable actab(4,2);
  actab << "ID | Process Name | Mail | Client";
  actab.addHeaderLines();

  map<string, string>::iterator q;
  for(q=m_map_id_src.begin(); q!=m_map_id_src.end(); q++) {
    string procid   = q->first;
    string procname = q->second;

    // Post the ID
    if(m_mask_sub_sources.count(procname))
      procid = "-" + procid;
    else if(m_mask_add_sources.count(procname))
      procid = "+" + procid;
    
    // Post the Mail Age (Time since last mail recvd from this source)
    double posttime = m_map_src_update[procname];
    string mail_age_str = "-";
    if(posttime > 0)
      mail_age_str = doubleToString((m_curr_time - posttime), 2);

    // Post the Client Age (Time since this source appeared in DB_CLIENTS)
    double dbclient_time = m_map_src_dbclient[procname];
    string client_age_str = "-";
    if(dbclient_time > 0)
      client_age_str = doubleToString((m_curr_time - dbclient_time), 2);
    
    actab << procid << procname << mail_age_str << client_age_str;
  }

  m_msgs << actab.getFormattedString();

  return(true);
}

//------------------------------------------------------------
// Procedure: updateDBClients
//      Note: Parses the DB_CLIENTS message and notes the current time
//            for each client on the list. Also adds the client/source
//            to the list of known sources if not otherwise known.

void XMS::updateDBClients(string db_clients)
{
  vector<string> svector = parseString(db_clients, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string source = stripBlankEnds(svector[i]);
    bool exclude = false;
    if(source == "DBWebServer")
      exclude = true;
    else if(source == "uXMS") 
      exclude = true;
    else if(strBegins(source, "uXMS")) {
      string source_copy = source;
      string front = biteString(source_copy, '_');
      string back  = source_copy;
      if((front == "uXMS") && isNumber(back))
	exclude = true;
    }

    if(!exclude) {
      m_map_src_dbclient[source] = m_curr_time;
      updateSourceInfo(source);
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
  string varname = msg.GetKey();  
  double vtime   = msg.GetTime() - m_db_start_time;

  if(m_map_var_entries.count(varname) == 0)
    return;

  string vtime_str = doubleToString(vtime, 2);
  vtime_str = dstringCompact(vtime_str);

  ScopeEntry entry;
  if(msg.IsString()) {
    entry.setType("string");
    entry.setValue(msg.GetString());
  }      
  else if(msg.IsDouble()) {
    entry.setType("double");
    entry.setValue(doubleToStringX(msg.GetDouble(),6));
  }

  entry.setSource(msg.GetSource());
  entry.setSrcAux(msg.GetSourceAux());
  entry.setTime(doubleToString((msg.GetTime()-m_db_start_time),2));
  entry.setCommunity(msg.GetCommunity());

  bool changed = updateVarEntry(varname, entry);

  if(varname == m_history_var) {
    m_history_event = true;
    if(msg.IsString()) {
      string entry = ("\"" + msg.GetString() + "\""); 
      updateHistory(entry, msg.GetSource(), msg.GetSourceAux(), vtime);
    }
    else if(msg.IsDouble()) {
      string entry = dstringCompact(doubleToString(msg.GetDouble(),6));
      updateHistory(entry, msg.GetSource(), msg.GetSourceAux(), vtime);
    }
  }

  if(changed && m_refresh_mode == "events")
    m_update_requested = true;
}

//------------------------------------------------------------
// Procedure: refreshProcVarsList
//      Note: Examines the list of sources-to-scope, and for a given
//            source pFooBar, it looks for PFOOBAR_STATUS and parses
//            it to know which variables pFooBar is publishing. Each
//            of those variables is then added onto the scope list.

void XMS::refreshProcVarsList()
{
  map<string, string>::iterator p;
  for(p=m_map_src_status.begin(); p!=m_map_src_status.end(); p++) {
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
	    Register(varname, 0);
	  }
	}
      }
    }
  }
  cacheColorMap();
}

//------------------------------------------------------------
// Procedure: cacheColorMap
//      Note: The color mapping changes rarely if at all after startup.
//            The mapping may be associated by variable name, source or
//            community. It would be inefficient to determine a variable's
//            color on each report generated, so cache it here.

void XMS::cacheColorMap()
{
  map<string,ScopeEntry>::iterator p;
  for(p=m_map_var_entries.begin(); p!=m_map_var_entries.end(); p++) {
    string varname = p->first;
    string v1 = getColorMapping(varname);
    string v2 = getColorMapping(m_map_var_entries[varname].getSource());
    string v3 = getColorMapping(m_map_var_entries[varname].getCommunity());
    if(v1 != "")
      m_map_var_entries_color[varname] = v1;
    else if(v2 != "")
      m_map_var_entries_color[varname] = v2;
    else if(v3 != "")
      m_map_var_entries_color[varname] = v3;
  }
}


//------------------------------------------------------------
// Procedure: handleSelectMaskSubSource

void XMS::handleSelectMaskSubSources()
{
  m_update_requested = true;
  cout << "\n\n\n\n\n\n\n";
  m_msgs.clear();
  m_msgs.str("");
  printProcsReport();
  cout << m_msgs.str() << endl;
  m_msgs.clear();
  m_msgs.str("");

  string sub_list;
  cout << "Exclude a process > " << flush;
  getline(cin, sub_list);

  // If user just hits return (empty) this clears the source sub list
  if(sub_list == "")
    return;
  
  // The '?' character indicates a wish to clear the source sub list.
  if(strContains(sub_list, "?"))
    m_mask_sub_sources.clear();
   
  // Otherwise we'll interpret the user entry as a request to AUGMENT
  // the current source mask list.
  for(unsigned int i=0; i<sub_list.length(); i++) {
    string id(1, sub_list[i]);
    if(m_map_id_src.count(id)) {
      m_mask_sub_sources.insert(m_map_id_src[id]);
      m_mask_add_sources.erase(m_map_id_src[id]);
    }
  }
}

//------------------------------------------------------------
// Procedure: handleSelectMaskAddSources

void XMS::handleSelectMaskAddSources()
{
  m_update_requested = true;
  cout << "\n\n\n\n\n\n\n";
  m_msgs.clear();
  m_msgs.str("");
  printProcsReport();
  cout << m_msgs.str() << endl;
  m_msgs.clear();
  m_msgs.str("");

  string add_list;
  
  cout << "Include a process > " << flush;
  getline(cin, add_list);
  
  cout << "---------------------------" << endl;
  // If user just hits return (empty) this clears source add list
  if(add_list.length() == 0)
    return;
  
  // The '?' character indicates a wish to clear the source add list.
  if(strContains(add_list, "?"))
    m_mask_add_sources.clear();
   
  // Otherwise we'll interpret the user entry as a request to AUGMENT
  // the current source mask list.
  for(unsigned int i=0; i<add_list.length(); i++) {
    string id(1, add_list[i]);
    if(m_map_id_src.count(id)) {
      string src = m_map_id_src[id];
      string src_status = toupper(src) + "_STATUS";
      addSource(src);
      Register(src_status, 1.0);
      m_mask_add_sources.insert(src);
      m_mask_sub_sources.erase(src);
    }
  }
}










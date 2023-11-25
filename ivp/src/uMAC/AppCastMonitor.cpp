/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastMonitor.cpp                                   */
/*    DATE: June 4th 2012                                        */
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

#include <iterator>
#include "MBUtils.h"
#include "ColorParse.h"
#include "AppCastMonitor.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AppCastMonitor::AppCastMonitor()
{
  m_term_reports     = 0;
  m_iteration        = 0;
  m_timewarp         = 1;
  m_curr_time        = 0;
  m_last_report_time = 0;
  m_term_report_interval = 0.6;

  m_terse_mode     = false;
  m_update_pending = true;
  m_refresh_mode   = "events";

  switchContentMode("nodes");   // nodes,procs,help,appcast
}

//---------------------------------------------------------
// Procedure: trySetInitialNodeProc()

void AppCastMonitor::trySetInitialNodeProc()
{
  if((m_initial_node == "") && (m_initial_proc == ""))
    return;
  if(m_iteration > 100)
    return;
  
  bool set_node = setCurrentNode(m_initial_node);
  if(set_node) {
    m_initial_node = "";
    switchContentMode("procs");
  }

  bool set_proc = setCurrentProc(m_initial_proc);
  if(set_proc) {
    m_initial_proc = "";
    switchContentMode("appcast");
  }
}


//---------------------------------------------------------
// Procedure: OnNewMail()

bool AppCastMonitor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "APPCAST") 
      handleMailAppCast(sval);
    else if(key == "MISSION_HASH") 
      handleMailMissionHash(sval);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool AppCastMonitor::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool AppCastMonitor::Iterate()
{
  // Part 1: Update the key state variables
  m_iteration++;

  m_curr_time = MOOSTime();
  trySetInitialNodeProc();
  
  // Consider how long its been since our last report (regular or history)
  // Want to report less frequently if using a higher time warp.
  bool   print_report = false;
  double moos_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = moos_elapsed_time / m_timewarp;

  if(real_elapsed_time >= m_term_report_interval) {
    m_last_report_time = m_curr_time;
    print_report = true;
    // Refresh the APPCAST_REQ request to the current app 
    if(m_refresh_mode == "streaming") {
      postAppCastRequest("all", "all", "", "any", 3);
    }
    else if(m_refresh_mode == "events") {
      string key_app = GetAppName() + ":app";      
      if(m_content_mode == "appcast")
	postAppCastRequest(currentNode(), currentProc(), key_app, "any", 3);
      else if(m_content_mode == "procs")
	postAppCastRequest(currentNode(), "all", key_app, "run_warning", 3);
      else if(m_content_mode == "nodes")
	postAppCastRequest("all", "all", key_app, "run_warning", 3);
    }
  }

  // Part 3: End early if we are in paused mode and no update request
  if((m_refresh_mode != "streaming") && !m_update_pending)
    return(true);

  bool update_generated = true;
  if(m_content_mode == "help")
    printHelp();
  else if((m_content_mode == "nodes") && print_report)
    printReportNodes();
  else if((m_content_mode == "procs") && print_report)
    printReportProcs();
  else if((m_content_mode == "appcast") && print_report)
    printReportAppCast();
  else
    update_generated = false;

  if(update_generated)
    m_update_pending = false;

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool AppCastMonitor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = toupper(biteStringX(*p, '='));
      string value = *p;
      
      if(param == "FOO") {
        //handled
      }
    }
  }
  
  m_timewarp = GetMOOSTimeWarp();

  string key = GetAppName() + "startup";
  postAppCastRequest("all", "all", key, "any", 10);

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables()

void AppCastMonitor::RegisterVariables()
{
  Register("APPCAST", 0);
  Register("MISSION_HASH", 0);
}

//---------------------------------------------------------
// Procedure: handleMailAppCast()

bool AppCastMonitor::handleMailAppCast(const string& str)
{
  AppCast appcast   = string2AppCast(str);
  string  node_name = appcast.getNodeName();

  if(node_name == "")
    // return(false);
    node_name = "unknown_node";
    
  // Add the appcast to the repo. Note if the node name is new.
  unsigned int old_node_count = m_repo.getNodeCount();
  bool new_node_name = false;

  m_repo.addAppCast(appcast);
  unsigned int new_node_count = m_repo.getNodeCount();
  if(new_node_count > old_node_count)
    new_node_name = true;

  // If the first time we've heard from this node, do some extra
  if(new_node_name) {
    // Notify node we want appcasts from ALL app at that node. 
    string key = GetAppName() + "startup";
    postAppCastRequest(node_name, "all", key, "any", 0.1);
  }

  if(m_refresh_mode != "paused")
    m_update_pending = true;

  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailMissionHash()
//   Example: mhash=230711-1929C-PRIM-HAYS,utc=1689118197.62 

bool AppCastMonitor::handleMailMissionHash(const string& str)
{
  string hash = tokStringParse(str, "mhash", ',', '=');
  biteString(hash, '-');
  biteString(hash, '-');

  if(hash == "")
    return(false);
  
  m_mission_hash = hash;  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleCommand()

void AppCastMonitor::handleCommand(char c)
{
  switch(c) {
  case ' ':
    m_refresh_mode   = "paused";
    m_update_pending = true;
    break;
  case '[':
    {
      if(m_content_mode == "appcast") {
	switchContentMode("procs");
	m_update_pending = true;
	setCurrentProc("all");
      }
      else if(m_content_mode == "procs") {
	switchContentMode("nodes");
	m_update_pending = true;
	setCurrentNode("all");
	setCurrentProc("all");
      }
    }
    break;		
  case 'p':
  case 'P':
    setCurrentProc("all");
    switchContentMode("procs");
    m_update_pending = true;
    break;
  case 'x':
  case 'X':
    postAppCastRequest(currentNode(), currentProc(), "", "any", 1);
    break;
  case 'e':
  case 'E':
    if(m_content_mode == "help")
      switchContentMode("revert");
    m_refresh_mode   = "events";
    m_update_pending = true;
    break;
  case 'n':
  case 'N':
    switchContentMode("nodes");
    m_update_pending = true;
    setCurrentNode("all");
    setCurrentProc("all");
    break;
  case 's':
  case 'S':
    if(m_content_mode == "help")
      switchContentMode("revert");
    m_refresh_mode = "streaming";
    break;
  case 'h':
  case 'H':
    if(m_content_mode == "help")
      switchContentMode("revert");
    else
      switchContentMode("help");
    m_update_pending = true;
    break;
  default: 
    {// By default, handle as possible channel switch
      string id(1,c); // create a string from the char
      if(m_content_mode == "nodes") {
	handleSelectNode(id);
      }
      else if(m_content_mode == "procs") {
	handleSelectChannel(id);
      }
    }
    break;
  }
}

//------------------------------------------------------------
// Procedure: handleSelectNode()
//   Example: str = "node=henry,app=pHostInfo,duration=10,key=uMAC_438"

void AppCastMonitor::handleSelectNode(string id)
{
  // Part 1: Try to change the present node name
  bool valid = m_repo.setCurrentNode(id);
  if(!valid)
    return;

  // Part 2: Notify the source of the present channel of the switch
  postAppCastRequest(currentNode(), currentProc(), "", "any", 0);

  m_update_pending = true;
  switchContentMode("procs");
}


//------------------------------------------------------------
// Procedure: handleSelectChannel()
//   Example: str = "node=henry,app=pHostInfo,duration=10,key=uMAC_438"

void AppCastMonitor::handleSelectChannel(string id)
{
  m_repo.setCurrentProc(id);

  //  bool changed = m_repo.setCurrentProc(id);
  //if(!changed)
  //  return;

   // Part 3: Notify the source of the present channel of the switch
  postAppCastRequest(currentNode(), currentProc(), "", "any", 0);

  // Part 4: Make the internal/local switch
  switchContentMode("appcast");
  m_update_pending = true;

   // Part 5: Notify the source of the NEW channel of the switch
  postAppCastRequest(currentNode(), currentProc(), "", "any", 10);
}


//------------------------------------------------------------
// Procedure: postAppCastRequest()
//   Example: str = "node=henry,app=pHostInfo,duration=10,key=uMAC_438"

void AppCastMonitor::postAppCastRequest(string channel_node, 
					string channel_proc, 
					string given_key, 
					string threshold, 
					double duration)
{
  string key = given_key;
  if(key == "")
    key = GetAppName();

  if((channel_node == "") || (channel_node == "")) {
    cout << "REJECTED postAppCastRequest!!!!" << endl;
    return;
  }

  if((threshold != "any") && (threshold != "run_warning")) {
    cout << "REJECTED postAppCastRequest: unrecognized threshold type" << endl;
    return;
  }

  // Notify the source of the NEW channel of the switch
  string str = "node=" + channel_node;
  str += ",app=" + channel_proc;
  str += ",duration=" + doubleToString(duration, 1);
  str += ",key=" + key;
  str += ",thresh=" + threshold;

  Notify("APPCAST_REQ", str);
  Notify("APPCAST_REQ_"+toupper(channel_node), str);
}


//------------------------------------------------------------
// Procedure: switchContentMode()

bool AppCastMonitor::switchContentMode(const string& new_mode) 
{
  if(new_mode == m_content_mode)
    return(false);

  if((new_mode == "revert") && (m_content_mode_prev == ""))
    return(false);

  if((new_mode != "revert") && (new_mode != "procs") &&
     (new_mode != "nodes")  && (new_mode != "appcast") &&
     (new_mode != "help"))
    return(false);

  if(new_mode == "revert") {
    string tmp = m_content_mode;
    m_content_mode = m_content_mode_prev;
    m_content_mode_prev = tmp;
  }
  else {
    m_content_mode_prev = m_content_mode;
    m_content_mode = new_mode;
  }

  return(true);
}


//---------------------------------------------------------
// Procedure: printReportNodes()
//
// ==============================================================
// uMAC:   3 Nodes   15 Channels   (161)                   PAUSED
// ==============================================================
// AppCasts Recd: 75
// 
// ID   Node Name      Helm           Recd   Config     Run     
//                     Mode                  Warnings   Warnings
// ---  -------------  ------         ----   ------     --------
//  0   shoreside      n/a            18     3          17
//  1   henry          Loitering      40     1          1
//  2   gilda          PARK           33     0          0
//  3   ike            PARK           5      0          0
//  4   jake           Station-Keep   6      4          0

void AppCastMonitor::printReportNodes()
{
  printHeader();
  if(!m_terse_mode) 
    cout << "===================================================================" << endl;
  else
    cout << "==================================================" << endl;
  cout << "AppCasts Recd: " << m_repo.actree().getTreeAppCastCount() << endl;
  cout << endl;
  
  ACTable actab(6,2); // 6 columns, 2 blanks separating
  if(!m_terse_mode) {
    actab << "ID | Node Name | Helm | Recd  | Config   | Run     ";
    actab << "   |           | Mode |       | Warnings | Warnings";
  }
  else
    actab << "ID | Node | HMode | Recd  | CWarns   | RWarns     ";

  actab.addHeaderLines();
  
  vector<string> svector = m_repo.actree().getNodeIDs();
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string id = svector[i];
    string node = m_repo.actree().getNodeNameFromID(id);

    actab << id << node << "HMODE";

    unsigned int appcasts_cnt, cwarning_cnt, rwarning_cnt;
    appcasts_cnt = m_repo.actree().getNodeAppCastCount(node);
    cwarning_cnt = m_repo.actree().getNodeCfgWarningCount(node);
    rwarning_cnt = m_repo.actree().getNodeRunWarningCount(node);

    string s_appcasts_cnt = " " + uintToString(appcasts_cnt) + " ";
    string s_cwarning_cnt = " " + uintToString(cwarning_cnt) + " ";
    string s_rwarning_cnt = " " + uintToString(rwarning_cnt) + " ";

    actab << s_appcasts_cnt;

    if(cwarning_cnt == 0)
      actab.addCell(s_cwarning_cnt, "reversegreen");
    else
      actab.addCell(s_cwarning_cnt, "reversered");
    
    if(rwarning_cnt == 0)
      actab.addCell(s_rwarning_cnt, "reversegreen");
    else
      actab.addCell(s_rwarning_cnt, "reversered");

  }
  actab.print();
}



//---------------------------------------------------------
// Procedure: printReportProcs()
//
// ==============================================================
// uMAC:   5 Channels   (61)
// ==============================================================
// AppCasts Recd: 75
// 
// ID   Channel Name            Recd   Config     Run     
//                                     Warnings   Warnings
// ---  -------------           ----   ------     --------
//  0   uFldShoreBroker         4      0          4
//  a   uTimerScript            40     1          1
//  b   uTimerScript_Current    19     0          1
//  c   pHostInfo               5      0          0
//  d   uFldNodeComms           6      4          0
// 


void AppCastMonitor::printReportProcs()
{
  printHeader();
  if(!m_terse_mode) 
    cout << "===================================================================" << endl;
  else
    cout << "==================================================" << endl;

  cout << "AppCasts Recd: " << m_repo.actree().getTreeAppCastCount() << endl;
  cout << endl;
  
  string node = m_repo.getCurrentNode();
  if(node == "") {
    cout << "Could determine the present channel_node." << endl;
    return;
  }

  ACTable actab(5,3); // 5 columns, 3 blanks separating
  if(!m_terse_mode) {
    actab << "ID | Channel/App | AppCasts | Config   | Run     ";
    actab << "   | Name        | Received | Warnings | Warnings";
  }
  else 
    actab << "ID | Channel/App | AppCasts | CWarns   | RWarns     ";    
  actab.addHeaderLines();
  
  vector<string> svector = m_repo.actree().getProcIDs(node);
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
  
    string id   = svector[i];
    string proc = m_repo.actree().getProcNameFromID(node, id);
    unsigned int proc_appcast_cnt  = m_repo.actree().getProcAppCastCount(node, proc);
    unsigned int proc_cwarning_cnt = m_repo.actree().getProcCfgWarningCount(node, proc);
    unsigned int proc_rwarning_cnt = m_repo.actree().getProcRunWarningCount(node, proc);

    string s_proc_appcast_cnt  = uintToString(proc_appcast_cnt);
    string s_proc_cwarning_cnt = " " + uintToString(proc_cwarning_cnt) + " ";
    string s_proc_rwarning_cnt = " " + uintToString(proc_rwarning_cnt) + " ";

    actab << id << proc << s_proc_appcast_cnt;
 
    if(proc_cwarning_cnt == 0)
      actab.addCell(s_proc_cwarning_cnt, "reversegreen");
    else
      actab.addCell(s_proc_cwarning_cnt, "reversered");
    
    if(proc_rwarning_cnt == 0)
      actab.addCell(s_proc_rwarning_cnt, "reversegreen");
    else
      actab.addCell(s_proc_rwarning_cnt, "reversered");

  }

  actab.print();
}


//---------------------------------------------------------
// Procedure: printReportAppCast()

void AppCastMonitor::printReportAppCast()
{
  string node = currentNode();
  string proc = currentProc();

  if(m_repo.actree().hasNodeProc(node, proc) == false) {
    cout << "Couldnt find appcast for node/proc: " << node << "/" << proc << endl;
    return;
  }

  printHeader();
  AppCast acast = m_repo.actree().getAppCast(node, proc);
  unsigned int cfg_cnt = acast.getCfgWarningCount();
  unsigned int run_cnt = acast.getRunWarningCount();

  // Build a headerline with colors
  string title = acast.getProcName() + " " + acast.getNodeName();
  string iter  = "(" + uintToString(acast.getIteration()) + ")";
  string warns = uintToString(cfg_cnt) + "/" + uintToString(run_cnt);
  
  unsigned int max_len = 67;
  unsigned int now_len = title.length() + iter.length() + warns.length();
  string pad_str;
  if(now_len < max_len)
    pad_str = string((max_len-now_len), ' ');

  
  string colored_warns;
  if(cfg_cnt == 0) {
    colored_warns += termColor("reverse_green");
    colored_warns += uintToString(cfg_cnt);
  }
  else {
    colored_warns += termColor("reverse_green");
    colored_warns += uintToString(cfg_cnt);
  }
  colored_warns += termColor();
  colored_warns += "/";
  if(run_cnt == 0) {
    colored_warns += termColor("reverse_green");
    colored_warns += uintToString(run_cnt);
  }
  else {
    colored_warns += termColor("reverse_green");
    colored_warns += uintToString(run_cnt);
  }
  colored_warns += termColor();

  cout << "==================================================================="<< endl;
  cout << title << pad_str << colored_warns << iter                            << endl;
  cout << "==================================================================="<< endl;


  vector<string> lines = parseString(acast.getFormattedString(false), '\n');
  for(unsigned int i=0; i<lines.size(); i++)
    cout << lines[i] << endl;
}

//---------------------------------------------------------
// Procedure: printHeader()

void AppCastMonitor::printHeader()
{
  m_term_reports++;
  unsigned int nodes = m_repo.actree().getTreeNodeCount();

  string header = GetAppName() + ":";
  if(m_content_mode == "procs")
    header += "  NODE=" + currentNode();
  else
    header += "  Nodes (" + uintToString(nodes) + ")";
  
  string iter = "(" + uintToString(m_term_reports)  + ") ";

  if(m_mission_hash != "") 
    iter = "[" + m_mission_hash + "] " + iter;
  
  unsigned int padlen = 67 - (m_refresh_mode.length() + iter.length());
  
  if(!m_terse_mode) {
    cout << endl << endl << endl << endl << endl << endl << endl;
    cout << "===================================================================" << endl;
  }
  else {
    cout << endl << endl;
    cout << "==================================================";
    cout << endl;
    padlen -= 17;
  }
    
  header = padString(header, padlen, false);
  cout << header + iter;
  if(m_refresh_mode == "paused") 
    cout << termColor("reversered") << "PAUSED" << termColor() << endl;
  else if(m_refresh_mode == "events") 
    cout << termColor("reversegreen") << "EVENTS" << termColor() << endl;
  else
    cout << termColor("reversegreen") << "STREAMING" << termColor() << endl;
}

//------------------------------------------------------------
// Procedure: printHelp()

void AppCastMonitor::printHelp()
{
  string refstr = termColor("reversered") + "HELP" + termColor();
  string mode_str = "(" + refstr + ")";   

  printf("\n\n");
  
  printf("KeyStroke    Function         %s      \n", mode_str.c_str());
  printf("---------    ---------------------------              \n");
  printf("    h        Content Mode: Help. Hit 'R' to resume    \n");
  printf("    n        Content Mode: List publishing nodes      \n");
  printf("    p        Content Mode: List publishing processes  \n");
  printf("                                                      \n");
  printf("   u/SPC     Refresh Mode: Update then Pause          \n");
  printf("    e        Refresh Mode: Events                     \n");
  printf("    s        Refresh Mode: Streaming                  \n");
  
  m_refresh_mode = "paused";
}

//------------------------------------------------------------
// Procedure: currentNode()

string AppCastMonitor::currentNode() const
{
  string node = m_repo.getCurrentNode();
  if(node == "")
    node = "all";
  return(node);
}

//------------------------------------------------------------
// Procedure: currentProc()

string AppCastMonitor::currentProc() const
{
  string proc = m_repo.getCurrentProc();
  if(proc == "")
    proc = "all";
  return(proc);
}










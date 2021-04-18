/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: MemWatch.cpp                                    */
/*    DATE: May 4th, 2019                                   */
/************************************************************/

#include <cstdlib>
#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MemWatch.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MemWatch::MemWatch()
{
  m_last_measure_ix = 0;
  m_total_measures  = 0;

  m_absolute_time_gap = 5;   // Five seconds between sys calls
  m_last_check_tstamp = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MemWatch::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == "DB_CLIENTS") 
      handleMailDBClients(sval);
    else if(strEnds(key, "_PID"))
      handleMailInfoPID(key, dval);
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MemWatch::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MemWatch::Iterate()
{
  AppCastingMOOSApp::Iterate();

  double delta_time = (m_curr_time - m_last_check_tstamp);
  if(m_time_warp > 0) {
    delta_time = delta_time / m_time_warp;
    if(delta_time > m_absolute_time_gap) {
      measureMemory();
      m_last_check_tstamp = m_curr_time;
    }
  }
      
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MemWatch::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "absolute_time_gap")
      handled = handleConfigTimeGap(value);
    else if(param == "watch_only") 
      handled = handleConfigWatchOnly(value);
    else if(param == "ignore")
      handled = handleConfigIgnore(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailDBClients

void MemWatch::handleMailDBClients(string clients)
{
  vector<string> svector = parseString(clients, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string client = stripBlankEnds(svector[i]);
    if(!vectorContains(m_apps_known_to_db, client)) {
      m_apps_known_to_db.push_back(client);

      string client_var_pid = toupper(client) + "_PID";
      Register(client_var_pid);
    }
  }
}

//---------------------------------------------------------
// Procedure: measureMemory()

void MemWatch::measureMemory()
{
  // Sanity check
  if(m_app.size() == 0)
    return;

  // Part 1: Cycle to next app to measure, reset to 0 if needed
  unsigned int ix = m_last_measure_ix + 1;
  if(ix >= m_app.size())
    ix = 0;

  // Part 2: Build the system call from the app and pid info
  //         Make the system call
  string app = m_app[ix];
  string pid = m_pid[ix];
  string tmp_file = ".mem_info_" + tolower(app) + "_" + pid;
  string syscall = "appmem.sh --pid=" + m_pid[ix];
  syscall += " > " + tmp_file;  
  system(syscall.c_str());

  // Part 3: Get the output of the system call
  vector<string> lines = fileBuffer(tmp_file);
  if(lines.size() == 0)
    return;

  // Part 4: Remove temporary file holding first sys call output
  string syscall_rm = "rm -f " + tmp_file + " &";
  system(syscall_rm.c_str());
  
  // Part 5: Intpret the output. Normall this is just an integer
  //         representing the mem size in Kilobytes. But we also
  //         handle the case where the output has a unit suffix,
  //         or ends in a '+' like the output sometimes of top.
  bool result_in_kb = false;
  bool result_in_gb = false;
  string result = lines[0];

  m_last_app = app;
  m_last_result_raw = result;
  
  result = biteString(result, '+');
  if(strEnds(result, "K")) {
    result_in_kb = true;
    result = biteString(result, 'K');
  }
  else if(strEnds(result, "G")) {
    result_in_gb = true;
    result = biteString(result, 'G');
  }
  else
    result_in_kb = true;
    

  // Part 6: Determine the memory size perhaps applying units.
  double raw_num = atof(result.c_str());
  double mbs = raw_num;
  if(result_in_kb)
    mbs = raw_num / 1000.0;
  if(result_in_gb)
    mbs = raw_num * 1000.0;

  m_last_mem = doubleToString(mbs,3);
  m_last_measure_ix = ix;

  // Part 7: Calculate and note the changes/deltas
  double prev = m_mem[ix];
  double delta = 0;
  if(prev > 0) {
    delta = mbs - prev;
    m_delta[ix] = delta;
  }
  m_mem[ix] = mbs;

  // Part 8: Finally, Post the memory for this app
  string varname = toupper(app) + "_MEM";
  Notify(varname, mbs);
}

//---------------------------------------------------------
// Procedure: handleMailInfoPID
//   Example: PHELMIVP_PID = 8730

void MemWatch::handleMailInfoPID(string app, double pid)
{
  app = biteStringX(app, '_');
  
  // Part 1: What is this app's true name with proper case
  bool found = false;
  for(unsigned int i=0; i<m_apps_known_to_db.size(); i++) {
    if(tolower(app) == tolower(m_apps_known_to_db[i])) {
      app = m_apps_known_to_db[i];
      found = true;
    }
  }
  if(!found)
    return;
  
  // Part 2: Determine if it already exists in our records
  bool pid_already_known = false;
  for(unsigned int i=0; i<m_app.size(); i++) {
    if(tolower(app) == tolower(m_app[i])) {
      pid_already_known = true;
    }
  }
  if(pid_already_known)
    return;

  // Part 3: If the watch list is non-empty, make sure this
  // app is on the watch list
  if(m_apps_watch_only.size() > 0) {
    if(!vectorContains(m_apps_watch_only, app))
      return;
  }

  // Part 4: If the ignore list is non-empty, make sure this
  // app is on NOT on the ignore list
  if(m_apps_ignore.size() > 0) {
    if(vectorContains(m_apps_watch_only, app))
      return;
  }
  
  // Part 5: If this is new PID info, augment the records
  m_app.push_back(app);
  m_pid.push_back(doubleToStringX(pid,0));
  m_mem.push_back(0);
  m_delta.push_back(0);
}


//---------------------------------------------------------
// Procedure: handleConfigTimeGap()

bool MemWatch::handleConfigTimeGap(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval >= 1) {
    m_absolute_time_gap = dval;
    return(true);
  }

  m_absolute_time_gap = 1;
  return(false);
}

//---------------------------------------------------------
// Procedure: handleConfigWatchOnly()

bool MemWatch::handleConfigWatchOnly(string str)
{
  vector<string> svector = parseString(str, ',');
  if(svector.size() == 0)
    return(false);

  bool all_ok = true;
  for(unsigned int i=0; i<svector.size(); i++) {
    string app = stripBlankEnds(svector[i]);
    bool ok = true;
    if((app == "") || strContainsWhite(app))
      ok = false;
    // A watch app should not also be on the ignore list
    if(vectorContains(m_apps_ignore, app))
      ok = false;
    // No duplicates
    if(vectorContains(m_apps_watch_only, app))
      ok = false;
    if(ok)
      m_apps_watch_only.push_back(app);
    all_ok = all_ok && ok;
  }
  return(all_ok);
}

//---------------------------------------------------------
// Procedure: handleConfigIgnore()

bool MemWatch::handleConfigIgnore(string str)
{
  vector<string> svector = parseString(str, ',');
  if(svector.size() == 0)
    return(false);

  bool all_ok = true;
  for(unsigned int i=0; i<svector.size(); i++) {
    string app = stripBlankEnds(svector[i]);
    bool ok = true;
    if((app == "") || strContainsWhite(app))
      ok = false;
    // An ignore app should not also be on the watch list
    if(vectorContains(m_apps_watch_only, app))
      ok = false;
    // No duplicates
    if(vectorContains(m_apps_ignore, app))
      ok = false;
    if(ok)
      m_apps_ignore.push_back(app);
    all_ok = all_ok && ok;
  }
  return(all_ok);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MemWatch::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_CLIENTS", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MemWatch::buildReport() 
{

  string watch_apps_str  = svectorToString(m_apps_watch_only);
  string ignore_apps_str = svectorToString(m_apps_ignore);
  string time_gap_str = doubleToStringX(m_absolute_time_gap,2);
  if(watch_apps_str == "")
    watch_apps_str = "None";
  if(ignore_apps_str == "")
    ignore_apps_str = "None";
    
  m_msgs << "Configuration:     " << endl;
  m_msgs << "  Watch Only Apps: " << watch_apps_str << endl;
  m_msgs << "      Ignore Apps: " << ignore_apps_str << endl;
  m_msgs << "Absolute Time Gap: " << time_gap_str << endl;
  m_msgs << endl;
    
  ACTable actab(4);
  actab << "App | PID | Memory | Delta";
  actab.addHeaderLines();
  for(unsigned int i=0; i<m_app.size(); i++) {
    string app = m_app[i];
    string pid = m_pid[i];
    string mem = doubleToString(m_mem[i],3);
    string delta = doubleToString(m_delta[i],3);
    if(m_delta[i] == 0)
      delta = "-";

    actab << app << pid << mem << delta;
  }
  m_msgs << actab.getFormattedString() << endl << endl;

  m_msgs << " Last ix: " << m_last_measure_ix << endl;
  m_msgs << "Last app: " << m_last_app << endl;
  m_msgs << "Last mem: " << m_last_mem << endl;
  m_msgs << "Last raw: " << m_last_result_raw << endl;
  m_msgs << "Warp: " << m_time_warp << endl;
  
  return(true);
}





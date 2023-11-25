/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Realm.cpp                                            */
/*    DATE: December 2nd, 2020                                   */
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

#include <sstream>
#include "MBUtils.h"
#include "Realm.h"
#include "RealmCast.h"
#include "WatchCast.h"
#include "RealmSummary.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Realm::Realm()
{
  // Init Config Variables
  m_msg_max_hist = 10;

  m_relcast_interval = 0.8;
  m_summary_interval = 5;

  m_wrap_length = 90;
  m_trunc_length = 270;

  // Init State Variables

  m_last_post_relcast = 0;
  m_last_post_summary = 0;

  m_total_realmcasts = 0;
  m_total_watchcasts = 0;
  
  m_time_warp = 1;

  m_new_app_noticed = false;
  m_summaries_posted = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Realm::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString();

    if(key == "DB_RWSUMMARY") 
      handleMailDBRWSummary(sval);
    else if(key == "REALMCAST_REQ") 
      handleMailRealmCastReq(sval);

    handleGeneralMail(msg);
  }

  AppCastingMOOSApp::OnNewMail(NewMail);
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Realm::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Realm::Iterate()
{
  AppCastingMOOSApp::Iterate();
  buildRealmCast();

  buildRealmCastSummary();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Realm::OnStartUp()
{
  string directives = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "relcast_interval")
      handled = setDoubleRngOnString(m_relcast_interval, value, 0.4, 15);
    else if(param == "summary_interval")
      handled = setDoubleRngOnString(m_summary_interval, value, 1, 10);
    else if(param == "wrap_length")
      handled = setPosUIntOnString(m_wrap_length, value);
    else if(param == "trunc_length")
      handled = setPosUIntOnString(m_trunc_length, value);
    else if((param == "msg_max_hist") || (param == "msg_max_history"))
      handled = setPosUIntOnString(m_msg_max_hist, value);
    else if(param == "hist_var")
      handled = handleConfigHistVar(value);
    else if(param == "scope_set")
      handled = handleConfigScopeSet(value);
    else
      handled = false;

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  m_set_apps.insert("MOOSDB_" + m_host_community);
  
  m_moosdb_name = "MOOSDB_" + m_host_community;

  m_time_warp = GetMOOSTimeWarp();
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Realm::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DB_RWSUMMARY", 0);
  Register("REALMCAST_REQ", 0);
}


//---------------------------------------------------------
// Procedure: handleConfigHistVar()

bool Realm::handleConfigHistVar(string var)
{
  if(strContainsWhite(var))
    return(false);

  // If this is a duplicate, we just quietly ignore it with no warning
  if(m_set_hist_vars.count(var))
    return(true);

  // For history vars we register immediately. Don't wait for the
  // varsummary to act.
  Register(var, 0);
  
  m_set_hist_vars.insert(var);

  m_map_scope_sets[var].insert(var);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigScopeSet()
//   Example: scope_set = name=life, vars=FOO:BAR:DEPLOY

bool Realm::handleConfigScopeSet(string scope_set)
{

  // Part 1: Parse the config line and syntax check
  string name;
  set<string> vars;

  vector<string> svector = parseString(scope_set, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "name")
      name = value;
    else if(param == "vars") {
      vector<string> jvector = parseString(value, ':');
      for(unsigned int j=0; j<jvector.size(); j++)
	vars.insert(jvector[j]);
    }
    else
      return(false);
  }
  if((name == "") || (vars.size() == 0))
    return(false);

  // Part 2: update the scopeset map and set of history vars
  set<string>::iterator p;
  for(p=vars.begin(); p!=vars.end(); p++) {
    string var = *p;

    // Part 2A: If hist var unknown, add it, and register
    if(m_set_hist_vars.count(var) == 0) {
      m_set_hist_vars.insert(var);
      Register(var, 0);
    }

    // Part 2B: Add hist var to appropriate scope_set
    m_map_scope_sets[name].insert(var);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailDBRWSummary()
// Structure: App=sub:sub:...:sub & pub:pub:...:pub,
//            App=sub:sub:...:sub & pub:pub:...:pub, ...

void Realm::handleMailDBRWSummary(string str)
{
  vector<string> apps = parseString(str, ',');
  for(unsigned int i=0; i<apps.size(); i++) {
    string app = biteStringX(apps[i], '=');

    if(m_set_apps.count(app) == 0) {
      m_set_apps.insert(app);
      m_new_app_noticed = true;
    }
    
    string subs_all = biteStringX(apps[i], '&');
    string pubs_all = apps[i];

    vector<string> subs = parseString(subs_all, ':');
    for(unsigned int j=0; j<subs.size(); j++) {
      m_map_subs[app].insert(subs[j]);
      if(m_set_realm_subs.count(subs[j]) == 0) {
	Register(subs[j], 0);
	m_set_realm_subs.insert(subs[j]);
	m_set_unique_vars.insert(subs[j]);
      }
    }
 
    vector<string> pubs = parseString(pubs_all, ':');
    for(unsigned int j=0; j<pubs.size(); j++) {
      m_map_pubs[app].insert(pubs[j]);
      m_set_realm_pubs.insert(pubs[j]);
      m_set_unique_vars.insert(pubs[j]);
    }
  }
}

//---------------------------------------------------------
// Procedure: handleMailRealmCastReq()
//   Example: channel=pNodeReporter,

void Realm::handleMailRealmCastReq(string sval)
{
  PipeWay pipeway = string2PipeWay(sval);
  if(!pipeway.valid()) {
    reportRunWarning("Bad RealmCastReq:" + sval);
    return;
  }

  string client = pipeway.getClient();

  if(m_map_pipeways.count(client) == 0) {
    m_map_pipeways[client] = pipeway;
  }
  else {  
    if(m_map_pipeways[client] != pipeway) {
      m_map_pipeways[client] = pipeway;
      string pinfo = pipeway.getContentDescriptor();
      reportEvent("Client " + client + ", new pipe: " + pinfo);
    }
  }
  
  m_map_pipeways[client].setStartExpTime(m_curr_time, m_time_warp);
}

//---------------------------------------------------------
// Procedure: handleGeneralMail()

void Realm::handleGeneralMail(const CMOOSMsg& msg)
{
  string key = msg.GetKey();
  string src = msg.GetSource();
  
  // Autodetect what the MOOSDB is publishing so the MOOSDB can have
  // its own app channel
  if(src == m_moosdb_name)
    m_map_pubs[m_moosdb_name].insert(key);
      
  m_map_data[key].push_front(msg);

  // If message is a Double, we will keep a bunch as a history. Otherwise
  // if not a Double, keep just the most recent.
  if(msg.IsDouble()) {
    if(m_map_data[key].size() > m_msg_max_hist)
      m_map_data[key].pop_back();
  }
  else {
    unsigned int max_hist = 1;       // default for strings
    if(m_set_hist_vars.count(key))
      max_hist = m_msg_max_hist;     // except if it is a history variable
    
    if(m_map_data[key].size() > max_hist)
      m_map_data[key].pop_back();
  }

}



//---------------------------------------------------------
// Procedure: buildRealmCast()

void Realm::buildRealmCast()
{
  // Part 1: Determine elapsed time since last post (or group-post)
  double elapsed = m_curr_time - m_last_post_relcast;
  elapsed = elapsed / m_time_warp;
  if(elapsed < m_relcast_interval)
    return;
  
  // Part 2: Determine for each client if the pipeway has expired or
  // not. If not, then generate a realmcast for the channel associated
  // with that pipeway. If the pipeway has expired, make note of the
  // client key, and cleanup afterwards. 
  vector<string> expired_clients;
       
  map<string, PipeWay>::iterator p;
  for(p=m_map_pipeways.begin(); p!=m_map_pipeways.end(); p++) {
    string  client = p->first;
    PipeWay pipeway = p->second;

    double time_until_expire = pipeway.timeUntilExpire(m_curr_time);
    
    if(time_until_expire > 0) {
      bool posted = false;
      if(pipeway.isChanneled()) 
	posted = buildRealmCastChannel(pipeway);
      else 
	posted = buildWatchCast(pipeway);

      if(posted) {
	string info_str = client + ":" + pipeway.getContentDescriptor(55);
	addLatestOutCast(info_str);
	m_last_post_relcast = m_curr_time;
      }
    }
    else if(time_until_expire < -30)
      expired_clients.push_back(client);
  }

  // Part 3: For client pipeways that have indeed expired, remove
  // these entries from the map.
  for(unsigned int i=0; i<expired_clients.size(); i++) {
    string client = expired_clients[i];
    m_map_pipeways.erase(client);
  }
}


//---------------------------------------------------------
// Procedure: buildWatchCast()
//   Returns: true if Posting is posted

bool Realm::buildWatchCast(PipeWay pipeway)
{
  // Sanity checks
  if(pipeway.isChanneled())
    return(false);

  bool posted = false;
  set<string> vars = pipeway.getVars();
  set<string>::iterator p;
  for(p=vars.begin(); p!=vars.end(); p++) {
    string var = *p;

    bool has_entry = false;
    CMOOSMsg msg;
    
    if(m_map_data.count(var) != 0) {
      list<CMOOSMsg> msgs = m_map_data[var];
      if(msgs.size() != 0) {
	msg = msgs.front();
	has_entry = true;
      }
    }

    double msg_utc_time = msg.GetTime();
    bool force_refresh = pipeway.forceRefresh();
    
    if(!force_refresh && (msg_utc_time < m_map_var_last_wcast[var]))
      continue;
    
    WatchCast wcast;
    wcast.setVarName(var);
    wcast.setNode(m_host_community);

    if(has_entry) {
      wcast.setCommunity(msg.GetCommunity());
      wcast.setSource(msg.GetSource());
      wcast.setLocTime(msg_utc_time - m_start_time);
      wcast.setUtcTime(msg_utc_time);
      if(msg.IsString())
	wcast.setSVal(msg.GetString());
      else if(msg.IsDouble())
	wcast.setDVal(msg.GetDouble());
      m_map_var_last_wcast[var] = m_curr_time;
    }
    else {
      wcast.setCommunity("n/a");
      wcast.setSource("n/a");
      wcast.setLocTime(-1);
      wcast.setSVal("n/a");
    }

    posted = true;
    m_total_watchcasts++;
    string wcast_str = wcast.get_spec();
    Notify("WATCHCAST", wcast_str);
  }
  return(posted);
}


//---------------------------------------------------------
// Procedure: buildRealmCastChannel

bool Realm::buildRealmCastChannel(PipeWay pipeway)
{
  string channel = pipeway.getChannel();
  if(channel == "")
    return(false);
    
  stringstream ss;

  bool hist_channel = false;
  if(m_map_scope_sets.count(channel)) {
    list<CMOOSMsg> msgs;
    string all_vars;
    set<string> hist_vars = m_map_scope_sets[channel];
    set<string>::iterator q;
    for(q=hist_vars.begin(); q!=hist_vars.end(); q++) {
      string var = *q;
      if(all_vars != "")
	all_vars += ",";
      all_vars += var;
      
      list<CMOOSMsg> var_msgs = m_map_data[var];
      msgs.merge(var_msgs);
    }
    msgs.sort();
    msgs.reverse();

    ss << "History Channel [" << channel << "]: " << all_vars << endl;
    ss << "=======================================" << endl;
    resetACTable(pipeway);
    
    list<CMOOSMsg>::iterator p;
    for(p=msgs.begin(); p!=msgs.end(); p++) {
      CMOOSMsg msg = *p;
      string   key   = msg.GetKey();
      string   src   = msg.GetSource();
      string   comm  = msg.GetCommunity();
      double   dtime = msg.GetTime();
      if(!pipeway.timeFormatUTC())
	dtime = dtime - m_start_time;
      
      string   stime = doubleToString(dtime, 2);
      string   sval  = "formatted report";
      if(msg.IsDouble())
	sval = doubleToStringX(msg.GetDouble(),6);
      else if(msg.IsBinary())
	sval = "binary";
      else 
	sval  = msg.GetString();
      
      addRowACTab(pipeway, key, src, stime, comm, sval);
    }
    ss << m_actab.getFormattedString(true) << endl << endl;
    hist_channel = true;
  }
  
  set<string> pubs = m_map_pubs[channel];  
  if(pipeway.showSubscriptions() && m_map_subs.count(channel)) {
    set<string> subs = m_map_subs[channel];
    ss << "Subscriptions" << endl;
    ss << "=======================================" << endl;
    resetACTable(pipeway);
    
    set<string>::iterator p;
    for(p=subs.begin(); p!=subs.end(); p++) {
      string var = *p;
      
      if(pipeway.showMasked() && (m_map_data.count(var) == 0))
	addRowACTab(pipeway, var, "-", "never", "-", "-");
      
      if(m_map_data.count(var) != 0) {
	CMOOSMsg msg   = m_map_data[var].front();
	string   src   = msg.GetSource();
	string   comm  = msg.GetCommunity();
	double   dtime = msg.GetTime();
	if(!pipeway.timeFormatUTC())
	  dtime = dtime - m_start_time;
	  
	string   stime = doubleToString(dtime, 2);
	string   sval  = "formatted report";
	if(msg.IsDouble())
	  sval = doubleToStringX(msg.GetDouble(),6);
	else if(msg.IsBinary())
	  sval = "binary";
	else if(!isIgnoreVar(var)) 
	  sval  = msg.GetString();
	
	addRowACTab(pipeway, var, src, stime, comm, sval);
      }
    }
    ss << m_actab.getFormattedString(true) << endl << endl;
  }

  if(m_map_pubs.count(channel) && !hist_channel) {
    set<string> pubs = m_map_pubs[channel];
    // if not showing subscriptions, we don't need the upper bar
    if(pipeway.showSubscriptions())
      ss << "=======================================" << endl;
    ss << "Publications" << endl;
    ss << "=======================================" << endl;
    
    resetACTable(pipeway);
    
    set<string>::iterator q;
    for(q=pubs.begin(); q!=pubs.end(); q++) {
      string var = *q;
      
      if(m_map_data.count(var) != 0) {
	CMOOSMsg msg   = m_map_data[var].front();
	string   src   = msg.GetSource();
	string   comm  = msg.GetCommunity();
	double   dtime = msg.GetTime();
	if(!pipeway.timeFormatUTC())
	  dtime = dtime - m_start_time;
	
	string   stime = doubleToString(dtime, 2);
	string   sval  = "formatted report";
	if(msg.IsDouble())
	  sval = doubleToStringX(msg.GetDouble(),6);
	else if(msg.IsBinary())
	  sval = "binary";
	else if(!isIgnoreVar(var))
	  sval  = msg.GetString();
	
	addRowACTab(pipeway, var, src, stime, comm, sval);
      }
    }
    ss << m_actab.getFormattedString(true) << endl;
  }
  
  RealmCast relcast;
  if(strContains(channel, "uQueryDB"))
    channel = "uQueryDB";

  relcast.setNodeName(m_host_community);
  relcast.setProcName(channel);
  relcast.msg(ss.str());
  
  m_total_realmcasts++;
  string relcast_str = relcast.getRealmCastString();
  Notify("REALMCAST", relcast_str);
  return(true);
}


//---------------------------------------------------------
// Procedure: buildRealmCastSummary()
//   Purpose: Every once in a while we update REALMCAST
//            recipients with the list of known channels

void Realm::buildRealmCastSummary()
{
  // ======================================================
  // Part 1: Determine if it is time to post a summary
  // ======================================================
  double elapsed = m_curr_time - m_last_post_summary;
  elapsed = elapsed / m_time_warp;

  // By default summery interval configured by the user.
  double summary_interval = m_summary_interval;

  // But, for first minute, interval will be much more frequent.
  if((m_curr_time - m_start_time) < 60)
    summary_interval = 0.5;

  // If, on the most recent Mail handling, a new app has been noticed
  // then a summary message will be posted immediately.
  if(m_new_app_noticed) {
    summary_interval = 0;
    m_new_app_noticed = false;
  }
    
  if(elapsed < summary_interval)
    return;
  
  // ======================================================
  // Part 2: Build summary: apps + history channels
  // ======================================================
  RealmSummary realm_summary;
  realm_summary.setNode(m_host_community);
  
  set<string>::iterator p;
  for(p=m_set_apps.begin(); p!=m_set_apps.end(); p++) {
    string app = *p;

    if(strContains(app, "uQueryDB") ||
       strContains(app, "uPokeDB") ||
       strContains(app, "uXMS") ||
       strContains(app, "uMAC"))
      continue;
    
    realm_summary.addProc(app);
  }

  map<string, set<string> >::iterator q;
  for(q=m_map_scope_sets.begin(); q!=m_map_scope_sets.end(); q++) {
    string channel = q->first;
    realm_summary.addHistVar(channel);
  }

  // If the summary has not changed, the interval time is 10x
  string new_summary = realm_summary.get_spec();
  if(new_summary == m_last_post_summary_info) {
    if(elapsed < (summary_interval * 10))
      return;
  }

  m_last_post_summary_info = new_summary;
  m_last_post_summary = m_curr_time;
  m_summaries_posted++;
  
  Notify("REALMCAST_CHANNELS", new_summary);
}

//------------------------------------------------------------
// Procedure: resetACTable()

void Realm::resetACTable(PipeWay pipeway)
{
  int table_cols = 5;
  string header = "Variable | Source | Time | Comm | Value";
  if(!pipeway.showSource()) {
    table_cols--;
    header = findReplace(header,"Source |", "");
  }
  if(!pipeway.showCommunity()) {
    table_cols--;
    header = findReplace(header,"Comm |", "");
  }

  ACTable actab(table_cols);
  actab << header;
  actab.addHeaderLines();

  m_actab = actab;
}


//------------------------------------------------------------
// Procedure: addRowACTab()

void Realm::addRowACTab(PipeWay pipeway, string var, string source,
			string time, string comm, string value)
{
  if(!pipeway.showSource())
    source  = "_ignore_";
  if(!pipeway.showCommunity())
    comm = "_ignore_";

  if(pipeway.truncContent())
    value = value.substr(0, m_trunc_length);
  
  if(strEnds(var, "_STATUS") && strContains(value, "MOOSName")) {
    value = findReplace(value, "MOOSName", "$");
    value = biteString(value, '$');
  }
  
  if(!pipeway.wrapContent())
    m_actab << var << source << time << comm << value;
  else {
    vector<string> svector = breakLen(value, m_wrap_length);
    for(unsigned int i=0; i<svector.size(); i++)  {
      if(i == 0)
	m_actab << var << source << time << comm << svector[i];
      else {
	if(pipeway.showSource())
	  source = "";
	if(pipeway.showCommunity())
	  comm = "";
	m_actab << "" << source << "" << comm << svector[i];
      }
    }      
  }
}

 
//------------------------------------------------------------
// Procedure: isIgnoreVar()

bool Realm::isIgnoreVar(string var) const
{
  if((var == "APPCAST") || (var == "REALMCAST") ||
     (var == "BHV_IPF") || (var == "DB_RWSUMMARY") ||
     (var == "DB_VARSUMMARY"))
    return(true);

  return(false);
}


//------------------------------------------------------------
// Procedure: getAppsVector()

vector<string> Realm::getAppsVector() const
{
  string all_apps;
  set<string>::iterator p;
  for(p=m_set_apps.begin(); p!=m_set_apps.end(); p++) {
    string app = *p;
    if(all_apps != "")
      all_apps += ", ";
    all_apps += app;
  }
  
  vector<string> svector = justifyLen(all_apps, 50);
  for(unsigned int i=0; i<svector.size(); i++)
    svector[i] = removeWhite(svector[i]);

  return(svector);
}

//------------------------------------------------------------
// Procedure: addLatestOutCast()

void Realm::addLatestOutCast(string str)
{
  // First find out if it is different from the most recent
  bool new_outcast = true;
  string previous_outcast, amt, time;

  if(m_recent_outcasts.size() > 0) {
    string recent = m_recent_outcasts.front();
    amt  = biteStringX(recent, ' ');
    time = biteStringX(recent, ' ');
    previous_outcast = recent;
    if(previous_outcast == str)
      new_outcast = false;
  }

  double curr_loc_time = m_curr_time - m_start_time;
  string curr_str_time = doubleToString(curr_loc_time,2);

  string new_entry;
  if(new_outcast) {
    new_entry = "1 " + curr_str_time + " " + str;
    m_recent_outcasts.push_front(new_entry);
  }
  else {
    int int_amt = atoi(amt.c_str());
    string str_amt = intToString(int_amt+1);
    new_entry = "" + str_amt + " " + curr_str_time + " " + str;
    if(m_recent_outcasts.size() == 0)
      m_recent_outcasts.push_front(new_entry);
    else
      m_recent_outcasts.front() = new_entry;
  }
  
  if(m_recent_outcasts.size() > 5)
    m_recent_outcasts.pop_back();
}


//------------------------------------------------------------
// Procedure: getHistoryVars()

string Realm::getHistoryVars() const
{
  string result;
  set<string>::iterator p;
  for(p=m_set_hist_vars.begin(); p!=m_set_hist_vars.end(); p++) {
    string var = *p;
    if(result != "")
      result += ",";
    result += var;
  }

  return(result);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Realm::buildReport() 
{
  m_msgs << "Configuration: " << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "  RelCast Interval: " << doubleToString(m_relcast_interval,2) << endl;
  m_msgs << "  Summary Interval: " << doubleToString(m_summary_interval,2) << endl;
  m_msgs << "  Hist Variables:   " << getHistoryVars() << endl;
  m_msgs << "  Wrap Length:  " << uintToString(m_wrap_length) << endl;
  m_msgs << "  Trunc Length: " << uintToString(m_trunc_length) << endl;
  m_msgs << "  Max Msg Hist: " << uintToString(m_msg_max_hist) << endl;

  m_msgs << endl;
  m_msgs << "MOOS Community State: " << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "  MOOSDB Name: " << m_moosdb_name << endl;
  m_msgs << "   Known Apps: " << uintToString(m_set_apps.size()) << endl;

  vector<string> all_apps = getAppsVector();
  for(unsigned int i=0; i<all_apps.size(); i++) {
    if(i==0)
      m_msgs << "   Known Apps: " << all_apps[i] << endl;
    else
      m_msgs << "               " << all_apps[i] << endl;
  }    
  m_msgs << "  Total SVars: " << uintToString(m_set_realm_subs.size()) << endl;
  m_msgs << "  Total PVars: " << uintToString(m_set_realm_pubs.size()) << endl;
  m_msgs << "  Unique Vars: " << uintToString(m_set_unique_vars.size()) << endl;
  m_msgs << "  UTC Time:    " << doubleToString(m_curr_time, 2) << endl;
  m_msgs << "  Local Time:  " << doubleToString(m_curr_time-m_start_time, 2) << endl;
  m_msgs << "  Summaries:   " << uintToString(m_summaries_posted) << endl;

  m_msgs << endl;
  m_msgs << "Recent RealmCasts or WatchCasts:" << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "  Total RealmCasts: " << uintToString(m_total_realmcasts) << endl;
  m_msgs << "  Total WatchCasts: " << uintToString(m_total_watchcasts) << endl;
  m_msgs << "--------------------------------------------" << endl;

  ACTable wctab(4);
  wctab << "Count | Time | Client | Content";
  wctab.addHeaderLines();
  wctab.setColumnJustify(1, "right");
  list<string>::iterator p;
  for(p=m_recent_outcasts.begin(); p!=m_recent_outcasts.end(); p++) {
    string entry = *p;
    string amt  = "(" + biteStringX(entry, ' ') + ")";
    string time = biteStringX(entry, ' ');
    string client = biteStringX(entry, ':');
    string info = entry;
    wctab << amt << time << client << info;
  }
  m_msgs << wctab.getFormattedString();

  
  m_msgs << endl;
  m_msgs << "Currently Active Clients:" << endl;
  m_msgs << "--------------------------------------------" << endl;
  ACTable actab(3);
  actab << "Client | Active | Content";
  actab.addHeaderLines();

  map<string, PipeWay>::iterator q;
  for(q=m_map_pipeways.begin(); q!=m_map_pipeways.end(); q++) {
    string client = q->first;
    PipeWay pipeway = q->second;
    double time_until_expire = pipeway.timeUntilExpire(m_curr_time);
    string content = pipeway.getContentDescriptor();
    string active = doubleToString(time_until_expire,2);
    actab << client << active << content;    
  }
    
  m_msgs << actab.getFormattedString();

  return(true);
}






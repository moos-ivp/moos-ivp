/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Realm.cpp                                       */
/*    DATE: December 2nd, 2020                              */
/************************************************************/

#include <sstream>
#include "MBUtils.h"
#include "Realm.h"
#include "RealmCast.h"
#include "RealmSummary.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Realm::Realm()
{
  // Init Config Variables
  m_channel      = "unset";
  m_msg_max_hist = 10;

  m_relcast_interval = 0.8;
  m_summary_interval = 5;

  m_wrap_length = 90;
  m_trunc_length = 270;

  // Init State Variables
  m_expire_time = 0;

  m_last_post_relcast = 0;
  m_last_post_summary = 0;
  
  m_time_warp = 1;

  m_new_app_noticed = false;
  m_summaries_posted = 0;
  
  m_show_source = true;
  m_show_community = true;
  m_wrap_content = false;

  m_show_subscriptions = true;
  m_show_masked = true;
  m_trunc_content = false;
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
    string comm   = msg.GetCommunity();
    string src    = msg.GetSource();

    if(key == "DB_RWSUMMARY") 
      handleMailDBRWSummary(sval);
    else if(key == "REALMCAST_REQ") 
      handleMailRealmCastReq(comm, src, sval);

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
    if(param == "channel")
      m_channel = value;
    else if(param == "relcast_interval")
      handled = setDoubleRngOnString(m_relcast_interval, value, 0.4, 15);
    else if(param == "summary_interval")
      handled = setDoubleRngOnString(m_summary_interval, value, 1, 10);
    else if(param == "wrap_length")
      handled = setPosUIntOnString(m_wrap_length, value);
    else if(param == "trunc_length")
      handled = setPosUIntOnString(m_trunc_length, value);
    else if(param == "msg_max_hist")
      handled = setPosUIntOnString(m_msg_max_hist, value);
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

void Realm::handleMailRealmCastReq(string comm, string src, string sval)
{
  string prev_channel = m_channel;

  // Prior to handling each request, we revert back to our defaults
  // for source, community and wrapping. Deviation from the defaults
  // must be explicitly requested on each relcast request.
  m_show_source = true;
  m_show_community = true;
  m_wrap_content = false;

  m_show_subscriptions = true;
  m_show_masked = true;
  m_trunc_content = false;
  
  vector<string> svector = parseString(sval, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    bool handled = true;
    if(param == "channel")
      m_channel = value;
    else if(param == "duration") {
      double dval = atof(value.c_str());
      if(dval <= 0)
	handled = false;
      else {
	// Want duration to be in real time, so mult by warp
	dval *= m_time_warp;
	m_expire_time = m_curr_time + dval;
	string request_key = tolower(comm) + ":" + tolower(src);
	m_map_key_expire[request_key]  = m_expire_time;
	m_map_key_channel[request_key] = m_channel;
      }
    }
    else if(param == "nosrc")
      m_show_source = false;
    else if(param == "nocom")
      m_show_community = false;
    else if(param == "nosubs")
      m_show_subscriptions = false;
    else if(param == "mask")
      m_show_masked = false;
    else if(param == "wrap")
      m_wrap_content = true;
    else if(param == "trunc")
      m_trunc_content = true;
    else
      handled = false;

    if(!handled)
      reportRunWarning("Unhandled Request:" + sval);
  }

  // If we changed channels, then don't wait to produce
  // the next realmcast!
  if(m_channel != prev_channel)
    m_last_post_relcast = 0;
  
  reportEvent("New Request. Now Channel:" + m_channel);
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
    if(m_map_data[key].size() > 1)
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
  
  // Part 2: Determine for each key_expire pair if the pair has
  // expired or not. If not, then generate a realmcast for the
  // channel associated with that key. If the key_expire pair
  // has expired, make note of the key, and cleanup afterwards
  vector<string> expired_keys;
       
  map<string, double>::iterator p;
  for(p=m_map_key_expire.begin(); p!=m_map_key_expire.end(); p++) {
    string key = p->first;
    double expire_time = p->second;

    if(m_curr_time <= expire_time) {
      buildRealmCast(m_map_key_channel[key]);
      m_last_post_relcast = m_curr_time;
    }
    else
      expired_keys.push_back(key);
  }

  // Part 3: For key_expire pairs that have indeed expired, remove
  // these entries from the maps.
  for(unsigned int i=0; i<expired_keys.size(); i++) {
    string key = expired_keys[i];
    m_map_key_expire.erase(key);
    m_map_key_channel.erase(key);
  }
}


//---------------------------------------------------------
// Procedure: buildRealmCast(channel)

void Realm::buildRealmCast(string channel)
{
  vector<string> output;
  stringstream ss;
  
  set<string> pubs = m_map_pubs[channel];

  if(m_show_subscriptions && m_map_subs.count(channel)) {
    set<string> subs = m_map_subs[channel];
    ss << "Subscriptions" << endl;
    ss << "=======================================" << endl;
    resetACTable();

    set<string>::iterator p;
    for(p=subs.begin(); p!=subs.end(); p++) {
      string var = *p;

      if(m_show_masked && (m_map_data.count(var) == 0))
	addRowACTab(var, "-", "never", "-", "-");
      
      if(m_map_data.count(var) != 0) {
	CMOOSMsg msg   = m_map_data[var].front();
	string   src   = msg.GetSource();
	string   comm  = msg.GetCommunity();
	double   dtime = msg.GetTime() - m_start_time;
	string   stime = doubleToString(dtime, 2);
	string   sval  = "formatted report";
	if(msg.IsDouble())
	  sval = doubleToStringX(msg.GetDouble(),6);
	else if(msg.IsBinary())
	  sval = "binary";
	else if(!isIgnoreVar(var)) 
	  sval  = msg.GetString();
	
	addRowACTab(var, src, stime, comm, sval);
      }
    }
    ss << m_actab.getFormattedString(true) << endl << endl;
  }

  if(m_map_pubs.count(channel)) {
    set<string> pubs = m_map_pubs[channel];
    // if not showing subscriptions, we don't need the upper bar
    if(m_show_subscriptions)
      ss << "=======================================" << endl;
    ss << "Publications" << endl;
    ss << "=======================================" << endl;

    resetACTable();
    
    set<string>::iterator q;
    for(q=pubs.begin(); q!=pubs.end(); q++) {
      string var = *q;
      
      if(m_map_data.count(var) != 0) {
	CMOOSMsg msg   = m_map_data[var].front();
	string   src   = msg.GetSource();
	string   comm  = msg.GetCommunity();
	double   dtime = msg.GetTime() - m_start_time;
	string   stime = doubleToString(dtime, 2);
	string   sval  = "formatted report";
	if(msg.IsDouble())
	  sval = doubleToStringX(msg.GetDouble(),6);
	else if(msg.IsBinary())
	  sval = "binary";
	else if(!isIgnoreVar(var))
	  sval  = msg.GetString();
	
	addRowACTab(var, src, stime, comm, sval);
      }
    }
    ss << m_actab.getFormattedString(true) << endl;
  }
  
  m_map_post_count[channel]++;
  unsigned int count = m_map_post_count[channel];
  
  RealmCast relcast;
  relcast.setNodeName(m_host_community);
  relcast.setProcName(channel);
  relcast.setCount(count);
  relcast.msg(ss.str());
  
  string relcast_str = relcast.getRealmCastString();
  
  Notify("REALMCAST", relcast_str);
}


//---------------------------------------------------------
// Procedure: buildRealmCastSummary()
//   Purpose: Every once in a while we update REALMCAST
//            recipients with the list of known channels

void Realm::buildRealmCastSummary()
{
  double elapsed = m_curr_time - m_last_post_summary;
  elapsed = elapsed / m_time_warp;

  // By default the summery interval is the value configured by the
  // user.
  double summary_interval = m_summary_interval;

  // But, for the first minute of operation, the summary interval will
  // be much more frequent.
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

  m_last_post_summary = m_curr_time;
  
  RealmSummary realm_summary;
  realm_summary.setNode(m_host_community);
  
  set<string>::iterator p;
  for(p=m_set_apps.begin(); p!=m_set_apps.end(); p++) {
    string app = *p;
    realm_summary.addProc(app);
  }

  m_summaries_posted++;
  
  Notify("REALMCAST_CHANNELS", realm_summary.get_spec());
}

//------------------------------------------------------------
// Procedure: resetACTable()

void Realm::resetACTable()
{
  int table_cols = 5;
  string header = "Variable | Source | Time | Comm | Value";
  if(!m_show_source) {
    table_cols--;
    header = findReplace(header,"Source |", "");
  }
  if(!m_show_community) {
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

void Realm::addRowACTab(string var, string source, string time,
			string comm, string value)
{
  if(!m_show_source)
    source  = "_ignore_";
  if(!m_show_community)
    comm = "_ignore_";

  if(m_trunc_content)
    value = value.substr(0, m_trunc_length);
  
  if(strEnds(var, "_STATUS") && strContains(value, "MOOSName")) {
    value = findReplace(value, "MOOSName", "$");
    value = biteString(value, '$');
  }
  
  if(!m_wrap_content)	
    m_actab << var << source << time << comm << value;
  else {
    vector<string> svector = breakLen(value, m_wrap_length);
    for(unsigned int i=0; i<svector.size(); i++)  {
      if(i == 0)
	m_actab << var << source << time << comm << svector[i];
      else {
	if(m_show_source)
	  source = "";
	if(m_show_community)
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
// Procedure: buildReport()

bool Realm::buildReport() 
{
  m_msgs << "Configuration: " << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "  RelCast Interval: " << doubleToString(m_relcast_interval,2) << endl;
  m_msgs << "  Summary Interval: " << doubleToString(m_summary_interval,2) << endl;
  m_msgs << "  Wrap Length:  " << uintToString(m_wrap_length) << endl;
  m_msgs << "  Trunc Length: " << uintToString(m_trunc_length) << endl;
  m_msgs << "  Max Msg Hist: " << uintToString(m_msg_max_hist) << endl;

  m_msgs << endl;
  m_msgs << "Content Config (from connected client): " << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "  Show Source: " << boolToString(m_show_source) << endl;
  m_msgs << "  Show Commun: " << boolToString(m_show_community) << endl;
  m_msgs << "  Show Subscr: " << boolToString(m_show_subscriptions) << endl;
  m_msgs << "  Show Masked: " << boolToString(m_show_masked) << endl;
  m_msgs << "  Wrp Content: " << boolToString(m_wrap_content) << endl;
  m_msgs << "  Trc Content: " << boolToString(m_trunc_content) << endl;
  m_msgs << "  Cur Channel: " << m_channel << endl;

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
  m_msgs << "  CurrTime:  " << doubleToString(m_curr_time, 2) << endl;
  m_msgs << "  Exp Time:  " << doubleToString(m_expire_time, 2) << endl;
  m_msgs << "  Summaries: " << uintToString(m_summaries_posted) << endl;
  
  m_msgs << endl;

  ACTable actab(2);
  actab << "Channel | Count";
  actab.addHeaderLines();
  
  map<string, unsigned int>::iterator q;
  for(q=m_map_post_count.begin(); q!=m_map_post_count.end(); q++) {
    string channel = q->first;
    string count = uintToString(q->second);
   
    actab << channel << count;
  }

  m_msgs << endl;
  m_msgs << "RealmCasts Generated: " << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << actab.getFormattedString();

  return(true);
}





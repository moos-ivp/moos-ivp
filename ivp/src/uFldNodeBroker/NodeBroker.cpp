/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeBroker.cpp                                       */
/*    DATE: Dec 19th 2011                                        */
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

#include <unistd.h>
#include <iterator>
#include "HostRecord.h"
#include "HostRecordUtils.h"
#include "UFieldUtils.h"
#include "NodeBroker.h"
#include "ACBlock.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

NodeBroker::NodeBroker()
{ 
  // Initialize State Variables
  m_pshare_cmd_posted = 0;
  m_pings_posted      = 0;
  m_ok_phis_received  = 0;
  m_bad_phis_received = 0;
  m_ok_acks_received  = 0;
  m_bad_acks_received = 0;
  m_host_info_changes = 0;

  m_messaging_policy = "";

  m_ping_sent_utc = 0;
  m_ok_ack_utc    = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool NodeBroker::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string comm  = msg.GetCommunity();

    bool msg_is_local = true;
    if(msg.GetCommunity() != m_node_host_record.getCommunity())
      msg_is_local = false;

    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();
    
    // Only accept HOST_INFO coming from our own community
    if((key == "PHI_HOST_INFO") && msg_is_local) 
      handleMailHostInfo(sval);

    else if(key == "DB_CLIENTS")
      checkMessagingPolicy(sval);
    
    else if(key == "TRY_SHORE_HOST") {
      bool handled = handleConfigTryShoreHost(sval, false);
      if(!handled)
	reportRunWarning("Invalid incoming TRY_SHORE_HOST:"+sval);
      else
	registerPingBridges(true);
    }

    // Only accept an ACK coming from a different community
    else if((key == "NODE_BROKER_ACK") && !msg_is_local) 
      handleMailAck(sval); 
    
    else 
      reportRunWarning("Unhandled Mail: " + key);    
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool NodeBroker::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool NodeBroker::Iterate()
{
  AppCastingMOOSApp::Iterate();

  bool resend_init_pshare_cmd = true;
  for(unsigned int i=0; i<m_shore_pings_ack.size(); i++) {
    if(m_shore_pings_ack[i] > 0)
      resend_init_pshare_cmd = false;
  }
  if(resend_init_pshare_cmd)
    registerPingBridges();
  
  sendNodeBrokerPing();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool NodeBroker::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  Notify("UFLDNODEBROKER_PID", getpid());

  // host_community info grabbed and stored at AppCastingMOOSApp level
  m_node_host_record.setCommunity(m_host_community);
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  bool auto_bridge_realmcast = true;
  bool auto_bridge_appcast   = true;
  bool auto_bridge_pshare_vars = true;
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "try_shore_host") 
      handled = handleConfigTryShoreHost(value);
    else if(param == "bridge") 
      handled = handleConfigBridge(value);
    else if(param == "auto_bridge_realmcast") 
      handled = setBooleanOnString(auto_bridge_realmcast, value);
    else if(param == "auto_bridge_appcast") 
      handled = setBooleanOnString(auto_bridge_appcast, value);
    else if(param == "auto_bridge_pshare_vars") 
      handled = setBooleanOnString(auto_bridge_pshare_vars, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  if(auto_bridge_realmcast) {
    handleConfigBridge("src=REALMCAST");
    handleConfigBridge("src=WATCHCAST");
    handleConfigBridge("src=REALMCAST_CHANNELS");
  }
  if(auto_bridge_appcast)
    handleConfigBridge("src=APPCAST");
  if(auto_bridge_pshare_vars)
    handleConfigBridge("src=NODE_PSHARE_VARS");

  registerVariables();
  registerPingBridges();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables()

void NodeBroker::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_BROKER_ACK", 0);
  Register("PHI_HOST_INFO", 0);
  Register("TRY_SHORE_HOST", 0);

  if(m_messaging_policy == "auto")
    Register("DB_CLIENTS", 0);
}


//------------------------------------------------------------
// Procedure: sendNodeBrokerPing()

void NodeBroker::sendNodeBrokerPing()
{
  // If we don't know enough about ourself, then don't send pings.
  if((m_node_host_record.getPShareIRoutes() == "") ||
     (m_node_host_record.getTimeWarp() == "")) 
    return;
    
  string iroutes = m_node_host_record.getPShareIRoutes();
  string ipaddr  = m_node_host_record.getHostIP();
  iroutes = findReplace(iroutes, "localhost", ipaddr);
  
  string tstamp = doubleToString(m_curr_time, 2);
  m_node_host_record.setTimeStamp(tstamp);
  string ping_msg = m_node_host_record.getSpec();
  
  for(unsigned int i=0; i<m_shore_routes.size(); i++) {

    // Dynamic threshold for sending out pings. If we have received
    // an ACK from the shoreside recently, we only re-send every 20
    // secs. If no recent ACKS, then send more frequently.
    double elapsed_ping_sent = m_curr_time - m_ping_sent_utc;
    double elapsed_ack_rcvd  = m_curr_time - m_ok_ack_utc;
    double thresh = 20;
    if(elapsed_ack_rcvd > 5)
      thresh = 5;

    if(elapsed_ping_sent > thresh) {
      string aug_ping_msg = ping_msg + ",key=" + uintToString(i);
      Notify("NODE_BROKER_PING_"+uintToString(i), aug_ping_msg);
      m_shore_pings_sent[i]++;
      m_pings_posted++;
      m_ping_sent_utc = m_curr_time;
    }
  }
}

//------------------------------------------------------------
// Procedure: checkMessagingPolicy()
//   Purpose: We should only be sharing MEDIATED_MESSAGE_LOCAL
//              to the shoreside when pMediator is in use.
//            And NODE_MESSAGE_LOCAL only when pMediator is
//              NOT in use.
//            Here we check for the presence of pMediator and
//              publish relevant warnings re: consistency
//            There is a 5 sec grace period to give time for
//              pMediator to show up on the db_clients list.

void NodeBroker::checkMessagingPolicy(string db_clients)
{
  // Give pMediator some time to show up on the db_clients list.
  // A 5 second grace period. The number is not terribly important
  // since the run warning will be quickly retracted if pMediator
  // shows up later.
  double elapsed = m_curr_time - m_start_time;
  if(elapsed < 5)
    return;

  if(strContains(db_clients, "pMediator")) {
    if(m_messaging_policy != "mediated") 
      reportRunWarning("Messaging Policy Mismatch");
    if(m_messaging_policy == "mediated") 
      retractRunWarning("Messaging Policy Mismatch");
    UnRegister("DB_CLIENTS");
  }
  
  else {
    if(m_messaging_policy == "mediated")
      reportRunWarning("Messaging Policy Mismatch");
  }
}


//------------------------------------------------------------
// Procedure: registerPingBridges()

void NodeBroker::registerPingBridges(bool only_latest)
{
  if(only_latest && (m_shore_routes.size() > 0)) {
    unsigned int index = m_shore_routes.size()-1;
    string src  = "NODE_BROKER_PING_"+uintToString(index);
    string dest = "NODE_BROKER_PING";
    string route = m_shore_routes[index];
    postPShareCommand(src, dest, route);
    return;
  }
    
  
  for(unsigned int i=0; i<m_shore_routes.size(); i++) {
    string src  = "NODE_BROKER_PING_"+uintToString(i);
    string dest = "NODE_BROKER_PING";
    string route = m_shore_routes[i];
    postPShareCommand(src, dest, route);
  }
}

//------------------------------------------------------------
// Procedure: registerUserBridges()

void NodeBroker::registerUserBridges()
{
  // Check each candidate shore node
  unsigned int k, ksize = m_shore_routes.size();
  for(k=0; k<ksize; k++) {
    // Check if the shore candidate (a) has been heard from with
    // a valid iroute, and (b) not yet had user variables bridged
    if(!m_shore_bridged[k] && (m_shore_pings_ack[k] > 0)) {
      m_shore_bridged[k] = true;

      unsigned int i, vsize = m_bridge_src_var.size();
      for(i=0; i<vsize; i++) {			  
	string src   = m_bridge_src_var[i];
	string dest  = m_bridge_alias[i];
	string route = m_shore_routes[k];
	postPShareCommand(src, dest, route);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: handleConfigBridge()
//   Example: bridge = src=FOO, alias=BAR

bool NodeBroker::handleConfigBridge(string line)
{
  // Part 1: Get the Source and Alias components
  string src, alias;
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = tolower(biteStringX(svector[i],'='));
    string right = svector[i];
    if(left == "src")
      src = right;
    else if(left == "alias")
      alias = right;
  }
  if((src == "") || strContainsWhite(src))
    return(false);
  if((alias == "") || strContainsWhite(alias))
    alias = src;

  // Part 2: If this bridge was already established, this request is
  // ignored with no error or warning.
  if(vectorContains(m_bridge_src_var, src) &&
     vectorContains(m_bridge_alias, alias))
    return(true);

  // Part 3: Take note of the implied messaging policy, so we can
  // post a run warning based on the presence of pMediator
  
  if((src == "NODE_MESSAGE_LOCAL") && (alias == "NODE_MESSAGE")) {
    if(m_messaging_policy == "mediated") {
      reportConfigWarning("Conflicted messaging policy");
      return(false);
    }
    m_messaging_policy = "unmediated";
  }
  
  if((src == "MEDIATED_MESSAGE_LOCAL") && (alias == "MEDIATED_MESSAGE")) {
    if(m_messaging_policy == "unmediated") {
      reportConfigWarning("Conflicted messaging policy");
      return(false);
    }
    m_messaging_policy = "mediated";
  }

  // Part 4: Add the components.
  m_bridge_src_var.push_back(src);
  m_bridge_alias.push_back(alias);
  return(true);
}


//------------------------------------------------------------
// Procedure: handleConfigTryShoreHost()

bool NodeBroker::handleConfigTryShoreHost(string original_line,
					  bool dup_warn)
{
  string pshare_route;

  vector<string> svector = parseString(original_line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    if(left == "pshare_route")
      pshare_route = right;
    else 
      return(false);
  }

  string ip_err_msg;
  if(!isValidPShareRoute(pshare_route, ip_err_msg)) {
    reportConfigWarning(ip_err_msg);
    return(false);
  }

  if(vectorContains(m_shore_routes, pshare_route)) {
    if(dup_warn) {
      reportConfigWarning("Duplicate try_shore_host:" + pshare_route);
      return(false);
    }
    else // just ignore 
      return(true);
  }
  
  m_shore_routes.push_back(pshare_route);
  m_shore_community.push_back("");
  m_shore_pings_sent.push_back(0);
  m_shore_pings_ack.push_back(0);
  m_shore_ipaddr.push_back("");
  m_shore_timewarp.push_back("");
  m_shore_bridged.push_back(false);

  // Keep track of actual (non localhost) IP Addresses found in routes
  if(strContains(pshare_route, ':') && !strBegins(pshare_route, "localhost:")) {
    string try_host_ip = biteString(pshare_route, ':');
    if(isValidIPAddress(try_host_ip))
      m_try_host_ips.push_back(try_host_ip);
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailAck()
//      Note: Parse NODE_BROKER_ACK and update the info about the
//            shoreside node that sent it.

void NodeBroker::handleMailAck(string ack_msg)
{
  // Part 1: Build/validate the incoming Host Record
  HostRecord hrecord = string2HostRecord(ack_msg);

  string new_key = hrecord.getKey();
  unsigned int key_ix = atoi(new_key.c_str());

  if((new_key == "") || !isNumber(new_key) || (key_ix >= m_shore_routes.size())) {
    m_bad_acks_received++;
    
    string msg = "NODE_BROKER_ACK recvd from " + hrecord.getHostIP();
    msg += " w/ null or bad key, for now.";

    reportRunWarning(msg);
    return;
  }

  m_shore_community[key_ix] = hrecord.getCommunity();
  m_shore_ipaddr[key_ix]   = hrecord.getHostIP();
  m_shore_timewarp[key_ix] = hrecord.getTimeWarp();
  m_shore_pings_ack[key_ix]++;

  string msg = stringVectorToString(m_bridge_alias);
  Notify("NODE_PSHARE_VARS", msg);
  
  m_ok_acks_received++;
  m_ok_ack_utc = m_curr_time;

  // Set up the user-configured variable bridges.
  registerUserBridges();
}


//------------------------------------------------------------
// Procedure: handleMailDBClients()

void NodeBroker::handleMailDBClients(string str)
{
  if(strContains(str, "pMediator")) {
  }
    

  double up_time = m_curr_time - m_start_time;
  if(up_time > 20) {
    m_messaging_policy = "unmediated";
    UnRegister("DB_CLIENTS");
    return;
  }
}

//------------------------------------------------------------
// Procedure: handleMailHostInfo()

void NodeBroker::handleMailHostInfo(string phi_msg)
{
  HostRecord hrecord = string2HostRecord(phi_msg);
  if(hrecord.valid() == false) {
    m_bad_phis_received++;
    reportRunWarning("Invalid PHI_HOST_INFO recd: " + phi_msg);
    return;
  }
  m_ok_phis_received++;
  
  // Set up the localhost bridges if the host info has changed.
  if((m_node_host_record.getCommunity()  != hrecord.getCommunity()) ||
     (m_node_host_record.getTimeWarp()   != hrecord.getTimeWarp())  ||
     (m_node_host_record.getHostIP()     != hrecord.getHostIP())    ||
     (m_node_host_record.getPortDB()     != hrecord.getPortDB())    ||
     (m_node_host_record.getPortUDP()    != hrecord.getPortUDP())   ||
     (m_node_host_record.getPShareIRoutes() != hrecord.getPShareIRoutes())) {
    m_host_info_changes++;
  }

  m_node_host_record = hrecord;
}

//------------------------------------------------------------
// Procedure: postPShareCommand()

void NodeBroker::postPShareCommand(string src, string dest, string route)
{
  if((src == "NODE_MESSAGE_LOCAL") && (m_messaging_policy != "unmediated"))
    return;
  
  if((src == "MEDIATED_MESSAGE_LOCAL") && (m_messaging_policy != "mediated"))
    return;
  
  string msg = "cmd=output";
  msg += ",src_name="  + src;
  msg += ",dest_name=" + dest;
  msg += ",route=" + route;
  Notify("PSHARE_CMD", msg);
  
  m_pshare_cmd_posted++;
  m_pshare_cmd_latest = msg;
}
  
//------------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//
bool NodeBroker::buildReport()
{
  m_msgs << endl;
  m_msgs << "  Total OK  PHI_HOST_INFO    received: " << m_ok_phis_received  << endl;
  m_msgs << "  Total BAD PHI_HOST_INFO    received: " << m_bad_phis_received << endl;
  m_msgs << "  Total HOST_INFO changes    received: " << m_host_info_changes << endl;
  m_msgs << "  Total          PSHARE_CMD    posted: " << m_pshare_cmd_posted << endl;
  m_msgs << "  Total BAD NODE_BROKER_ACK  received: " << m_bad_acks_received << endl;
  m_msgs << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << "             Vehicle Node Information:" << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << endl;
  m_msgs << "     Community: " << m_node_host_record.getCommunity()       << endl; 
  m_msgs << "        HostIP: " << m_node_host_record.getHostIP()          << endl; 
  m_msgs << "   Port MOOSDB: " << m_node_host_record.getPortDB()          << endl; 
  m_msgs << "     Time Warp: " << m_node_host_record.getTimeWarp()        << endl; 
  //  m_msgs << "  Comms Policy: " << commsPolicy()                           << endl; 

  string try_host_ips;
  for(unsigned int i=0; i<m_try_host_ips.size(); i++) {
    if(i!=0)
      try_host_ips += ",";
    try_host_ips += m_try_host_ips[i];
  }
  m_msgs << "   TryHost IPs: " << try_host_ips << endl;



  string iroutes = m_node_host_record.getPShareIRoutes();
  string ipaddr  = m_node_host_record.getHostIP();
  iroutes = findReplace(iroutes, "localhost", ipaddr);
  ACBlock block("      IRoutes: ", iroutes, 1, '&');
  m_msgs << block.getFormattedString();

  m_msgs << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << "            Shoreside Node(s) Information:" << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << endl;

  ACTable actab(6);
  actab << "Community |       | Pings | Pings | IP      | Time";
  actab << "Name      | Route | Sent  | Acked | Address | Warp";
  actab.addHeaderLines();
  unsigned int k, ksize = m_shore_routes.size();
  for(k=0; k<ksize; k++) {
    actab << m_shore_community[k];
    actab << m_shore_routes[k];
    actab << m_shore_pings_sent[k];
    actab << m_shore_pings_ack[k];
    actab << m_shore_ipaddr[k];
    actab << m_shore_timewarp[k];
  }
  m_msgs << actab.getFormattedString();

  m_msgs << endl << endl;

  m_msgs << "Phase Completion Summary:"             << endl;
  m_msgs << "------------------------------------" << endl;
  string s1 = (m_ok_phis_received > 0) ? "(Y)" : "(N)";
  m_msgs << " Phase 1: " << s1 << " Valid Host information retrieved (iroutes)." << endl;
  
  string s2 = (m_shore_routes.size() > 0) ? "(Y)" : "(N)";
  string num2 = uintToString(m_shore_routes.size());
  m_msgs << " Phase 2: " << s2 << " Valid TryHosts (" << num2 << ") configured." << endl;
  
  string s3 = (m_pings_posted > 0) ? "(Y)" : "(N)";
  m_msgs << " Phase 3: " << s3 << " NODE_BROKER_PINGS are being sent to TryHosts." << endl;

  string s4 = (m_ok_acks_received > 0) ? "(Y)" : "(N)";
  m_msgs << " Phase 4: " << s4 << " A Valid NODE_BROKER_ACK has been received." << endl;

  bool user_vars_bridged = false;
  for(unsigned int i=0; i<m_shore_bridged.size(); i++)
    user_vars_bridged = user_vars_bridged || m_shore_bridged[i];

  string s5 = user_vars_bridged ? "(Y)" : "(N)";
  m_msgs << " Phase 5: "<<s5<<" pShare requested to share user vars with shoreside." << endl;

  if(s1 == "(N)")
    m_msgs << "*Hint: Phase 1 problem. Check status/output of pHostInfo.              " << endl;
  else if(s2 == "(N)")
    m_msgs << "*Hint: Phase 2 problem. Check your .moos file configuration.           " << endl;
  else if(s3 == "(N)")
    m_msgs << "*Hint: Phase 3 problem. Check if our node's iroutes, timewarp are set. " << endl;
  else if(s4 == "(N)") {
    m_msgs << "*Hint: Phase 4 problem. Check that shoreside is running and            " << endl;
    m_msgs << "      that shoreside has received this node's NODE_BROKER_PING.        " << endl;
    m_msgs << "      If the shoreside HAS NOT received the ping, check that the       " << endl;
    m_msgs << "      pShare input route matches one of the TryHosts in uFldNodeBroker." << endl; 
    m_msgs << "      If the shoreside HAS received a ping, check that we have received" << endl;
    m_msgs << "      a NODE_BROKER_ACK in the local MOOSDB."                            << endl;
  }    
  else if(s5 == "(N)") {
    m_msgs << "*Hint: Phase 5 problem. Check that the user has indeed configured " << endl;
    m_msgs << "      in the .moos config block for this app."                      << endl;
  }    
  else
    m_msgs << "All Phases complete. Things should be working as configured. "      << endl;

  return(true);
}










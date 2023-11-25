/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ShoreBroker.cpp                                      */
/*    DATE: Dec 16th 2011                                        */
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
#include "ShoreBroker.h"
#include "ColorParse.h"
#include "HostRecord.h"
#include "HostRecordUtils.h"
#include "ACBlock.h"
#include "ACTable.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ShoreBroker::ShoreBroker()
{
  // Initialize config variables
  m_warning_on_stale = false;

  m_prev_node_count = 0;
  m_prev_node_count_tstamp = 0;

  // Initialize state variables
  m_iteration_last_ack = 0;

  m_pings_received    = 0;  // Times NODE_BROKER_PING    received
  m_phis_received     = 0;  // Times PHI_HOST_INFO       received
  m_acks_posted       = 0;  // Times NODE_BROKER_ACK     posted
  m_pshare_cmd_posted = 0;  // Times PSHARE_CMD_REGISTER posted

  m_last_pshare_vnodes = 0;
  m_last_posting_vnodes = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool ShoreBroker::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    //p->Trace();
    CMOOSMsg msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string comm  = msg.GetCommunity();

    bool msg_is_local = true;
    if(msg.GetCommunity() != m_shore_host_record.getCommunity())
      msg_is_local = false;

    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();

    if((key == "NODE_BROKER_PING") && !msg_is_local)
      handleMailNodePing(sval);
    else if((key == "PHI_HOST_INFO") && msg_is_local) {
      m_phis_received++;
      m_shore_host_record = string2HostRecord(sval);
    }
    else 
      reportRunWarning("Unhandled mail: " + key);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool ShoreBroker::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool ShoreBroker::Iterate()
{
  AppCastingMOOSApp::Iterate();

  makeBridgeRequestAll();
  sendAcks();
  checkForStaleNodes();
  postNodeCount();
  postTryVNodes();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ShoreBroker::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  // m_host_community is grabbed at the AppCastingMOOSApp level
  m_shore_host_record.setCommunity(m_host_community);
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  bool auto_bridge_realmcast = true;
  bool auto_bridge_appcast = true;
  bool auto_bridge_mhash = true;
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "bridge")
      handleConfigBridge(value);
    else if(param == "qbridge") 
      handleConfigQBridge(value);
    else if(param == "keyword") 
      m_keyword = value;
    else if(param == "auto_bridge_realmcast") 
      handled = setBooleanOnString(auto_bridge_realmcast, value);
    else if(param == "auto_bridge_appcast") 
      handled = setBooleanOnString(auto_bridge_appcast, value);
    else if(param == "auto_bridge_mhash") 
      handled = setBooleanOnString(auto_bridge_mhash, value);
    else if(param == "warning_on_stale") 
      handled = setBooleanOnString(m_warning_on_stale, value);
    else if(param == "try_vnode") 
      handled = handleConfigTryVNode(value);
    else
      handled = false;

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  m_time_warp_str = doubleToStringX(m_time_warp);

  if(auto_bridge_realmcast)
    handleConfigQBridge("REALMCAST_REQ");
  if(auto_bridge_appcast)
    handleConfigQBridge("APPCAST_REQ");
  if(auto_bridge_mhash)
    handleConfigBridge("src=MISSION_HASH");
  
  postQBridgeSet();
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables()

void ShoreBroker::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NODE_BROKER_PING", 0);
  Register("PHI_HOST_INFO", 0);
}

//------------------------------------------------------------
// Procedure: sendAcks()

void ShoreBroker::sendAcks()
{
  if(!m_shore_host_record.valid())
    return;

  unsigned int i, vsize = m_node_host_records.size();
  for(i=0; i<vsize; i++) {
    double elapsed_time = m_curr_time - m_node_last_tstamp[i];
    if(elapsed_time < 10) {
      string msg_var = "NODE_BROKER_ACK_" + 
	toupper(m_node_host_records[i].getCommunity());
      string msg_val = m_shore_host_record.getSpec() + 
	",status=" + m_node_host_records[i].getStatus();
      if(m_node_host_records[i].getKey() != "")
	msg_val += ",key=" + m_node_host_records[i].getKey();

      msg_val += ",id=" + uintToString(m_acks_posted);
      
      Notify(msg_var, msg_val);
      Notify("NODE_BROKER_VACK", m_node_host_records[i].getCommunity());
    }
  }
  m_acks_posted++;
}

//------------------------------------------------------------
// Procedure: postNodeCount()

void ShoreBroker::postNodeCount()
{
  bool post_now = false;

  unsigned int node_cnt_now = m_node_host_records.size();
  if(node_cnt_now != m_prev_node_count)
    post_now = true;
  
  if((m_curr_time - m_prev_node_count_tstamp) > 30)
    post_now = true;

  if(!post_now)
    return;

  Notify("UFSB_NODE_COUNT", node_cnt_now);
  m_prev_node_count = node_cnt_now;
  m_prev_node_count_tstamp = m_curr_time;

}

//------------------------------------------------------------
// Procedure: postTryVNodes()

void ShoreBroker::postTryVNodes()
{
  // Part 1: Every ~30 secs (re)post the PSHARE_CMD needed to
  // get the message to the nodes.
  double elapsed_pshare = m_curr_time - m_last_pshare_vnodes;
  if(elapsed_pshare > 30) {
    m_last_pshare_vnodes = m_curr_time;
    for(unsigned int i=0; i<m_try_vnodes.size(); i++) {
      string pshare_post = "cmd=output";
      pshare_post += ",src_name=TRY_SHORE_HOST";
      pshare_post += ",dest_name=TRY_SHORE_HOST";
      pshare_post += ",route=" + m_try_vnodes[i];
      Notify("PSHARE_CMD", pshare_post);
    }
  }
  
  // Part 2: Every ~10 secs Post the TRY_SHORE_HOST msgs out
  // to the vehicle nodes (vnodes).
  double elapsed_posting = m_curr_time - m_last_posting_vnodes;
  if(elapsed_posting > 10) {
    m_last_posting_vnodes = m_curr_time;
    for(unsigned int i=0; i<m_try_vnodes.size(); i++) {
      string ip = m_shore_host_record.getHostIP();
      string port = m_shore_host_record.getPortUDP();
      if((ip != "") && (port != "")) {
	string post = "pshare_route=" + ip + ":" + port;
	Notify("TRY_SHORE_HOST", post);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: postQBridgeSet()

void ShoreBroker::postQBridgeSet()
{
  string msg;
  set<string>::iterator p;
  for(p=m_set_qbridge_vars.begin(); p!=m_set_qbridge_vars.end(); p++) {
    string var = *p;
    if(msg != "")
      msg += ",";
    msg += var;
  }
  Notify("UFSB_QBRIDGE_VARS", msg);
}

//------------------------------------------------------------
// Procedure: postBridgeSet()

void ShoreBroker::postBridgeSet()
{
  string msg;
  set<string>::iterator p;
  for(p=m_set_bridge_vars.begin(); p!=m_set_bridge_vars.end(); p++) {
    string var = *p;
    if(msg != "")
      msg += ",";
    msg += var;
  }
  if(msg != "")
    Notify("UFSB_BRIDGE_VARS", msg);
}

//------------------------------------------------------------
// Procedure: checkForStaleNodes()

void ShoreBroker::checkForStaleNodes()
{
  unsigned int i, vsize = m_node_host_records.size();
  for(i=0; i<vsize; i++) {

    double elapsed_time = m_curr_time - m_node_last_tstamp[i];
    if(elapsed_time > 10) {
      if(!m_node_is_stale[i]) {
	string node_name = m_node_host_records[i].getCommunity();
	m_node_is_stale[i] = true;
	if(m_warning_on_stale)
	  reportRunWarning("Node " + node_name + " is stale.");
      }
    }
    else {
      if(m_node_is_stale[i]) {
	string node_name = m_node_host_records[i].getCommunity();
	m_node_is_stale[i] = false;
	if(m_warning_on_stale)
	  retractRunWarning("Node " + node_name + " is stale.");
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: handleMailNodePing()
//   Example: NODE_BROKER_PING = "community=abe,host_ip=192.168.7.6,
//                      port_db=9000,pshare_iroutes=192.168.7.6:9200
//                      time_warp=5,time=8351001297.78,key=1

void ShoreBroker::handleMailNodePing(const string& info)
{
  m_pings_received++;

  // Part 1: Build the incoming Host Record
  HostRecord hrecord = string2HostRecord(info);
  if(!hrecord.valid()) {
    reportRunWarning("Invalid Incoming NODE_BROKER_PING");
    reportEvent("Invalid Incoming NODE_BROKER_PING");
    return;
  }
  
  // Part 2: Determine the status (response) to the incoming ping.
  string status = "ok";
  if(m_time_warp_str != hrecord.getTimeWarp()) 
    status = "timewarp (" + m_time_warp_str + "!=" + hrecord.getTimeWarp() + ")";
  
  if((m_keyword != "") && (m_keyword != hrecord.getKeyword()))
    status = "keyword_mismatch";
  hrecord.setStatus(status);

  string ping_time = hrecord.getTimeStamp();
  double skew = m_curr_time - (atof(ping_time.c_str()));
  
  // Part 3: If the remote hostip != the local hostip, find/replace "localhost"
  // with the remote hostip if localhost appears in the iroute.

  string hostip_remote = hrecord.getHostIP();
  string hostip_shore  = m_shore_host_record.getHostIP();
  if(hostip_remote != hostip_shore) {
    string iroutes = hrecord.getPShareIRoutes();
    iroutes = findReplace(iroutes, "localhost", hostip_remote);
    hrecord.setPShareIRoutes(iroutes);
  } 

  // Part 4: Determine if this incoming node is a new node. 
  // If not, then just update its information and return.
  unsigned int j, jsize = m_node_host_records.size();
  for(j=0; j<jsize; j++) { 
    if(m_node_host_records[j].getCommunity() == hrecord.getCommunity()) {
      // Begin added mikerb mar0414
      if(m_node_host_records[j].getHostIP() != hrecord.getHostIP()) {
	m_node_bridges_made[j] = false;
	makeBridgeRequest("NODE_BROKER_ACK_$V", hrecord, "NODE_BROKER_ACK"); 
	m_node_host_records[j] = hrecord;
      }
      // End added mikerb mar0414      
      m_node_last_tstamp[j] = m_curr_time;
      m_node_total_skew[j] += skew;
      m_node_pings[j]++;
      m_node_host_records[j].setStatus(status);
      return;
    }
  }

  // Part 5: Handle the new Node.
  // Prepare to send this host and acknowldgement by posting a 
  // request to pShare for a new variable bridging.
  makeBridgeRequest("NODE_BROKER_ACK_$V", hrecord, "NODE_BROKER_ACK"); 

  reportEvent("New node discovered: " + hrecord.getCommunity());
  
  // The incoming host record now becomes the host record of record, so 
  // store its status.
  //hrecord.setStatus(status);
  
  // Store the new host info.
  m_node_host_records.push_back(hrecord);
  m_node_total_skew.push_back(skew);
  m_node_last_tstamp.push_back(m_curr_time);  
  m_node_bridges_made.push_back(false);
  m_node_is_stale.push_back(false);
  m_node_pings.push_back(1);
}


//------------------------------------------------------------
// Procedure: makeBridgeRequestAll()

void ShoreBroker::makeBridgeRequestAll()
{
  // For each known remote node
  unsigned int i, vsize = m_node_host_records.size();
  for(i=0; i<vsize; i++) {
    // If bridges have been made for this node, don't repeat
    if(!m_node_bridges_made[i]) {
      HostRecord hrecord = m_node_host_records[i];
      unsigned int k, ksize = m_bridge_src_var.size();
      // For each Bridge variable configured by the user, bridge.
      for(k=0; k<ksize; k++)
	makeBridgeRequest(m_bridge_src_var[k], hrecord, m_bridge_alias[k], i+1);
      m_node_bridges_made[i] = true;
    }
  }
}

//------------------------------------------------------------
// Procedure: makeBridgeRequest()
//  
// PSHARE_CMD="cmd=output,
//             src_name=FOO,
//             dest_name=BAR,
//             route=localhost:9000 & 192.168.1.5:9200

void ShoreBroker::makeBridgeRequest(string src_var, HostRecord hrecord, 
				    string alias, unsigned int node_index)
{
  if(!hrecord.valid()) {
    reportRunWarning("Failed to make bridge request. Invalid Host Record");
    return;
  }

  string community      = hrecord.getCommunity();
  string pshare_iroutes = hrecord.getPShareIRoutes();

  if(strContains(src_var, "$V"))
    src_var = findReplace(src_var, "$V", toupper(community));
  else if(strContains(src_var, "$v"))
    src_var = findReplace(src_var, "$v", community);
  else if(strContains(src_var, "$N")) {
    string nstr = uintToString(node_index);
    src_var = findReplace(src_var, "$N", nstr);
  }

  // Handle the pShare Dynamic registration
  string pshare_post = "cmd=output";
  pshare_post += ",src_name=" + src_var;
  pshare_post += ",dest_name=" + alias;
  pshare_post += ",route=" + pshare_iroutes;
  Notify("PSHARE_CMD", pshare_post);

  // If this is a new bridge, update the posted list of bridges
  if(!m_set_bridge_vars.count(src_var)) {
    m_set_bridge_vars.insert(src_var);
    postBridgeSet();
  }
  
  reportEvent("PSHARE_CMD:" + pshare_post);
  m_pshare_cmd_posted++;
}
  
  
//------------------------------------------------------------
// Procedure: handleConfigBridge()
//   Example: bridge = src=FOO
//   Example: bridge = src=FOO, alias=BAR
//   Example: bridge = src=DEPLOY_ALL, alias=DEPLOY
//   Example: bridge = src=DEPLOY_$V,  alias=DEPLOY

void ShoreBroker::handleConfigBridge(const string& line)
{
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
    else
      reportConfigWarning("BRIDGE config with unhandled param: " + left);
  }

  if(src == "") {
    reportConfigWarning("BRIDGE config empty src field");
    return;
  }

  if(alias == "")
    alias = src;

  m_set_bridge_vars.insert(src);
  
  handleConfigBridgeAux(src, alias);
}


//------------------------------------------------------------
// Procedure: handleConfigQBridge()
//      Note: line is expected to be simply a MOOS variable.
// 
//  QBRIDGE = FOOBAR
//                      is simply shorthand for:
//
//  BRIDGE  = src=FOOBAR_ALL, alias=FOOBAR
//  BRIDGE  = src=FOOBAR_$V,  alias=FOOBAR

void ShoreBroker::handleConfigQBridge(const string& line)
{
  vector<string> svector = parseString(line, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize;i++) {
    string src_var = stripBlankEnds(svector[i]);
    if(strContains(src_var, '=')) 
      reportConfigWarning("Invalid QBRIDGE component: " + src_var);
    else {
      m_set_qbridge_vars.insert(src_var);
      handleConfigBridgeAux(src_var+"_ALL", src_var);
      handleConfigBridgeAux(src_var+"_$V", src_var);
    }
  }
}

//------------------------------------------------------------
// Procedure: handleConfigBridgeAux()
//   Purpose: Add the bridge config but also check for duplicates

void ShoreBroker::handleConfigBridgeAux(string src_var, string alias)
{
  if(vectorContains(m_bridge_src_var, src_var) &&
     vectorContains(m_bridge_alias, alias))
    return;

  m_bridge_src_var.push_back(src_var);
  m_bridge_alias.push_back(alias);
}

//------------------------------------------------------------
// Procedure: handleConfigTryVNode()
//   Example: route=192.168.7.6:9200
//   Example: route=192.168.7.6      (9200 default)

bool ShoreBroker::handleConfigTryVNode(string vnode)
{
  string ip,port;
  
  vector<string> svector = parseString(vnode, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "route") {
      ip   = biteStringX(value, ':');
      port = value;
      if(port == "")
	port = "9200";
    }
    else
      return(false);
  }

  if(!isValidIPAddress(ip))
    return(false);
  if(!isNumber(port))
    return(false);

  string route = ip + ":" + port;

  if(vectorContains(m_try_vnodes, route)) {
    reportConfigWarning("Duplicate vnode route:" + route);
    return(false);
  }
  
  m_try_vnodes.push_back(route);
  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.

bool ShoreBroker::buildReport()
{
  m_msgs << endl;
  m_msgs << " Total PHI_HOST_INFO    received: " << m_phis_received       << endl;
  m_msgs << " Total NODE_BROKER_PING received: " << m_pings_received      << endl;
  m_msgs << " Total NODE_BROKER_ACK    posted: " << m_acks_posted         << endl;
  m_msgs << " Total PSHARE_CMD         posted: " << m_pshare_cmd_posted   << endl;
  m_msgs  << endl;

  m_msgs << "===========================================================" << endl;
  m_msgs << "            Shoreside Node(s) Information:"                  << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << endl;
  m_msgs << "     Community: " << m_shore_host_record.getCommunity()      << endl; 
  m_msgs << "        HostIP: " << m_shore_host_record.getHostIP()         << endl; 
  m_msgs << "   Port MOOSDB: " << m_shore_host_record.getPortDB()         << endl; 
  m_msgs << "     Time Warp: " << m_shore_host_record.getTimeWarp()       << endl; 
  ACBlock block("       IRoutes: ", m_shore_host_record.getPShareIRoutes(), 1, '&');
  m_msgs << block.getFormattedString();

  m_msgs << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << "             Vehicle Node Information:"                      << endl;
  m_msgs << "===========================================================" << endl;
  m_msgs << endl;


  ACTable actab(6,2); // 5 columns, 2 spaces separating columns
  actab  << "Node |   IP    |        | Elap | pShare         |      \n";
  actab  << "Name | Address | Status | Time | Input Route(s) | Skew \n";
  actab.addHeaderLines();

  unsigned int i, vsize = m_node_host_records.size();
  for(i=0; i<vsize; i++) {
    string node_name = m_node_host_records[i].getCommunity();
    string hostip    = m_node_host_records[i].getHostIP();
    string status    = m_node_host_records[i].getStatus();
    double elapsed   = m_curr_time - m_node_last_tstamp[i];
    string iroutes   = m_node_host_records[i].getPShareIRoutes();   
    string s_elapsed = doubleToString(elapsed, 1);

    double avg_skew = m_node_total_skew[i] / ((double)(m_node_pings[i]));
    string s_skew    = doubleToString(avg_skew,4);

    // Handle the case where there are zero input_routes (rare if ever)
    if(iroutes == "") 
      actab << node_name  << hostip << status  << s_elapsed << "" << s_skew;
    else {
      // Handle the case with one or more input routes for this node
      vector<string> svector = parseString(iroutes, '&');
      for(unsigned int j=0; j<svector.size(); j++) {
	// Handle the case with exactly one or first input route
	if(j==0)
	  actab << node_name  << hostip << status  << s_elapsed << svector[0] << s_skew;
	// Handle the case for reporting secondary input routes
	else
	  actab << " "  << " " << " " << " " <<  svector[j] << " ";
      }
    }
  }
  m_msgs << actab.getFormattedString();
  return(true);
}










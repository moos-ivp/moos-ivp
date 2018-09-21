/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
// Constructor

ShoreBroker::ShoreBroker()
{
  // Initialize config variables
  m_warning_on_stale = false;

  // Initialize state variables
  m_iteration_last_ack = 0;

  m_pings_received    = 0;  // Times NODE_BROKER_PING    received
  m_phis_received     = 0;  // Times PHI_HOST_INFO       received
  m_acks_posted       = 0;  // Times NODE_BROKER_ACK     posted
  m_pshare_cmd_posted = 0;  // Times PSHARE_CMD_REGISTER posted
}

//---------------------------------------------------------
// Procedure: OnNewMail

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
// Procedure: OnConnectToServer

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
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;
    
    if(param == "BRIDGE")
      handleConfigBridge(value);
    else if(param == "QBRIDGE") 
      handleConfigQBridge(value);
    else if(param == "KEYWORD") 
      m_keyword = value;
    else if(param == "WARNING_ON_STALE") {
      bool handled = setBooleanOnString(m_warning_on_stale, value);
      if(!handled)
	reportUnhandledConfigWarning(orig);
    }
    else 
      reportUnhandledConfigWarning(orig);
  }
  
  m_time_warp_str = doubleToStringX(m_time_warp);
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void ShoreBroker::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  m_Comms.Register("NODE_BROKER_PING", 0);
  m_Comms.Register("PHI_HOST_INFO", 0);
}

//------------------------------------------------------------
// Procedure: sendAcks

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
// Procedure: checkForStaleNodes

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
// Procedure: handleMailNodePing
//   Example: NODE_BROKER_PING = "COMMUNITY=alpha,IP=128.2.3.4,
//                       PORT=9000,PORT_UDP=9200,keyword=lemon
//                       pshare_iroutes=multicast_8#localhost:9000"

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
// Procedure: makeBridgeRequest
//  
// PSHARE_CMD="cmd=output,
//             src_name=FOO,
//             dest_name=BAR,
//             route=localhost:9000 & multicast_8

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

  reportEvent("PSHARE_CMD:" + pshare_post);
  m_pshare_cmd_posted++;
}
  
  
//------------------------------------------------------------
// Procedure: handleConfigBridge
//   Example: BRIDGE = src=FOO, alias=BAR
//   Example: BRIDGE = src=DEPLOY_ALL, alias=DEPLOY
//   Example: BRIDGE = src=DEPLOY_$V,  alias=DEPLOY

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
  
  m_bridge_src_var.push_back(src);
  m_bridge_alias.push_back(alias);
}


//------------------------------------------------------------
// Procedure: handleConfigQBridge
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
      m_bridge_src_var.push_back(src_var+"_ALL");
      m_bridge_alias.push_back(src_var);
      
      m_bridge_src_var.push_back(src_var+"_$V");
      m_bridge_alias.push_back(src_var);
    }
  }
}

//------------------------------------------------------------
// Procedure: buildReport
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









/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: NodeBroker.cpp                                       */
/*   DATE: Dec 19th 2011                                        */
/****************************************************************/

#include <iterator>
#include "HostRecord.h"
#include "HostRecordUtils.h"
#include "NodeBroker.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NodeBroker::NodeBroker()
{ 
  m_iteration         = 0;
  m_pmbs_posted       = 0;
  m_pings_posted      = 0;
  m_ok_phis_received  = 0;
  m_bad_phis_received = 0;
  m_ok_acks_received  = 0;
  m_bad_acks_received = 0;
  m_host_info_changes = 0;

  m_hack_ix           = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NodeBroker::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string comm  = msg.GetCommunity();

    bool msg_is_local = true;
    if(msg.GetCommunity() != m_node_host_record.getCommunity())
      msg_is_local = false;

#if 0
    double dval  = msg.GetDouble();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();
#endif
    
    // Only accept HOST_INFO coming from our own community
    if((key == "PHI_HOST_INFO") && msg_is_local) 
      handleMailHostInfo(sval);

    // Only accept an ACK coming from a different community
    else if((key == "NODE_BROKER_ACK") && !msg_is_local)
      handleMailAck(sval);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NodeBroker::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool NodeBroker::Iterate()
{
  m_iteration++;

  if(m_node_host_record.valid())
    postOutgoingPing();

  if(!m_shore_host_record.valid())
    registerPingBridges(true);


  printReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool NodeBroker::OnStartUp()
{
  // We grab our community info right away because we use this to
  // further filter incoming mail.
  string node_community;
  if(!m_MissionReader.GetValue("COMMUNITY", node_community)) {
    MOOSTrace("Community name not found in mission file!\n");
    return(false);
  }
  
  m_node_host_record.setCommunity(node_community);
  
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = toupper(biteStringX(line, '='));
      string value = line;

      if(param == "KEYWORD")
	m_keyword = value;
      // Example: TRY_SHORE_HOST = 192.168.1.2, 9000
      else if(param == "TRY_SHORE_HOST") 
	handleConfigTryShoreHost(value);

      // Example: PIGEON_VAR = LOITER_UP_1, LOITER_UP
      else if(param == "BRIDGE") {
	bool ok = handleConfigBridge(value);
	if(!ok)
	  MOOSTrace("Invalid BRIDGE init: %s\n", value.c_str());
      }
    }
  }	
  registerVariables();
  registerPingBridges();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void NodeBroker::registerVariables()
{
  m_Comms.Register("NODE_BROKER_ACK", 0);
  m_Comms.Register("PHI_HOST_INFO", 0);
}


//------------------------------------------------------------
// Procedure: registerPingBridges

void NodeBroker::registerPingBridges(bool random)
{
  unsigned int i, vsize = m_candidate_shore_host.size();

  m_hack_ix++;
  if(m_hack_ix >= vsize)
    m_hack_ix = 0;

  for(i=0; i<vsize; i++) {

    if(!random || (m_hack_ix==i)) {
      if(m_candidate_shore_host[i] != "localhost") {
	string msg = "SrcVarName=NODE_BROKER_PING_" + uintToString(i);
	msg += ",DestCommunity=" + m_candidate_shore_name[i];
	msg += ",DestCommunityHost=" + m_candidate_shore_host[i];
	msg += ",DestCommunityPort=" + m_candidate_shore_port[i];
	msg += ",DestVarName=NODE_BROKER_PING";
	m_Comms.Notify("PMB_REGISTER", msg);
	m_pmbs_posted++;
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: registerPingBridgesSubsLocal
//      Note: If localhost_ip is a valid IP address, and one of 
//            the candidate hosts is "localhost", the IP address
//            will be substituted in.


void NodeBroker::registerPingBridgesSubsLocal(string localhost_ip)
{
  if(!isValidIPAddress(localhost_ip))
    return;

  unsigned int i, vsize = m_candidate_shore_host.size();
  for(i=0; i<vsize; i++) {
    if(m_candidate_shore_host[i] == "localhost") {      
      string msg = "SrcVarName=NODE_BROKER_PING_" + uintToString(i);
      msg += ",DestCommunity=shoreside";
      msg += ",DestCommunityHost=" + localhost_ip;
      msg += ",DestCommunityPort=" + m_candidate_shore_port[i];
      msg += ",DestVarName=NODE_BROKER_PING";
      m_Comms.Notify("PMB_REGISTER", msg);
      m_pmbs_posted++;
    }
  }
}

//------------------------------------------------------------
// Procedure: registerUserBridges

void NodeBroker::registerUserBridges()
{
  unsigned int i, vsize = m_bridge_src_var.size();
  cout << "In registerUserBridges()" << vsize << endl;
  for(i=0; i<vsize; i++) {
    string msg = "SrcVarName=" + m_bridge_src_var[i];
    msg += ",DestCommunity=" + m_shore_host_record.getCommunity();
    msg += ",DestCommunityHost=" + m_shore_host_record.getHostIP();
    msg += ",DestCommunityPort=" + m_shore_host_record.getPortUDP();
    msg += ",DestVarName=" + m_bridge_alias[i];
    m_Comms.Notify("PMB_REGISTER", msg);
    m_pmbs_posted++;
  }
}


//------------------------------------------------------------
// Procedure: postOutgoingPing

void NodeBroker::postOutgoingPing()
{
  HostRecord hrecord = m_node_host_record;

  double curr_time = MOOSTime();
  string tstamp    = doubleToString(curr_time, 2);
  hrecord.setTimeStamp(tstamp);
  
  if(m_keyword != "")
    hrecord.setKeyword(m_keyword);

  string msg = hrecord.getSpec();
  
  if(!m_shore_host_record.valid()) {
    unsigned int i, vsize = m_candidate_shore_host.size();
    for(i=0; i<vsize; i++) {
      string moos_var = "NODE_BROKER_PING_" + uintToString(i);
      m_Comms.Notify(moos_var, msg);
      m_pings_posted++;
    }
  }
  else {
    m_Comms.Notify("NODE_BROKER_PING", msg);
    m_pings_posted++;
  }
}


//------------------------------------------------------------
// Procedure: handleConfigBridge
//   Example: BRIDGE = src=FOO, alias=BAR

bool NodeBroker::handleConfigBridge(string line)
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
  }

  if((src == "") || strContainsWhite(src))
    return(false);

  if((alias == "") || strContainsWhite(alias))
    alias = src;
  
  m_bridge_src_var.push_back(src);
  m_bridge_alias.push_back(alias);
  return(true);
}


//------------------------------------------------------------
// Procedure: handleConfigTryShoreHost

void NodeBroker::handleConfigTryShoreHost(string str)
{
  string hostip;
  string port_udp  = "9200";
  string community = "shoreside";

  string original_line = str;

  string unknown_params;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    if((left == "hostip") && isValidIPAddress(right))
      hostip = right;
    else if((left == "port_udp") && isNumber(right))
      port_udp = right;
    else if(left == "community")
      community = right;
    else {
      if(unknown_params != "")
	unknown_params += ",";
      unknown_params += (left + "=" + right);
    }
  }

  if(hostip == "") {
    MOOSTrace("Invalid TRY_SHORE_HOST: %s\n", original_line.c_str());
    m_invalid_tryhosts.push_back(original_line);
    return;
  }
  
  if(unknown_params != "")
    original_line += ("  Unknown Params:" + unknown_params);

  m_valid_tryhosts.push_back(original_line);
  m_candidate_shore_host.push_back(hostip);
  m_candidate_shore_port.push_back(port_udp);
  m_candidate_shore_name.push_back(community);
}

//------------------------------------------------------------
// Procedure: handleMailAck

void NodeBroker::handleMailAck(string ack_msg)
{
  HostRecord hrecord = string2HostRecord(ack_msg);

  if(hrecord.valid() == false) {
    m_bad_acks_received++;
    return;
  }
  
  // If this is the first ack received....
  if(m_shore_host_record.valid() == false) {
    // (1) Store the shore host record information 
    m_shore_host_record = hrecord;

    // (2) Set up a new bridge for pinging just the known host
    string msg = "SrcVarName=NODE_BROKER_PING";
    msg += ",DestCommunity=" + hrecord.getCommunity();
    msg += ",DestCommunityHost=" + hrecord.getHostIP();
    msg += ",DestCommunityPort=" + hrecord.getPortUDP();
    msg += ",DestVarName=NODE_BROKER_PING";
    m_Comms.Notify("PMB_REGISTER", msg);
    m_pmbs_posted++;

    // (3) Set up the user-configured variable bridges.
    registerUserBridges();
    
    m_ok_acks_received++;
  }
  
  else {
    if((hrecord.getCommunity() != m_shore_host_record.getCommunity()) ||
       (hrecord.getHostIP() != m_shore_host_record.getHostIP()) ||
       (hrecord.getPortUDP() != m_shore_host_record.getPortUDP())) {
      m_bad_acks_received++;
    }
    else
      m_ok_acks_received++;
  }
}


//------------------------------------------------------------
// Procedure: handleMailHostInfo

void NodeBroker::handleMailHostInfo(string phi_msg)
{
  HostRecord hrecord = string2HostRecord(phi_msg);
  if(hrecord.valid() == false) {
    m_bad_phis_received++;
    return;
  }
  m_ok_phis_received++;

  // Set up the localhost bridges if the host info has changed.
  bool change_detected = false;
  if((m_node_host_record.getCommunity() != hrecord.getCommunity()) ||
     (m_node_host_record.getHostIP()    != hrecord.getHostIP()) ||
     (m_node_host_record.getPortDB()    != hrecord.getPortDB()) ||
     (m_node_host_record.getPortUDP()   != hrecord.getPortUDP())) {
    change_detected = true;
    m_host_info_changes++;
  }

  if(change_detected && (hrecord.getPortUDP() != "")) {
    m_node_host_record = hrecord;
    string localhost_ip = m_node_host_record.getHostIP();
    registerPingBridgesSubsLocal(localhost_ip);
  }

}


//------------------------------------------------------------
// Procedure: printReport
//  

void NodeBroker::printReport()
{
  cout << endl << endl << endl << endl << endl;
  cout << "NodeBroker Status ----------- (" << m_iteration << ")" << endl;
  //  cout << "  MOOS Time Warp:                  " << m_timewarp << endl;
  cout << "  Total OK  PHI_HOST_INFO    received: " << m_ok_phis_received << endl;
  cout << "  Total BAD PHI_HOST_INFO    received: " << m_bad_phis_received << endl;
  cout << "  Total HOST_INFO changes    received: " << m_host_info_changes << endl;
  cout << "  Total PMB_REGISTER           posted: " << m_pmbs_posted << endl;
  cout << "  Total NODE_BROKER_PING       posted: " << m_pings_posted << endl;
  cout << "  Total OK  NODE_BROKER_ACK  received: " << m_ok_acks_received << endl;
  cout << "  Total BAD NODE_BROKER_ACK  received: " << m_bad_acks_received << endl;
  cout << "                                   " << endl;
  cout << "Configuration Info:            " << endl;
  cout << "  Total Valid TryHosts:   " << m_valid_tryhosts.size() << endl;
  unsigned int i, goodsize = m_valid_tryhosts.size();
  for(i=0; i<goodsize; i++)
    cout << "  [" << i+1 << "]: " << m_valid_tryhosts[i] << endl;
  cout << "  Total InValid TryHosts: " << m_invalid_tryhosts.size() << endl;
  unsigned int j, badsize = m_invalid_tryhosts.size();
  for(j=0; j<badsize; j++)
    cout << "  [" << j+1 << "]: " << m_invalid_tryhosts[j] << endl;
  cout << "                                   " << endl;
  cout << "Node Information: " << m_node_host_record.getCommunity() << endl; 
  cout << "      HostIP:  " << m_node_host_record.getHostIP() << endl; 
  cout << "   Port (DB):  " << m_node_host_record.getPortDB() << endl; 
  cout << "  Port (UDP):  " << m_node_host_record.getPortUDP() << endl; 
  cout << "   Time Warp:  " << m_node_host_record.getTimeWarp() << endl; 
  cout << "                                   " << endl;
  cout << "Shoreside Information: " << m_shore_host_record.getCommunity() << endl; 
  cout << "      HostIP:  " << m_shore_host_record.getHostIP() << endl; 
  cout << "   Port (DB):  " << m_shore_host_record.getPortDB() << endl; 
  cout << "  Port (UDP):  " << m_shore_host_record.getPortUDP() << endl; 
  cout << "   Time Warp:  " << m_shore_host_record.getTimeWarp() << endl; 



}



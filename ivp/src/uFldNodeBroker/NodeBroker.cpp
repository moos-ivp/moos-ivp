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
  m_user_bridges_posted = false;
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
      m_node_host_record = string2HostRecord(sval);

    // Only accept an ACK coming from a different community
    else if((key == "NODE_BROKER_ACK") && !msg_is_local)
      m_shore_host_record = string2HostRecord(sval);
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
  cout << "*" << flush;
    
  bool host_info_set  =  m_node_host_record.valid();
  bool shore_info_set =  m_shore_host_record.valid();

  cout << "host_info_set: " << host_info_set << endl;
  cout << "shore_info_set:" << shore_info_set << endl;

  //if(m_node_host_record.valid() && !m_shore_host_record.valid())
  //  postOutgoingPing();
  if(m_node_host_record.valid())
    postOutgoingPing();

  if(m_node_host_record.valid() && m_shore_host_record.valid()) {
    if(!m_user_bridges_posted) {
      registerUserBridges();
      m_user_bridges_posted = true;
    }
  }

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
      else if(param == "TRY_SHORE_HOST") {
	string host = biteStringX(value, ',');
	string port = value;
	if(port == "")
	  port = "9200";
	if(isValidIPAddress(host) && isNumber(port)) {
	  m_candidate_shore_host.push_back(host);
	  m_candidate_shore_port.push_back(port);
	}
	else
	  MOOSTrace("Invalid TRY_SHORE_HOST: %s\n", line.c_str());
      }

      // Example: PIGEON_VAR = LOITER_UP_1, LOITER_UP
      else if(param == "PIGEON_VAR") {
	string var   = biteStringX(value, ',');
	string alias = value;
	if(alias == "")
	  alias = var;
	
	if(!strContainsWhite(var) && !strContainsWhite(alias)) {
	  m_pigeon_vars.push_back(var);
	  m_pigeon_aliases.push_back(alias);
	}
	else
	  MOOSTrace("Invalid PIGEON_VAR init: %s\n", value.c_str());
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

void NodeBroker::registerPingBridges()
{
  unsigned int i, vsize = m_candidate_shore_host.size();
  for(i=0; i<vsize; i++) {
    string msg = "src_var=NODE_BROKER_PING,dest_community=shoreside";
    msg += ",dest_host=" + m_candidate_shore_host[i];
    msg += ",dest_port=" + m_candidate_shore_port[i];
    msg += ",dest_alias=NODE_BROKER_PING";
    m_Comms.Notify("PMB_REGISTER", msg);
  }
}

//------------------------------------------------------------
// Procedure: registerUserBridges

void NodeBroker::registerUserBridges()
{
  unsigned int i, vsize = m_pigeon_vars.size();
  cout << "In registerUserBridges()" << vsize << endl;
  for(i=0; i<vsize; i++) {
    string msg = "src_var=" + m_pigeon_vars[i];
    msg += ",dest_community=" + m_shore_host_record.getCommunity();
    msg += ",dest_host=" + m_shore_host_record.getHostIP();
    msg += ",dest_port=" + m_shore_host_record.getPortUDP();
    msg += ",dest_alias=" + m_pigeon_aliases[i];
    m_Comms.Notify("PMB_REGISTER", msg);
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
  
  m_Comms.Notify("NODE_BROKER_PING", msg);
}



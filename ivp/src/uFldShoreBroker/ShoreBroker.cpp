/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: ShoreBroker.cpp                                      */
/*   DATE: Dec 16th 2011                                        */
/****************************************************************/

#include <iterator>
#include "ShoreBroker.h"
#include "ColorParse.h"
#include "HostRecord.h"
#include "HostRecordUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ShoreBroker::ShoreBroker()
{
  // Initialize state variables
  m_iterations         = 0;
  m_iteration_last_ack = 0;
  m_curr_time          = 0;
  m_time_warp          = 1;
  m_last_report_time   = 0;

  m_pings_received = 0;  // Times NODE_BROKER_PING received
  m_phis_received  = 0;  // Times PHI_HOST_INFO    received
  m_acks_posted    = 0;  // Times NODE_BROKER_ACK  posted
  m_pmbs_posted    = 0;  // Times PMB_REGISTER     posted

  // Initialize config variables
  m_term_report_interval = 0.75;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ShoreBroker::OnNewMail(MOOSMSG_LIST &NewMail)
{
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

    // NODE_BROKER_PING = "community=archie,hostip=192.169.1.12,
    //                     port_db=9000,port_udp=9200,keyword=lemon"
    if((key == "NODE_BROKER_PING") && !msg_is_local) {
      m_pings_received++;
      bool ok = handleMailNodePing(sval);
      if(!ok)
	MOOSTrace("Unhandled Node Ping: %s\n", sval.c_str());
    }
    else if((key == "PHI_HOST_INFO") && msg_is_local) {
      m_phis_received++;
      m_shore_host_record = string2HostRecord(sval);
    }
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
  m_iterations++;
  m_curr_time = MOOSTime();

  makeBridgeRequestAll();
  sendAcks();

  // Consider generating terminal output
  double warp_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = warp_elapsed_time;
  if(m_time_warp > 0)
    real_elapsed_time = warp_elapsed_time / m_time_warp;
  if(real_elapsed_time > m_term_report_interval) {
    printReport();
    m_last_report_time = m_curr_time;
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ShoreBroker::OnStartUp()
{
  // We grab our community info right away because we use this to
  // further filter incoming mail.
  string node_community;
  if(!m_MissionReader.GetValue("COMMUNITY", node_community)) {
    MOOSTrace("Community name not found in mission file!\n");
    return(false);
  }
  m_shore_host_record.setCommunity(node_community);
  
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = toupper(biteStringX(line, '='));
      string value = line;
      
      // Example: BRIDGE = src=DEPLOY_ALL, alias=DEPLOY
      // Example: BRIDGE = src=DEPLOY_$V,  alias=DEPLOY
      if(param == "BRIDGE") {
	bool ok = handleConfigBridge(value);
	if(!ok)
	  MOOSTrace("Invalid BRIDGE config: %s\n", value.c_str());
      }
      else if(param == "QBRIDGE") {
	vector<string> svector = parseString(value, ',');
	unsigned int i, vsize = svector.size();
	for(i=0; i<vsize; i++) {
	  string varname = stripBlankEnds(svector[i]);
	  bool ok = handleConfigQBridge(varname);
	  if(!ok)
	    MOOSTrace("Invalid QBRIDGE config: %s\n", value.c_str());
	}
      }
      else if(param == "TERM_REPORT_INTERVAL") {
	return(handleConfigTermReportInterval(value));
      }
      else if(param == "KEYWORD") {
	m_keyword = value;
      }
    }
  }
  
  m_time_warp     = GetMOOSTimeWarp();
  m_time_warp_str = doubleToStringX(m_time_warp);
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void ShoreBroker::registerVariables()
{
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

      m_Comms.Notify(msg_var, msg_val);
      m_Comms.Notify("NODE_BROKER_VACK", m_node_host_records[i].getCommunity());
      m_acks_posted++;
    }
  }
}

//------------------------------------------------------------
// Procedure: handleMailNodePing
//   Example: NODE_BROKER_PING = "COMMUNITY=alpha,IP=128.2.3.4,
//                       PORT=9000,PORT_UDP=9200,keyword=lemon"

bool ShoreBroker::handleMailNodePing(string info)
{
  // Part 1: Build the incoming Host Record
  HostRecord hrecord = string2HostRecord(info);
  if(!hrecord.valid())
    return(false);
  
  // Part 2: Determine the status (response) to the incoming ping.
  string status = "ok";
  if(m_time_warp_str != hrecord.getTimeWarp()) 
    status = "timewarp (" + m_time_warp_str + "!=" + hrecord.getTimeWarp() + ")";
  
  if((m_keyword != "") && (m_keyword != hrecord.getKeyword()))
    status = "keyword_mismatch";

  string ping_time = hrecord.getTimeStamp();
  double skew = m_curr_time - (atof(ping_time.c_str()));
  
  // Part 3: Determine if this incoming node is a new node. 
  // If not, then jsut update its information and return.
  unsigned int j, jsize = m_node_host_records.size();
  for(j=0; j<jsize; j++) { 
    if(m_node_host_records[j].getCommunity() == hrecord.getCommunity()) {
      m_node_last_tstamp[j] = m_curr_time;
      m_node_total_skew[j] += skew;
      m_node_pings[j]++;
      m_node_host_records[j].setStatus(status);
      return(true);
    }
  }

  // Part 4: Handle the new Node.
  // Prepare to send this host and acknowldgement by posting a 
  // request to pMOOSBridge for a new variable bridging.
  makeBridgeRequest("NODE_BROKER_ACK_$V", hrecord, "NODE_BROKER_ACK"); 
  
  // The incoming host record now becomes the host record of record, so 
  // store its status.
  hrecord.setStatus(status);
  
  // Store the host info.
  m_node_host_records.push_back(hrecord);
  m_node_total_skew.push_back(skew);
  m_node_last_tstamp.push_back(m_curr_time);  
  m_node_bridges_made.push_back(false);
  m_node_pings.push_back(1);
  return(true);
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
// PMB_REGISTER="SrcVarName=FOO,
//               DestCommunity=henry,
//               DestCommunityHost=192.168.1.1,
//               DestCommunityPort=9201,
//               DestVarName=BAR"
//
// For Variable FOO, do two registrations:
//  [FOO_ALL]    -->  henry @ 192.168.1.1:9201 [FOO]
//  [FOO_HENRY]  -->  henry @ 192.168.1.1:9201 [FOO]

void ShoreBroker::makeBridgeRequest(string src_var, HostRecord hrecord, 
				    string alias, unsigned int node_index)
{
  if(!hrecord.valid())
    return;

  string community = hrecord.getCommunity();
  string hostip    = hrecord.getHostIP();
  string port_udp  = hrecord.getPortUDP();

  cout << "makeBridgeRequest:"       << endl;
  cout << "  src_var:" << src_var    << endl;
  cout << "community:" << community  << endl;
  cout << "   hostip:" << hostip     << endl;
  cout << "  portudp:" << port_udp   << endl;
  cout << "    alias:" << alias      << endl;

  string suffix = ",DestCommunity=" + community;
  suffix += ",DestCommunityHost=" + hostip;
  suffix += ",DestCommunityPort=" + port_udp;
  suffix += ",DestVarName=" + alias;

  if(strContains(src_var, "$V"))
    src_var = findReplace(src_var, "$V", toupper(community));
  else if(strContains(src_var, "$v"))
    src_var = findReplace(src_var, "$v", community);
  else if(strContains(src_var, "$N")) {
    string nstr = uintToString(node_index);
    src_var = findReplace(src_var, "$N", nstr);
  }

  string post = "SrcVarName=" + src_var + suffix;
  m_Comms.Notify("PMB_REGISTER", post);
  m_pmbs_posted++;
}
  
  
//------------------------------------------------------------
// Procedure: handleConfigBridge
//   Example: BRIDGE = src=FOO, alias=BAR

bool ShoreBroker::handleConfigBridge(string line)
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

  if(src == "")
    return(false);

  if(alias == "")
    alias = src;
  
  m_bridge_src_var.push_back(src);
  m_bridge_alias.push_back(alias);
  return(true);
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

bool ShoreBroker::handleConfigQBridge(string line)
{
  if(strContains(line, ','))
    return(false);
  if(strContains(line, '='))
    return(false);

  string src_var = line;

  m_bridge_src_var.push_back(src_var+"_ALL");
  m_bridge_alias.push_back(src_var);

  m_bridge_src_var.push_back(src_var+"_$V");
  m_bridge_alias.push_back(src_var);
  return(true);
}

//------------------------------------------------------------
// Procedure: setTermReportInterval

bool ShoreBroker::handleConfigTermReportInterval(string str)
{
  if(!isNumber(str))
    return(false);

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
  return(true);
}

//------------------------------------------------------------
// Procedure: printReport
//  

void ShoreBroker::printReport()
{
  string shore_info = termColor("magenta") + m_shore_host_record.getSpecTerse();

  cout << endl << endl << endl << endl << endl;
  cout << "ShoreBroker Status ----------- (" << m_iterations << ")" << endl;
  cout << " Shoreside Info:         " << shore_info << termColor() << endl;
  cout << " MOOS Time Warp:                  " << m_time_warp_str << endl;
  cout << " Total PHI_HOST_INFO    received: " << m_phis_received << endl;
  cout << " Total NODE_BROKER_PING received: " << m_pings_received << endl;
  cout << " Total NODE_BROKER_ACK    posted: " << m_acks_posted << endl;
  cout << " Total PMB_REGISTER       posted: " << m_pmbs_posted << endl;
  cout << "                                  " << endl;
  cout << "Node                  IPAddress        Status         Elapsed  Skew" << endl;
  cout << "--------------        ---------------  -------------  -------  ----" << endl;
  unsigned int i, vsize = m_node_host_records.size();
  for(i=0; i<vsize; i++) {
    string node_name = m_node_host_records[i].getCommunity();
    string hostip    = m_node_host_records[i].getHostIP();
    string status    = m_node_host_records[i].getStatus();
    double elapsed   = m_curr_time - m_node_last_tstamp[i];
    string s_elapsed = doubleToString(elapsed, 1);

    double avg_skew = m_node_total_skew[i] / ((double)(m_node_pings[i]));
    string s_skew    = doubleToString(avg_skew,4);


    node_name = padString(node_name, 22, false);
    hostip    = padString(hostip,    17, false);
    status    = padString(status,    15, false);
    s_elapsed = padString(s_elapsed,  8, false);
    s_skew    = padString(s_skew,     5, false);
    
    cout << node_name  << hostip << status  << s_elapsed << "  " << s_skew << endl;
  }
}



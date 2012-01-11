/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: NodeBroker.h                                         */
/*   DATE: Dec 19th 2011                                        */
/****************************************************************/

#ifndef UFLD_NODE_BROKER_HEADER
#define UFLD_NODE_BROKER_HEADER

#include "MOOSLib.h"
#include "HostRecord.h"

class NodeBroker : public CMOOSApp
{
 public:
  NodeBroker();
  virtual ~NodeBroker() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void handleConfigTryShoreHost(std::string);
  bool handleConfigBridge(std::string);

  void handleMailHostInfo(std::string);
  void handleMailAck(std::string);

  void registerVariables();
  void registerPingBridges(bool=false);
  void registerPingBridgesSubsLocal(std::string);
  void registerUserBridges();

  void postOutgoingPing();
  void printReport();


 protected: // Configuration Variables
  std::vector<std::string> m_bridge_src_var;
  std::vector<std::string> m_bridge_alias;

  std::string m_keyword;

  // Index on below vectors is a host to try as shoreside
  std::vector<std::string> m_candidate_shore_host;
  std::vector<std::string> m_candidate_shore_port;
  std::vector<std::string> m_candidate_shore_name;

 protected: // State Variables

  HostRecord  m_shore_host_record; // From NODE_BROKER_ACK
  HostRecord  m_node_host_record;  // From PHI_HOST_INFO

  unsigned int m_iteration;
  unsigned int m_pmbs_posted;
  unsigned int m_pings_posted;
  unsigned int m_ok_phis_received;
  unsigned int m_bad_phis_received;
  unsigned int m_ok_acks_received;
  unsigned int m_bad_acks_received;
  unsigned int m_host_info_changes;

  std::vector<std::string> m_valid_tryhosts;
  std::vector<std::string> m_invalid_tryhosts;

  unsigned int m_hack_ix;

};

#endif 


// NODE_BROKER_PING = "community=alpha,hostip=1.2.3.4,
//                     port_db=9000,port_udp=9200,key=lemon"

// NODE_BROKER_ACK = "community=shoreside,hostip=6.7.8.9,
//                    port_db=9000, port_udp=9200



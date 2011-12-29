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
  void registerVariables();

  void registerPingBridges();
  void registerUserBridges();
  void postOutgoingPing();

 protected: // Configuration Variables

  std::vector<std::string> m_pigeon_vars;
  std::vector<std::string> m_pigeon_aliases;

  std::string m_keyword;

  std::vector<std::string> m_candidate_shore_host;
  std::vector<std::string> m_candidate_shore_port;

 protected: // State Variables

  HostRecord  m_shore_host_record; // From NODE_BROKER_ACK
  HostRecord  m_node_host_record;  // From PHI_HOST_INFO

  bool        m_user_bridges_posted;

};

#endif 


// NODE_BROKER_PING = "community=alpha,hostip=1.2.3.4,
//                     port_db=9000,port_udp=9200,key=lemon"

// NODE_BROKER_ACK = "community=shoreside,hostip=6.7.8.9,
//                    port_db=9000, port_udp=9200

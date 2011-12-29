/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: ShoreBroker.h                                        */
/*   DATE: Dec 16th 2011                                        */
/****************************************************************/

#ifndef UFLD_SHORE_BROKER_HEADER
#define UFLD_SHORE_BROKER_HEADER

#include "MOOSLib.h"
#include "HostRecord.h"

class ShoreBroker : public CMOOSApp
{
 public:
  ShoreBroker();
  virtual ~ShoreBroker() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void registerVariables();
  bool handleNodePing(std::string);
  void  makeVariableBridges();
  bool handleBridgeConfig(std::string);
  bool handleQuickBridgeConfig(std::string);

  void bridgeRegister(std::string src, HostRecord, std::string alias,
		      unsigned int node_index=0);

  void sendAcks();

  void printReport();

 protected: // Config Variables
  std::vector<std::string> m_bridge_src_var;
  std::vector<std::string> m_bridge_alias;

  bool  m_all_localhost;
  
  std::string m_keyword;

 protected: // State Variables
  std::vector<HostRecord>   m_node_host_records;
  std::vector<double>       m_node_total_skew;
  std::vector<double>       m_node_last_tstamp;
  std::vector<bool>         m_node_bridges_made;
  std::vector<unsigned int> m_node_pings;

  HostRecord   m_shore_host_record;

  double       m_curr_time;
  std::string  m_timewarp;
  unsigned int m_iteration;
  unsigned int m_iteration_last_ack;

  unsigned int m_pings_received;
  unsigned int m_phis_received;
  unsigned int m_acks_posted;
  unsigned int m_pmbs_posted;
};

#endif 

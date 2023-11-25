/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ShoreBroker.h                                        */
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

#ifndef UFLD_SHORE_BROKER_HEADER
#define UFLD_SHORE_BROKER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "HostRecord.h"

class ShoreBroker : public AppCastingMOOSApp
{
 public:
  ShoreBroker();
  virtual ~ShoreBroker() {}

 public: // Standard MOOSApp/AppCastingMOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  void handleConfigBridge(const std::string&);
  void handleConfigQBridge(const std::string&);
  void handleConfigBridgeAux(std::string, std::string);
  bool handleConfigTryVNode(std::string);
  
  void handleMailNodePing(const std::string&);

  void makeBridgeRequestAll();
  void makeBridgeRequest(std::string src, HostRecord, std::string alias,
			 unsigned int node_index=0);
  
  void sendAcks();
  void checkForStaleNodes();
  void postQBridgeSet();
  void postBridgeSet();
  void postNodeCount();
  void postTryVNodes();
  void registerVariables();

 protected: // Config Variables
  std::vector<std::string> m_bridge_src_var;
  std::vector<std::string> m_bridge_alias;

  std::string m_keyword;

  bool m_warning_on_stale;

  std::set<std::string> m_set_qbridge_vars;
  std::set<std::string> m_set_bridge_vars;

 protected: // State Variables

  unsigned int m_prev_node_count;
  double       m_prev_node_count_tstamp;
  
  // Index of below vectors is unique on a connected node.
  std::vector<HostRecord>   m_node_host_records;
  std::vector<double>       m_node_total_skew;
  std::vector<double>       m_node_last_tstamp;
  std::vector<bool>         m_node_bridges_made;
  std::vector<bool>         m_node_is_stale;
  std::vector<unsigned int> m_node_pings;

  std::string  m_time_warp_str;
  HostRecord   m_shore_host_record;
  unsigned int m_iteration_last_ack;
  unsigned int m_pings_received;
  unsigned int m_phis_received;
  unsigned int m_acks_posted;
  unsigned int m_pshare_cmd_posted;

  std::vector<std::string> m_try_vnodes;
  double m_last_pshare_vnodes;
  double m_last_posting_vnodes;
};

#endif 


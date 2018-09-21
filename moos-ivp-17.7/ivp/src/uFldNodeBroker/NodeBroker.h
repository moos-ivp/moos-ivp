/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeBroker.h                                         */
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

#ifndef UFLD_NODE_BROKER_HEADER
#define UFLD_NODE_BROKER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "HostRecord.h"

class NodeBroker : public AppCastingMOOSApp
{
 public:
  NodeBroker();
  virtual ~NodeBroker() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  bool handleConfigTryShoreHost(std::string);
  bool handleConfigBridge(std::string);

  void sendNodeBrokerPing();

  void handleMailHostInfo(std::string);
  void handleMailAck(std::string);

  void registerVariables();
  void registerPingBridges();
  void registerUserBridges();

  void postPShareCommand(std::string src, std::string dest, std::string route);

 protected: // Configuration Variables
  std::vector<std::string> m_bridge_src_var;
  std::vector<std::string> m_bridge_alias;

  // Index on below vectors is a host to try as shoreside
  std::vector<std::string>  m_shore_community;
  std::vector<std::string>  m_shore_routes;
  std::vector<unsigned int> m_shore_pings_sent;
  std::vector<unsigned int> m_shore_pings_ack;
  std::vector<std::string>  m_shore_ipaddr;
  std::vector<std::string>  m_shore_timewarp;
  std::vector<bool>         m_shore_bridged;

  // A list of IP addresses on the pulled from the try_shore_host param
  std::vector<std::string>  m_try_host_ips;

 protected: // State Variables
  HostRecord   m_node_host_record;  // From PHI_HOST_INFO

  unsigned int m_pshare_cmd_posted;
  unsigned int m_pings_posted;
  unsigned int m_ok_phis_received;
  unsigned int m_bad_phis_received;
  unsigned int m_ok_acks_received;
  unsigned int m_bad_acks_received;
  unsigned int m_host_info_changes;

  std::string m_pshare_cmd_latest;
};

#endif 









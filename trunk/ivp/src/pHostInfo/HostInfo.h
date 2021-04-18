/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HostInfo.h                                           */
/*    DATE: Dec 11th 2011                                        */
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

#ifndef P_HOST_INFO_HEADER
#define P_HOST_INFO_HEADER

#include <string>
#include <list>
#include <vector>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class HostInfo : public AppCastingMOOSApp
{
 public:
  HostInfo();
  virtual ~HostInfo() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  void registerVariables();
  int  generateIPInfoFiles();
  void gatherIPInfoFromFiles();
  void postIPInfo();

 protected: 
  std::string readOSXInfoIP(std::string);
  std::string readLinuxInfoIP(std::string);
  int  clearTempFiles();
  bool handleMailPShareInput(std::string);
  bool handleConfigPreferInterface(std::string);
  
  void addIPInfo(std::string ip, std::string ip_source);

 protected: // Config variables
  std::string   m_tmp_file_dir;
  std::string   m_default_hostip;
  bool          m_default_hostip_force;
  std::string   m_prefer_interface;
  
 protected: // state variables

  std::string   m_ip_osx_wifi;
  std::string   m_ip_osx_wi_fi;
  std::string   m_ip_osx_airport;
  std::string   m_ip_osx_ethernet;
  std::string   m_ip_osx_usb_1000;  
  std::string   m_ip_osx_usb_ethernet;
  std::string   m_ip_osx_ethernet1;
  std::string   m_ip_osx_ethernet2;
  std::string   m_ip_osx_bridge100;

  std::string   m_ip_linux_wifi;
  std::string   m_ip_linux_ethernet0;
  std::string   m_ip_linux_ethernet1;
  std::string   m_ip_linux_usb0;
  std::string   m_ip_linux_usb1;
  std::string   m_ip_linux_usb2;
  std::string   m_ip_linux_hostname;
  std::string   m_ip_linux_any;

  unsigned int  m_pmb_udp_listen_cnt;

  bool          m_ip_info_files_generated;
  bool          m_ip_info_gathered;
  bool          m_ip_info_posted;

  std::list<std::string> m_event_messages;

 protected: // MOOSApp output
  std::string   m_host_ip;
  std::string   m_host_port_db;
  std::string   m_pshare_iroutes;
  std::string   m_host_ip_verbose;
  std::string   m_host_ip_all;
  std::string   m_host_record_all;
  
  std::vector<std::string> m_bad_configs;
};

#endif 









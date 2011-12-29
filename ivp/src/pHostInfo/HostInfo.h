/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HostInfo.h                                           */
/*   DATE: Dec 11th 2011                                        */
/****************************************************************/

#ifndef P_HOST_INFO_HEADER
#define P_HOST_INFO_HEADER

#include "MOOSLib.h"

class HostInfo : public CMOOSApp
{
 public:
  HostInfo();
  virtual ~HostInfo() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void registerVariables();
  void generateIPInfoFiles();
  void gatherIPInfoFromFiles();
  void postIPInfo();

 protected: 
  std::string readOSXInfoIP(std::string);
  std::string readLinuxInfoIP(std::string);
  void clearTempFiles();
  
  void addIPInfo(std::string ip, std::string ip_source);

 protected: // Config variables
  std::string   m_tmp_file_dir;

 protected: // state variables
  unsigned int  m_iterations;

  std::string   m_ip_osx_wifi;
  std::string   m_ip_osx_airport;
  std::string   m_ip_osx_ethernet;
  std::string   m_ip_osx_ethernet1;
  std::string   m_ip_osx_ethernet2;
  std::string   m_ip_linux_wifi;
  std::string   m_ip_linux_ethernet0;
  std::string   m_ip_linux_ethernet1;

  bool          m_ip_info_files_generated;
  bool          m_ip_info_gathered;
  bool          m_ip_info_posted;

 protected: // MOOSApp output
  std::string   m_host_community;
  std::string   m_host_ip;
  std::string   m_host_port_db;
  std::string   m_host_port_udp;
  std::string   m_timewarp;

  std::string   m_host_ip_verbose;
  std::string   m_host_ip_all;

};

#endif 

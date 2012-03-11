/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HostInfo.h                                           */
/*   DATE: Dec 11th 2011                                        */
/****************************************************************/

#ifndef P_HOST_INFO_HEADER
#define P_HOST_INFO_HEADER

#include <string>
#include <vector>
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
  void printReport();

 protected: 
  std::string readOSXInfoIP(std::string);
  std::string readLinuxInfoIP(std::string);
  void clearTempFiles();
  
  void addIPInfo(std::string ip, std::string ip_source);

 protected: // Config variables
  std::string   m_tmp_file_dir;
  std::string   m_default_hostip;
  bool          m_default_hostip_force;
  double        m_report_interval;

 protected: // state variables
  unsigned int  m_iterations;
  double        m_last_report_time;
  double        m_curr_time;

  std::string   m_ip_osx_wifi;
  std::string   m_ip_osx_airport;
  std::string   m_ip_osx_ethernet;
  std::string   m_ip_osx_ethernet1;
  std::string   m_ip_osx_ethernet2;
  std::string   m_ip_linux_wifi;
  std::string   m_ip_linux_ethernet0;
  std::string   m_ip_linux_ethernet1;
  std::string   m_ip_linux_usb0;
  std::string   m_ip_linux_usb1;
  std::string   m_ip_linux_usb2;
  std::string   m_ip_linux_any;

  bool          m_ip_info_files_generated;
  bool          m_ip_info_gathered;
  bool          m_ip_info_posted;

 protected: // MOOSApp output
  std::string   m_host_community;
  std::string   m_host_ip;
  std::string   m_host_port_db;
  std::string   m_host_port_udp;
  double        m_timewarp;
  std::string   m_timewarp_str;

  std::string   m_host_ip_verbose;
  std::string   m_host_ip_all;
  std::string   m_host_record_all;
  
  std::vector<std::string> m_bad_configs;
};

#endif 

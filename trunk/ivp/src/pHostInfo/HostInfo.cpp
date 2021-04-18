/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HostInfo.cpp                                         */
/*    DATE: Dec 14th 2011                                        */
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

#include <cstdlib>
#include <iterator>
#include "HostInfo.h"
#include "HostRecord.h"
#include "ColorParse.h"
#include "FileBuffer.h"
#include "MBUtils.h"
#include "ACBlock.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HostInfo::HostInfo() 
{
  // Initialize config variables
  m_tmp_file_dir   = "~/";
  m_default_hostip = "localhost";   // fall-back ip address
  m_default_hostip_force = false;   // Force use fall-back for debugging

  m_ip_info_files_generated = false;
  m_ip_info_gathered = false;
  m_ip_info_posted   = false;

  m_pmb_udp_listen_cnt = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HostInfo::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    string sval  = msg.GetString(); 
	
#if 0 // Keep these around just for template
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = true;
    if(key == "HOST_INFO_REQUEST") {
      m_ip_info_files_generated = false;
      m_ip_info_gathered = false;
      m_ip_info_posted = false;
    }
    else if(key == "PSHARE_INPUT_SUMMARY") 
      handled = handleMailPShareInput(sval);
    else 
      handled = false;

    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HostInfo::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool HostInfo::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Do all the auto-discovery work here - unless in the
  // rare/debugging case where the hostip address is hard-configured
  // (m_default_hostip_force is true).

  if(m_default_hostip_force == false) {
    if(!m_ip_info_files_generated)
      generateIPInfoFiles();
    
    if(!m_ip_info_gathered || (m_host_ip==""))
      gatherIPInfoFromFiles();
  }    

  if((!m_ip_info_posted) || ((m_iteration % 10) == 0))
    postIPInfo();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool HostInfo::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if(param == "TEMP_FILE_DIR") {
      if(!strContainsWhite(value)) {
	m_tmp_file_dir = value;
	handled = true;
      }
    }
    
    else if(param == "PREFER_INTERFACE")
      handled = handleConfigPreferInterface(value);
    else if((param == "DEFAULT_HOSTIP") ||
	    (param == "DEFAULT_HOST_IP")) {
      if(isValidIPAddress(value)) {
	m_default_hostip = value;
	handled = true;
      }
    }
    else if((param == "DEFAULT_HOSTIP_FORCE") || 
	    (param == "DEFAULT_HOST_IP_FORCE")) {
      if(isValidIPAddress(value)) {
	m_default_hostip = value;
	m_default_hostip_force = true;
	handled = true;
      }
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // Get the MOOSDB Port Information
  m_host_port_db = doubleToString(m_lServerPort,0);
  
  if(strContains(m_tmp_file_dir, "~")) {
    string home_dir = getenv("HOME");
    m_tmp_file_dir = findReplace(m_tmp_file_dir, "~", home_dir);
  }
  m_tmp_file_dir += "/.phostinfo/";

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void HostInfo::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("HOST_INFO_REQUEST", 0);
  m_Comms.Register("PSHARE_INPUT_SUMMARY", 0);
}


//---------------------------------------------------------
// Procedure: generateIPInfoFiles
//   Purpose: Invoke the various system commands that may or may not work
//            on the caller's platform. No harm in calling each one. 
//            Note the tilda '~' expands to the user's home directory.
//            We're making the assumption that the caller is a user on 
//            the system with a home directory with write permission

int HostInfo::generateIPInfoFiles()
{
  // First the various OS X system calls
  int result = 0;
  string sys_call;
  string name = m_host_community;

  string bgd = "";

  sys_call = "mkdir " + m_tmp_file_dir;
  // Bogusly pay attention to system return vals, avoid compiler warnings
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo Airport  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_airport_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo Wi-Fi  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_wi_fi_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo WiFi  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_wifi_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo Ethernet  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"USB Ethernet\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_usb_ethernet_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"USB 10/100/1000 LAN\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_usb_10_100_1000_lan_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"Ethernet 1\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet1_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"Ethernet 2\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet2_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  sys_call = "ifconfig bridge100 | grep 'broadcast 192.168.2.255' | cut -d ' ' -f2 > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_bridge100_" + name + ".txt" + bgd; 
  result = system(sys_call.c_str());

  //wlp60s0
  //sys_call  = "ifconfig eth0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  //sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  //sys_call += m_tmp_file_dir + "ipinfo_linux_ethernet0_" + name + ".txt" + bgd;
  //result = system(sys_call.c_str());
  
  // Next the various GNU/Linux system calls


  sys_call  = "ifconfig eth0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_ethernet0_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig eth1 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_ethernet1_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig wlan0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_wifi_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig usb0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb0_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig usb1 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb1_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig usb2 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb2_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "ifconfig | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_any_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  sys_call  = "hostname -I > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_hostname_" + name + ".txt" + bgd;
  result = system(sys_call.c_str());

  m_ip_info_files_generated = true;
  return(result);
}

//---------------------------------------------------------
// Procedure: gatherIPInfoFromFiles
//   Purpose: Try to gather IP information from each of the various
//            info files generated previously. 
//      Note: We don't expect that they will all return non-null IP
//            strings. Likely (hopefully) only one will be non-null.

void HostInfo::gatherIPInfoFromFiles()
{
  string name = m_host_community;
  m_ip_osx_wi_fi       = readOSXInfoIP("ipinfo_osx_wi_fi_" + name + ".txt");
  m_ip_osx_wifi        = readOSXInfoIP("ipinfo_osx_wifi_" + name + ".tx");
  m_ip_osx_airport     = readOSXInfoIP("ipinfo_osx_airport_" + name + ".txt");
  m_ip_osx_usb_1000    = readOSXInfoIP("ipinfo_osx_usb_10_100_1000_lan_"+name+".txt");
  m_ip_osx_ethernet    = readOSXInfoIP("ipinfo_osx_ethernet_" + name + ".txt");
  m_ip_osx_ethernet1   = readOSXInfoIP("ipinfo_osx_ethernet1_" + name + ".txt");
  m_ip_osx_ethernet2   = readOSXInfoIP("ipinfo_osx_ethernet2_" + name + ".txt");
  m_ip_osx_bridge100   = readLinuxInfoIP("ipinfo_osx_bridge100_" + name + ".txt");
  m_ip_osx_usb_ethernet = readOSXInfoIP("ipinfo_osx_usb_ethernet_" + name + ".txt");

  m_ip_linux_wifi      = readLinuxInfoIP("ipinfo_linux_wifi_" + name + ".txt");
  m_ip_linux_ethernet0 = readLinuxInfoIP("ipinfo_linux_ethernet0_" + name + ".txt");
  m_ip_linux_ethernet1 = readLinuxInfoIP("ipinfo_linux_ethernet1_" + name + ".txt");
  m_ip_linux_usb0      = readLinuxInfoIP("ipinfo_linux_usb0_" + name + ".txt");
  m_ip_linux_usb1      = readLinuxInfoIP("ipinfo_linux_usb1_" + name + ".txt");
  m_ip_linux_usb2      = readLinuxInfoIP("ipinfo_linux_usb2_" + name + ".txt");
  m_ip_linux_hostname  = readLinuxInfoIP("ipinfo_linux_hostname_" + name + ".txt");
  m_ip_linux_any       = readLinuxInfoIP("ipinfo_linux_any_" + name + ".txt");

  m_ip_info_gathered = true;
}

//---------------------------------------------------------
// Procedure: postIPInfo
//  Examples:
//  PHI_HOST_IP         = 123.1.1.0
//  PHI_HOST_IP_ALL     = 123.1.1.0,192.168.1.119
//  PHI_HOST_PORT_DB    = 9000
//  PHI_HOST_IP_VERBOSE = "OSX_WIFI=123.1.1.0,OSX_ETHERNET=192.168.1.119"
//  PHI_HOST_INFO       = community=alpha,hostip=123.1.1.0,port_db=9000,
//                        pshare_iroutes=localhost:9301,timewarp=12

void HostInfo::postIPInfo()
{
  // If m_host_ip is empty string give it a default value
  if(m_host_ip == "")             
    m_host_ip = m_default_hostip;

  m_host_ip_all = "";
  m_host_ip_verbose = "";

  if(m_default_hostip_force == true) 
    addIPInfo(m_default_hostip, "USER_HARD_CONFIGURED");
  else {
    addIPInfo(m_ip_linux_wifi, "LINUX_WLAN");
    addIPInfo(m_ip_osx_airport, "OSX_AIRPORT");
    addIPInfo(m_ip_linux_any, "LINUX_ANY");
    addIPInfo(m_ip_linux_ethernet0, "LINUX_ETH0");
    addIPInfo(m_ip_linux_ethernet1, "LINUX_ETH1");
    addIPInfo(m_ip_linux_usb0, "LINUX_USB0");
    addIPInfo(m_ip_linux_usb1, "LINUX_USB1");
    addIPInfo(m_ip_linux_usb2, "LINUX_USB2");
    addIPInfo(m_ip_linux_hostname, "LINUX_HOSTNAME");
    addIPInfo(m_ip_osx_ethernet, "OSX_ETHERNET");
    addIPInfo(m_ip_osx_usb_ethernet, "OSX_USB_ETHERNET");
    addIPInfo(m_ip_osx_usb_1000, "OSX_USB_10_100_1000_LAN");
    addIPInfo(m_ip_osx_ethernet1, "OSX_ETHERNET1");
    addIPInfo(m_ip_osx_ethernet2, "OSX_ETHERNET2");
    addIPInfo(m_ip_osx_bridge100, "OSX_BRIDGE100");
    addIPInfo(m_ip_osx_wifi, "OSX_WIFI");
    addIPInfo(m_ip_osx_wi_fi, "OSX_WI-FI");
  }    

  if((m_prefer_interface == "wlan0") && (m_ip_linux_wifi != ""))
    m_host_ip = m_ip_linux_wifi;
  else if((m_prefer_interface == "wifi") && (m_ip_linux_wifi != ""))
    m_host_ip = m_ip_linux_wifi;
  else if((m_prefer_interface == "eth0") && (m_ip_linux_ethernet0 != ""))
    m_host_ip = m_ip_linux_ethernet0;
  else if((m_prefer_interface == "eth1") && (m_ip_linux_ethernet1 != ""))
    m_host_ip = m_ip_linux_ethernet1;
  else if((m_prefer_interface == "usb0") && (m_ip_linux_usb0 != ""))
    m_host_ip = m_ip_linux_usb0;
  else if((m_prefer_interface == "usb1") && (m_ip_linux_usb1 != ""))
    m_host_ip = m_ip_linux_usb0;
  else if((m_prefer_interface == "usb2") && (m_ip_linux_usb2 != ""))
    m_host_ip = m_ip_linux_usb0;
  
  if(m_host_ip != "")
    Notify("PHI_HOST_IP", m_host_ip);
  if(m_host_ip_all != "")
    Notify("PHI_HOST_IP_ALL", m_host_ip_all);
  if(m_host_ip_verbose != "")
    Notify("PHI_HOST_IP_VERBOSE", m_host_ip_verbose);
  if(m_host_port_db != "")
    Notify("PHI_HOST_PORT_DB", m_host_port_db);

  HostRecord hrecord;
  hrecord.setCommunity(m_host_community);
  hrecord.setHostIP(m_host_ip);
  hrecord.setHostIPAlts(m_host_ip_all);
  hrecord.setPortDB(m_host_port_db);
  hrecord.setTimeWarp(doubleToStringX(m_time_warp,1));
  hrecord.setPShareIRoutes(m_pshare_iroutes);
  m_host_record_all = hrecord.getSpec();

  if(strContains(m_host_record_all, "localhost") && (m_host_ip != ""))
    m_host_record_all = findReplace(m_host_record_all, "localhost", m_host_ip);


  Notify("PHI_HOST_INFO", m_host_record_all);
  
  m_ip_info_posted = true;

  // If m_host_ip still is the default value, reset m_host_ip to indicate
  // that the host informatin has still yet to be determined.
  if(m_host_ip == m_default_hostip)
    m_host_ip = "";
}

//---------------------------------------------------------
// Procedure: addIPInfo
//   Purpose: simple utility function for handling a known IP address
//            preparing if for output in one or more of the MOOS vars.

void HostInfo::addIPInfo(string ip, string ip_source)
{
  if(ip=="")
    return;

  m_host_ip = ip;
  
  // Append to the front of the string
  if(m_host_ip_all == "")
    m_host_ip_all = ip;
  else
    m_host_ip_all = ip + "," + m_host_ip_all;
  
  // Append to the front of the string
  if(m_host_ip_verbose == "")
    m_host_ip_verbose = ip_source + "=" + ip;
  else
    m_host_ip_verbose = ip_source + "=" + ip + "," + m_host_ip_verbose;

}

//---------------------------------------------------------
// Procedure: readOSXInfoIP
//   Purpose: Discern the IP address from the given file previously
//            generated with a system call. 

string HostInfo::readOSXInfoIP(string filename)
{
  filename = m_tmp_file_dir + filename;

  string return_info;

  vector<string> svector = fileBuffer(filename);
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string line = stripBlankEnds(svector[i]);
    if(strBegins(line, "IP address:"))
      return_info = stripBlankEnds(line.substr(11));
  }
  return(return_info);
}


//---------------------------------------------------------
// Procedure: readLinuxInfoIP
//   Purpose: Discern the IP address from the given file previously
//            generated with a system call. 
//      Note: We expect this file will contain a single line only.
//            The contents of the line should just be the IP address.

string HostInfo::readLinuxInfoIP(string filename)
{
  filename = m_tmp_file_dir + filename;

  string return_info;

  vector<string> svector = fileBuffer(filename);
  unsigned int i, vsize = svector.size();

  for(i=0; i<vsize; i++) {
    string line = stripBlankEnds(svector[i]);
    // Added Apr 2419 to handle cases where the line may be several
    // IP addresses separated by white space. In this case, just
    // bite off and use the first one (better than nothing).
    if(strContainsWhite(line))
      line = biteString(line, ' ');
    if(isValidIPAddress(line))
      return_info = line;
  }
  return(return_info);
}

//---------------------------------------------------------
// Procedure: clearTempFiles
//   Purpose: Clear all the temporary files created for storing IP
//            information. No harm in making the below calls if the
//            files don't actually exist. Want to prevent using 
//            stale information.

int HostInfo::clearTempFiles()
{
  // Bogusly pay attention to system return vals, avoid compiler warnings
  int res = 0;
  res = system("rm -f ~/.ipinfo_osx_airport.txt");     // OS X Snow Leopard
  res = system("rm -f ~/.ipinfo_osx_wifi.txt");        // OS X Lion
  res = system("rm -f ~/.ipinfo_osx_ethernet.txt");    // OS X 
  res = system("rm -f ~/.ipinfo_osx_ethernet1.txt");   // OS X 
  res = system("rm -f ~/.ipinfo_osx_ethernet2.txt");   // OS X 
  res = system("rm -f ~/.ipinfo_linux_ethernet0.txt"); // Linux
  res = system("rm -f ~/.ipinfo_linux_ethernet1.txt"); // Linux
  res = system("rm -f ~/.ipinfo_linux_usb0.txt");      // Linux
  res = system("rm -f ~/.ipinfo_linux_usb1.txt");      // Linux
  res = system("rm -f ~/.ipinfo_linux_usb2.txt");      // Linux
  res = system("rm -f ~/.ipinfo_linux_hostname.txt");  // Linux
  res = system("rm -f ~/.ipinfo_linux_wifi.txt");      // Linux
  return(res);
}

//---------------------------------------------------------
// Procedure: handleMailPShareInput
//   Example: localhost:9001:udp , 221.1.1.18:2000:multicast_18
//
//      Note: Two things are done here:
//            (1) We reduce the long form (localhost:9000:udp) to the 
//            short form (localhost:9000)
//            (2) We error check where possible.

bool HostInfo::handleMailPShareInput(string list_of_routes)
{
  string new_pshare_iroutes;

  vector<string> svector = parseString(list_of_routes, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string route = svector[i];
    vector<string> jvector = parseString(route, ':');
    unsigned int jsize = jvector.size();
    string new_route;
    if((jsize == 2) || (jsize == 3)) {
      string hostname  = jvector[0];
      string port      = jvector[1];
      string protocol  = "udp";
      if(jsize == 3)
	protocol  = jvector[2];
      if(isValidIPAddress(hostname) && isNumber(port) && (protocol=="udp")) 
	new_route = hostname + ":" + port;
      else if(isValidIPAddress(hostname) && isNumber(port))
	new_route = protocol;
    }
    if(new_route != "") {
      if(new_pshare_iroutes != "")
	new_pshare_iroutes += "&";
      new_pshare_iroutes += new_route;
    }
    else
      reportRunWarning("Invalid pShare input route reported: " + route);
  }

  m_pshare_iroutes = new_pshare_iroutes;
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigPreferInterface

bool HostInfo::handleConfigPreferInterface(string interface)
{
  interface = tolower(interface);
  if((interface != "wlan0") &&  (interface != "wifi") &&
     (interface != "eth0")  &&  (interface != "eth1") &&
     (interface != "usb0")  &&  (interface != "usb1") &&
     (interface != "usb2")) {
    return(false);
  }
  
  m_prefer_interface = interface;
  return(true);
}



//---------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//   Example:
//
//  PHI_HOST_IP:         128.30.24.232
//  PHI_HOST_IP_ALL:     128.30.24.232,128.31.33.159
//  PHI_HOST_IP_VERBOSE: OSX_ETHERNET2=128.30.24.232,
//                       OSX_WIFI=128.31.33.159
//  PHI_HOST_PORT:       9000
//  PHI_HOST_INFO:       community=shoreside,hostip=128.30.24.232,
//                       port_db=9000,port_udp=9200,timewarp=2
//

bool HostInfo::buildReport()
{
  vector<string> svector;

  m_msgs << endl;
  m_msgs << "  PHI_HOST_IP:          " << m_host_ip;

  if((m_host_ip == "") && m_default_hostip_force)
    m_msgs << termColor("reversered") << "DEFAULT HOSTIP FORCED";
  else if(m_host_ip == "")
    m_msgs << termColor("reversegreen") << "DEFAULT HOSTIP USED";

  m_msgs << termColor() << endl;

  ACBlock block;

  block = ACBlock("  PHI_HOST_IP_ALL:      ", m_host_ip_all, 42);
  m_msgs << block.getFormattedString() << endl;

  block = ACBlock("  PHI_HOST_IP_VERBOSE:  ", m_host_ip_verbose, 42);
  m_msgs << block.getFormattedString() << endl;

  m_msgs << "  PHI_HOST_PORT:        " << m_host_port_db    << endl;
  m_msgs << "  PHI_PSHARES_IROUTES:  " << m_pshare_iroutes  << endl;

  m_msgs << "  HOSTNAME util(Linux): " << m_ip_linux_hostname << endl;
  
  block = ACBlock("  PHI_HOST_INFO:        ", m_host_record_all, 42);
  m_msgs << block.getFormattedString();

  return(true);
}


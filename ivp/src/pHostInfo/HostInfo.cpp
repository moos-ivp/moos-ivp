/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HostInfo.cpp                                         */
/*   DATE: Dec 14th 2011                                        */
/****************************************************************/

#include <cstdlib>
#include <iterator>
#include "HostInfo.h"
#include "HostRecord.h"
#include "ColorParse.h"
#include "FileBuffer.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HostInfo::HostInfo()
{
  // Initialize config variables
  m_tmp_file_dir   = "~/";
  m_default_hostip = "localhost";   // fall-back ip address
  m_default_hostip_force = false;   // Force use fall-back for debugging
  m_report_interval = 0.75;         // Terminal report interval seconds
  m_last_report_time = 0;           // Timestamp of last terminal report

  // Initialize state variables
  m_iterations = 0;
  m_timewarp   = 1;

  m_ip_info_files_generated = false;
  m_ip_info_gathered = false;
  m_ip_info_posted   = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HostInfo::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
	
#if 0 // Keep these around just for template
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "HOST_INFO_REQUEST") {
      m_ip_info_files_generated = false;
      m_ip_info_gathered = false;
      m_ip_info_posted = false;
    }
    // We want to know, if pMOOSBridge is running locally, what port
    // it is listening to for UDP traffic.
    else if(key == "PMB_UDP_LISTEN") {
      string prior_port_udp = m_host_port_udp;
      // Only pay attention to pMOOSBridge running in this community
      if(comm == m_host_community)
	m_host_port_udp = doubleToStringX(dval);
      if(prior_port_udp != m_host_port_udp)
	m_ip_info_posted = false;
    }
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
  m_iterations++;
  m_curr_time = MOOSTime();

  // Do all the auto-discovery work here - unless in the rare/debugging 
  // case where the hostip address is hard-configured (m_default_hostip_force
  // is true).
  if(m_default_hostip_force == false) {
    if(!m_ip_info_files_generated)
      generateIPInfoFiles();
    
    if(!m_ip_info_gathered || (m_host_ip==""))
      gatherIPInfoFromFiles();
  }    

  if((!m_ip_info_posted) || ((m_iterations % 10) == 0))
    postIPInfo();

  double moos_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = moos_elapsed_time / m_timewarp;
  if(real_elapsed_time >= m_report_interval)
    printReport();
  else
    cout << "*" << flush;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool HostInfo::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "TEMP_FILE_DIR") {
	if(!strContainsWhite(value))
	  m_tmp_file_dir = value;
	else
	  m_bad_configs.push_back(original_line);	  
      }
      
      else if((param == "DEFAULT_HOSTIP") ||
	      (param == "DEFAULT_HOST_IP")) {
	if(isValidIPAddress(value))
	  m_default_hostip = value;
	else
	  m_bad_configs.push_back(original_line);	  
      }

      else if((param == "DEFAULT_HOSTIP_FORCE") || 
	      (param == "DEFAULT_HOST_IP_FORCE")) {
	if(isValidIPAddress(value)) {
	  m_default_hostip = value;
	  m_default_hostip_force = true;
	}
	else
	  m_bad_configs.push_back(original_line);	  
      }

      else if((param != "APPTICK") && (param != "COMMSTICK"))
	m_bad_configs.push_back(original_line);
    }
  }

  // Get the MOOSDB Port Information
  m_host_port_db = m_sServerPort;
  
  // Get the MOOSDB Community Information
  if(!m_MissionReader.GetValue("COMMUNITY", m_host_community))
    MOOSTrace("Community name not found in mission file!\n");
  else
    MOOSTrace("Community: %s\n", m_host_community.c_str());
  
  m_timewarp     = GetMOOSTimeWarp();
  m_timewarp_str = doubleToStringX(m_timewarp);
  
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
  m_Comms.Register("HOST_INFO_REQUEST", 0);
  m_Comms.Register("PMB_UDP_LISTEN", 0);
}


//---------------------------------------------------------
// Procedure: generateIPInfoFiles
//   Purpose: Invoke the various system commands that may or may not work
//            on the caller's platform. No harm in calling each one. 
//            Note the tilda '~' expands to the user's home directory.
//            We're making the assumption that the caller is a user on 
//            the system with a home directory with write permission

void HostInfo::generateIPInfoFiles()
{
  // First the various OS X system calls
  string sys_call;
  string name = m_host_community;

  sys_call = "mkdir " + m_tmp_file_dir;
  system(sys_call.c_str());

  sys_call = "networksetup -getinfo Airport  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_airport_" + name + ".txt"; 
  system(sys_call.c_str());

  sys_call = "networksetup -getinfo Wi-Fi  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_wifi_" + name + ".txt"; 
  system(sys_call.c_str());

  sys_call = "networksetup -getinfo Ethernet  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet_" + name + ".txt"; 
  system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"Ethernet 1\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet1_" + name + ".txt"; 
  system(sys_call.c_str());

  sys_call = "networksetup -getinfo \"Ethernet 2\"  > ";
  sys_call += m_tmp_file_dir + "ipinfo_osx_ethernet2_" + name + ".txt"; 
  system(sys_call.c_str());

  // Next the various GNU/Linux system calls
  sys_call  = "ifconfig eth0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_ethernet0_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig eth1 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_ethernet1_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig wlan0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_wifi_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig usb0 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb0_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig usb1 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb1_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig usb2 | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_usb2_" + name + ".txt";
  system(sys_call.c_str());

  sys_call  = "ifconfig | grep 'inet addr:'| grep -v '127.0.0.1' ";
  sys_call += "| cut -d: -f2 | awk '{ print $1}' > ";
  sys_call += m_tmp_file_dir + "ipinfo_linux_any_" + name + ".txt";
  system(sys_call.c_str());

  m_ip_info_files_generated = true;
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
  m_ip_osx_wifi        = readOSXInfoIP("ipinfo_osx_wifi_" + name + ".txt");
  m_ip_osx_airport     = readOSXInfoIP("ipinfo_osx_airport_" + name + ".txt");
  m_ip_osx_ethernet    = readOSXInfoIP("ipinfo_osx_ethernet_" + name + ".txt");
  m_ip_osx_ethernet1   = readOSXInfoIP("ipinfo_osx_ethernet1_" + name + ".txt");
  m_ip_osx_ethernet2   = readOSXInfoIP("ipinfo_osx_ethernet2_" + name + ".txt");
  m_ip_linux_wifi      = readLinuxInfoIP("ipinfo_linux_wifi_" + name + ".txt");
  m_ip_linux_ethernet0 = readLinuxInfoIP("ipinfo_linux_ethernet0_" + name + ".txt");
  m_ip_linux_ethernet1 = readLinuxInfoIP("ipinfo_linux_ethernet1_" + name + ".txt");
  m_ip_linux_usb0      = readLinuxInfoIP("ipinfo_linux_usb0_" + name + ".txt");
  m_ip_linux_usb1      = readLinuxInfoIP("ipinfo_linux_usb1_" + name + ".txt");
  m_ip_linux_usb2      = readLinuxInfoIP("ipinfo_linux_usb2_" + name + ".txt");
  m_ip_linux_any       = readLinuxInfoIP("ipinfo_linux_any_" + name + ".txt");

  m_ip_info_gathered = true;
}

//---------------------------------------------------------
// Procedure: postIPInfo
//  Examples:
//  PHI_HOST_IP = "123.1.1.0"
//  PHI_HOST_IP_ALL = "123.1.1.0,192.168.1.119"
//  PHI_HOST_IP_VERBOSE = "OSX_WIFI=123.1.1.0,OSX_ETHERNET=192.168.1.119"
//  PHI_HOST_INFO = "COMMUNITY=alpha,IP=123.1.1.0,PORT=9000,PORT_UDP=9200"

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
    addIPInfo(m_ip_osx_wifi, "OSX_WIFI");
    addIPInfo(m_ip_osx_airport, "OSX_AIRPORT");
    addIPInfo(m_ip_linux_any, "LINUX_ANY");
    addIPInfo(m_ip_linux_ethernet0, "LINUX_ETH0");
    addIPInfo(m_ip_linux_ethernet1, "LINUX_ETH1");
    addIPInfo(m_ip_linux_usb0, "LINUX_USB0");
    addIPInfo(m_ip_linux_usb1, "LINUX_USB1");
    addIPInfo(m_ip_linux_usb2, "LINUX_USB2");
    addIPInfo(m_ip_osx_ethernet, "OSX_ETHERNET");
    addIPInfo(m_ip_osx_ethernet1, "OSX_ETHERNET1");
    addIPInfo(m_ip_osx_ethernet2, "OSX_ETHERNET2");
  }    

  if(m_host_ip != "")
    m_Comms.Notify("PHI_HOST_IP", m_host_ip);
  if(m_host_ip_all != "")
    m_Comms.Notify("PHI_HOST_IP_ALL", m_host_ip_all);
  if(m_host_ip_verbose != "")
    m_Comms.Notify("PHI_HOST_IP_VERBOSE", m_host_ip_verbose);
  if(m_host_port_db != "")
    m_Comms.Notify("PHI_HOST_PORT", m_host_port_db);
  if(m_host_port_udp != "")
    m_Comms.Notify("PHI_HOST_PORT_UDP", m_host_port_udp);

  HostRecord hrecord;
  hrecord.setCommunity(m_host_community);
  hrecord.setHostIP(m_host_ip);
  hrecord.setPortDB(m_host_port_db);
  hrecord.setPortUDP(m_host_port_udp);
  hrecord.setTimeWarp(m_timewarp_str);
  
  m_host_record_all = hrecord.getSpec();
  m_Comms.Notify("PHI_HOST_INFO", m_host_record_all);

  m_ip_info_posted = true;

  // If m_host_ip still is the default value, reset m_host_ip to indicate
  // that the host informatin has still yet to be determined.
  if(m_host_ip == m_default_hostip)
    m_host_ip = "";
}

//---------------------------------------------------------
// Procedure: printReport()

void HostInfo::printReport()
{
  // Part 1: Header
  cout << endl << endl << endl << endl << endl;
  cout << "======================================================" << endl;
  cout << "pHostInfo Report ";
  cout << termColor("reverseblue") + m_host_community + termColor();
  cout << "  (" << m_iterations << ")" << endl;

  // Part 2: Configuration Warnings
  unsigned int config_warnings = m_bad_configs.size();
  if(config_warnings != 0) {
    cout << termColor("red") << "MOOS File Configuration Errors (";
    cout << config_warnings << ")" << endl;
    for(unsigned i=0; i<config_warnings; i++) {
      cout << "  [" <<stripBlankEnds(m_bad_configs[i]) << "]" << endl;
    }
  }
  cout << termColor();


  // Part 3: Normal Status Output
  cout << "  PHI_HOST_IP:         " << m_host_ip;

  if((m_host_ip == "") && m_default_hostip_force)
    cout << termColor("reversered") << "DEFAULT HOSTIP FORCED";
  else if(m_host_ip == "")
    cout << termColor("reversegreen") << "DEFAULT HOSTIP USED";

  cout << termColor() << endl;

  cout << "  PHI_HOST_IP_ALL:     " << m_host_ip_all << endl;
  cout << "  PHI_HOST_IP_VERBOSE: " << m_host_ip_verbose << endl;
  cout << "  PHI_HOST_PORT:       " << m_host_port_db << endl;
  cout << "  PHI_HOST_PORT_UDP:   " << m_host_port_udp << endl;
  cout << "  PHI_HOST_INFO:       " << m_host_record_all << endl;

  m_last_report_time = m_curr_time;
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

void HostInfo::clearTempFiles()
{
  system("rm -f ~/.ipinfo_osx_airport.txt");     // OS X Snow Leopard
  system("rm -f ~/.ipinfo_osx_wifi.txt");        // OS X Lion
  system("rm -f ~/.ipinfo_osx_ethernet.txt");    // OS X 
  system("rm -f ~/.ipinfo_osx_ethernet1.txt");   // OS X 
  system("rm -f ~/.ipinfo_osx_ethernet2.txt");   // OS X 
  system("rm -f ~/.ipinfo_linux_ethernet0.txt"); // Linux
  system("rm -f ~/.ipinfo_linux_ethernet1.txt"); // Linux
  system("rm -f ~/.ipinfo_linux_usb0.txt");      // Linux
  system("rm -f ~/.ipinfo_linux_usb1.txt");      // Linux
  system("rm -f ~/.ipinfo_linux_usb2.txt");      // Linux
  system("rm -f ~/.ipinfo_linux_wifi.txt");      // Linux
}



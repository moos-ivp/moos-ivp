// t. schneider tes@mit.edu 10.1.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is iSerialNMEA.cpp 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#include "iSerialNMEA.h"

// number of last speeds to average (where a speed is the time between two GPS points)
#define SPEED_POINTS 5

using namespace std;

CiSerialNMEA::CiSerialNMEA()
{
  m_speed[SPEED_POINTS];
  m_last_x = 0;
  m_last_y = 0;
  m_last_time = 0;

}
CiSerialNMEA::~CiSerialNMEA()
{
}

// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CiSerialNMEA::OnNewMail(MOOSMSG_LIST &NewMail)
{
  return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CiSerialNMEA::OnConnectToServer()
{
  return true;
}

// Iterate: called AppTick times per second
bool CiSerialNMEA::Iterate()
{ 
  string data;
  m_port.GetTelegram(data, 0.5);

  MOOSTrace(data);
  MOOSTrace("\n");

  vector<string> p_data;
  p_data = parseString(data,',');
  
  string key = stripBlankEnds(p_data[0]);

  // let's see what we can get
  // by the end we want lat (to get x), long (to get y)
  // heading, speed

  // process different types of NMEA strings
  if (MOOSStrCmp(key, "$GPGGA"))
    processGPGGA(p_data);
  
  else if (MOOSStrCmp(key, "$GPHGT"))
    processGPHGT(p_data);

  // proprietary types
  else if (MOOSStrCmp(key, "$PASHR"))
    processPASHR(p_data);
  
  double moos_time = MOOSTime();
  
  // borrowed from TransponderAIS.cpp
  // If (a) this is the first chance to post, or (b) there is no
  // blackout interval being implemented, or (c) the time since last
  // post exceeds the blackout interval, then perform a posting.

  if((m_last_post_time == -1) || (m_blackout_interval <= 0) ||
     ((moos_time-m_last_post_time) > m_blackout_interval))
    {
    
      string timeinfo = dstringCompact(doubleToString(m_best_time));
      string summary = "NAME=" + m_vessel_name;
      summary += ",TYPE=" + m_vessel_type;
      summary += ",TIME=" + timeinfo;
      summary += ",X="   + dstringCompact(doubleToString(m_nav_x));
      summary += ",Y="   + dstringCompact(doubleToString(m_nav_y));
      summary += ",SPD=" + dstringCompact(doubleToString(m_nav_speed));
      summary += ",HDG=" + dstringCompact(doubleToString(m_nav_heading));
      summary += ",DEPTH=0";
      
      m_Comms.Notify("AIS_REPORT_LOCAL", summary);
      
      m_last_post_time = moos_time;
    }
  
  return true;
}

// OnStartUp: called when program is run
bool CiSerialNMEA::OnStartUp()
{       
  m_last_post_time = -1;

  // read in configuration
  m_vessel_name = "Unknown";
  m_MissionReader.GetConfigurationParam("name", m_vessel_name);
  
  m_vessel_type = "Unknown";
  m_MissionReader.GetConfigurationParam("type", m_vessel_type);
  
  m_baud = 4800;
  m_MissionReader.GetConfigurationParam("baud", m_baud);
  
  m_port_name = "/dev/ttyS0";
  m_MissionReader.GetConfigurationParam("port", m_port_name);

  m_blackout_interval = 1;
  m_MissionReader.GetConfigurationParam("wait", m_blackout_interval);



  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if (!m_MissionReader.GetValue("LatOrigin", latOrigin))
    return MOOSFail("LatOrigin not set in *.moos file.\n");
      
  if (!m_MissionReader.GetValue("LongOrigin", longOrigin))
    return MOOSFail("LongOrigin not set in *.moos file\n");
  
  // initialize m_Geodesy
  if (!m_Geodesy.Initialise(latOrigin, longOrigin))
    return MOOSFail("Geodesy init failed.\n");


  
  // create the CMOOSLinuxSerialPort m_port with config specified
  // baud and port
  if(!m_port.Create(m_port_name.c_str(), m_baud))
    return false;
  return true;
}


// parse GPGGA string
bool CiSerialNMEA::processGPGGA(vector<string> p_data)
{

  string lat_deg;
  string lat_min;

  lat_deg = p_data[2].substr(0, 2);
  lat_min = p_data[2].substr(2, p_data[2].size());

  m_lat = atof(lat_deg.c_str()) + (atof(lat_min.c_str()) / 60);

  if (MOOSStrCmp("S", p_data[3]))
    m_lat = -m_lat;

  string long_deg;
  string long_min;


  long_deg = p_data[4].substr(0, 3);
  long_min = p_data[4].substr(3, p_data[4].size());


  m_long = atof(long_deg.c_str())+(atof(long_min.c_str()) / 60);

  if (MOOSStrCmp("W", p_data[5]))
    m_long = -m_long;

  m_last_x = m_nav_x;
  m_last_y = m_nav_y;
  m_last_time = m_best_time;

  m_best_time = MOOSTime();
  // convert lat, long into x, y.
  if(!m_Geodesy.LatLong2LocalGrid(m_lat, m_long, m_nav_y, m_nav_x))
    return false; 

  cout << "\n" << m_i << "\n" << m_speed[0] << "\n";
  
  // estimate speed from a few last positions
  double dx = m_last_x - m_nav_x;
  double dy = m_last_y - m_nav_y;
  double dt = m_best_time - m_last_time;
  m_speed[m_i] = sqrt(dx*dx + dy*dy) / dt;

  m_i = m_i + 1;
  if (m_i >= SPEED_POINTS)
    m_i = 0;

  m_nav_speed = 0;
  int i;
  for(i = 0; i < SPEED_POINTS; i++)
    m_nav_speed += m_speed[i] / SPEED_POINTS; 
  
  return true;
}


// parse GPHGT string
bool CiSerialNMEA::processGPHGT(vector<string> p_data)
{

  return true;
}


// parse PASHR string
bool CiSerialNMEA::processPASHR(vector<string> p_data)
{
  if (MOOSStrCmp(p_data[1], "ATT"))
    {
      m_nav_heading = atof(p_data[3].c_str());         
    }
  if (MOOSStrCmp(p_data[1], "AT2"))
    {
      // nothing that ATT doesn't give
    }


  return true;
}

/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MetaMemCompass.cpp                                   */
/*    DATE: Oct 3rd 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include <math.h>
#include "MetaMemCompass.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

MetaMemCompass::MetaMemCompass()
{
  curr_gps_speed       = 0;
  curr_gps_heading     = 0;
  curr_compass_heading = 0;
  curr_thrust          = 0;
  curr_delta           = 0;
  curr_total_delta     = 0;
  curr_total_compares  = 0;
  speed_thresh         = 0.2;  // meters per second

  time_stamp_gps       = -1;
  time_stamp_compass   = -1;

  m_memory_time        = 3;
  m_age_stale          = 3;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool MetaMemCompass::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    if(key == "GPS_HEADING") {
      if(ddata <= -180) ddata += 360.0;
      if(ddata >   180) ddata -= 360.0;
      curr_gps_heading = ddata;
      time_stamp_gps = MOOSTime();
    }
    else if(key == "COMPASS_HEADING") {
      if(ddata <= -180) ddata += 360.0;
      if(ddata >   180) ddata -= 360.0;
      curr_compass_heading = ddata;
      time_stamp_compass = MOOSTime();
    }
    else if(key == "GPS_SPEED") {
      curr_gps_speed = ddata;
    }
    else if(key == "DESIRED_THRUST") {
      curr_thrust = ddata;
    }
    else {
      MOOSTrace("MetaMemCompass: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool MetaMemCompass::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("GPS_HEADING", 0);
  m_Comms.Register("GPS_SPEED", 0);
  m_Comms.Register("COMPASS_HEADING", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool MetaMemCompass::Iterate()
{
  
  double curr_heading;
  string source = determineSource();
  
  if(source == "gps")
    curr_heading = curr_gps_heading;
  else
    curr_heading = curr_compass_heading;
  
  
  double currtime = MOOSTime();
  addHeading(curr_heading, currtime);

  double heading_avg;
  bool ok = getHeadingAvg(heading_avg);
  if(!ok)
    heading_avg = curr_heading;
  
  
  m_Comms.Notify("METAMEM_HEADING", heading_avg);

  if(source == "compass") 
    m_Comms.Notify("METAMEM_SOURCE", 0);
  else
    m_Comms.Notify("METAMEM_SOURCE", 1);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: determineSource()

string MetaMemCompass::determineSource()
{
  double gps_age     = gpsAge();
  double compass_age = compassAge();

  // Handle special case where both sources are TOO OLD
  if((gps_age > m_age_stale) && (compass_age > m_age_stale)) {
    if(gps_age < compass_age)
      return("gps");
    else
      return("compass");
  }

  if(gps_age > m_age_stale)
    return("compass");
  if(compass_age > m_age_stale)
    return("gps");
  

  if(curr_gps_speed <= speed_thresh)
    return("compass");
  else
    return("gps");
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool MetaMemCompass::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("iMetaMemCompass starting....\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine    = *p;
      string sVarName = MOOSChomp(sLine, "=");
      
      if(MOOSStrCmp(sVarName, "SpeedThresh")) {
	sLine = stripBlankEnds(sLine);
	speed_thresh = atof(sLine.c_str());
      }
      if(MOOSStrCmp(sVarName, "Memory")) {
	sLine = stripBlankEnds(sLine);
	m_memory_time = atof(sLine.c_str());
      }
    }
  }
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: gpsAge
//      Note: 

double MetaMemCompass::gpsAge()
{
  if(time_stamp_gps == -1)
    return(time_stamp_gps);
  
  return(MOOSTime() - time_stamp_gps);
}

//-----------------------------------------------------------------
// Procedure: compassAge
//      Note: 

double MetaMemCompass::compassAge()
{
  if(time_stamp_compass == -1)
    return(time_stamp_compass);

  return(MOOSTime() - time_stamp_compass);
}


//-----------------------------------------------------------
// Procedure: addHeading(double)

void MetaMemCompass::addHeading(double heading, 
			     double currtime)
{
  m_heading_val.push_back(heading);
  m_heading_time.push_back(currtime);
  
  int counter = 0;

  // Remove all stale elements from memory
  list<double>::iterator p;
  for(p = m_heading_time.begin(); p!=m_heading_time.end(); p++) {
    double itime = *p;
    if((currtime - itime) > m_memory_time) {
      counter++;
    }
  }

  for(int i=0; i<counter; i++) {
    m_heading_val.pop_front();
    m_heading_time.pop_front();
  }
}


//-----------------------------------------------------------
// Procedure: getHeadingAvg

bool MetaMemCompass::getHeadingAvg(double& heading_avg)
{
  if(m_heading_val.size() == 0)
    return(false);

  double s,c,ssum,csum,avg;
  
  ssum = 0.0;
  csum = 0.0;
  
  list<double>::iterator p;
  for(p = m_heading_val.begin(); p!=m_heading_val.end(); p++) {
    double iheading = *p;
  
    s = sin(iheading*M_PI/180.0);
    c = cos(iheading*M_PI/180.0);

    ssum += s;
    csum += c;    
  }

  avg = atan2(ssum,csum)*180.0/M_PI;

  if (avg < 0.0)
    avg += 360.0;
  
  heading_avg = avg;
  return(true);
}


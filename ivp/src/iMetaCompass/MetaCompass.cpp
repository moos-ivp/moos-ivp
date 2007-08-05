/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MetaCompass.cpp                                      */
/*    DATE: June 14th 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include "MetaCompass.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

MetaCompass::MetaCompass()
{
  curr_gps_speed       = 0;
  curr_gps_heading     = 0;
  curr_compass_heading = 0;
  curr_thrust          = 0;
  curr_delta           = 0;
  curr_total_delta     = 0;
  curr_total_compares  = 0;
  speed_thresh         = 0.2;  // meters per second
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool MetaCompass::OnNewMail(MOOSMSG_LIST &NewMail)
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
    }
    else if(key == "COMPASS_HEADING") {
      if(ddata <= -180) ddata += 360.0;
      if(ddata >   180) ddata -= 360.0;
      curr_compass_heading = ddata;
    }
    else if(key == "GPS_SPEED") {
      curr_gps_speed = ddata;
    }
    else if(key == "DESIRED_THRUST") {
      curr_thrust = ddata;
    }
    else {
      MOOSTrace("MetaCompass: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool MetaCompass::OnConnectToServer()
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

bool MetaCompass::Iterate()
{
  if(curr_gps_speed <= speed_thresh) {
    m_Comms.Notify("META_HEADING", curr_compass_heading);
    m_Comms.Notify("META_SOURCE", 0);
  }
  else {
    m_Comms.Notify("META_HEADING", curr_gps_heading);
    m_Comms.Notify("META_SOURCE", 1);
    double delta = curr_gps_heading - curr_compass_heading; 
    if(delta < 0) 
      delta *= -1.0;
    curr_total_delta += delta;
    curr_total_compares++;
    double new_avg = (curr_total_delta / curr_total_compares);
    m_Comms.Notify("META_OFFSET", new_avg);
  }    

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool MetaCompass::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("iMetaCompass starting....\n");

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
    }
  }
  
  return true;
}






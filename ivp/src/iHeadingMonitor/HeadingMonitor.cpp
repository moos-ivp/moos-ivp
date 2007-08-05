/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HeadingMonitor.cpp                                   */
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
#include "HeadingMonitor.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

HeadingMonitor::HeadingMonitor()
{
  heading_fresh  = false;
  heading_delta  = 0;
  track_fresh    = true;
  curr_rudder    = 0;
  time_thresh    = 3.0;
  heading_source = "meta";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool HeadingMonitor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    heading_new   = 0;
    heading_fresh = false;

    if((key == "GPS_HEADING") && (heading_source == "gps")) {
      heading_new   = ddata;
      heading_fresh = true;
    }
    else if((key == "MARINESIM_HEADING") && (heading_source == "sim")) {
      heading_new   = ddata;
      heading_fresh = true;
    }
    else if((key == "META_HEADING") && (heading_source == "meta")) {
      heading_new   = ddata;
      heading_fresh = true;
    } 
    else if((key == "COMPASS_HEADING") && (heading_source == "compass")) {
      heading_new   = ddata;
      heading_fresh = true;
    }
    else if(key == "GPS_SPEED") {
      curr_gps_speed = ddata;
    }
    else if(key == "DESIRED_RUDDER") {
      if(curr_rudder != ddata) {	
	curr_rudder = ddata;
	time_reset  = MOOSTime();
	track_fresh = true;
	heading_delta = 0;
      }
    }
    else {
      MOOSTrace("iHeadingMonitor: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }

  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool HeadingMonitor::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("COMPASS_HEADING", 0);
  m_Comms.Register("GPS_HEADING", 0);
  m_Comms.Register("META_HEADING", 0);
  m_Comms.Register("MARINESIM_HEADING", 0);
  m_Comms.Register("GPS_SPEED", 0);
  m_Comms.Register("DESIRED_RUDDER", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool HeadingMonitor::Iterate()
{
  double curr_time = MOOSTime();

  // If we haven't stabalized yet, post an invalid number
  if((curr_time - time_reset) <= time_thresh) {
    m_Comms.Notify("HEADING_RATE", 999.0);
    cout << "." << flush;
    return(true);
  }	

  // If no updates to heading have come, post the same rate
  if(!heading_fresh) {
    m_Comms.Notify("HEADING_RATE", heading_rate);
    return(true);
  }

  // track_fresh is true for the first heading value in the track
  if(track_fresh) {
    time_first   = curr_time;
    track_fresh  = false;
    heading_curr = heading_new;
    cout << endl;
  }
  else {
    if(heading_new <= -180) heading_new += 360.0;
    if(heading_new >   180) heading_new -= 360.0;
    double delta = heading_curr - heading_new;
    if(delta < -180)
      delta = (delta + 360.0) * -1.0;
    if(delta > 180)
      delta = (delta - 360.0) * -1.0;
    heading_delta += delta;
    heading_curr = heading_new;
    heading_rate = heading_delta / (curr_time - time_first);
    m_Comms.Notify("HEADING_RATE", heading_rate);
    cout << "Heading_rate: " << heading_rate << endl;
  }

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool HeadingMonitor::OnStartUp()
{
  CMOOSInstrument::OnStartUp();

  MOOSTrace("iHeadingMonitor starting....\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine    = *p;
      string sVarName = MOOSChomp(sLine, "=");
      
      if(MOOSStrCmp(sVarName, "TimeThresh")) {
	sLine = stripBlankEnds(sLine);
	time_thresh = atof(sLine.c_str());
      }
      if(MOOSStrCmp(sVarName, "HeadingSource")) {
	heading_source = stripBlankEnds(sLine);
      }

    }
  }
  
  return true;
}






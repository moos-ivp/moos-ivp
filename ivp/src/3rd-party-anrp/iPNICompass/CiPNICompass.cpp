// $Header: /home/cvsroot/project-marine-shell/src/iPNICompass/CiPNICompass.cpp,v 1.1.1.1 2005/08/08 01:17:13 cvsadmin Exp $
// (c) 2004

// CiPNICompass.cpp: implementation of the CiPNICompass class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiPNICompass.h"

#include "CPNICompass.h"

CiPNICompass::CiPNICompass()
{
	// constructor
	prerot = 0;
	magdec = 0;
}

CiPNICompass::~CiPNICompass()
{
	// destructor
}

bool CiPNICompass::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for (p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    if (msg.m_sKey == "DESIRED_THRUST") {
      // ...
    } else if (!msg.IsSkewed(MOOSTime()) && 
	       msg.m_sKey == ("COMPASS_CALIB_ENABLE" + suffix)) {
      if (msg.m_sVal == "enable") {
	MOOSTrace("entering calibration mode\n");
	comp->StartCalibMode();
      } else if (msg.m_sVal == "disable") {
	MOOSTrace("leaving calibration mode\n");
	comp->StopCalibMode();
      }
    } else if(msg.m_sKey == "GPS_MAGNETIC_DECLINATION") {
      if(magdec != msg.m_dfVal) {
	MOOSTrace("Setting magnetic declination to %lf\n",
		  msg.m_dfVal);
	comp->SetMagDec(msg.m_dfVal);
	magdec = msg.m_dfVal;
      }
    }
  }
  
  NewMail.clear();
  
  return true;
}

bool CiPNICompass::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", <max frequency at which to get
  //                             updates, 0 = max>);
  // note, you cannot ask the server for anything in this function yet
  
  string tmp;
  
  if (m_MissionReader.GetConfigurationParam("Port", tmp)) {
    comp = new CPNICompass;
    comp->SetPort(tmp);
  } else {
    MOOSTrace("Need to set Port in the mission file\n");
    throw;
  }
  
  if (m_MissionReader.GetConfigurationParam("Suffix", suffix)) {
    ;
  } else {
    suffix = "";
  }
  
  m_Comms.Register("COMPASS_CALIB_ENABLE" + suffix, 0);
  
  m_MissionReader.GetConfigurationParam("Type", tmp);
  m_MissionReader.GetConfigurationParam("PreRotation", prerot);
  
  if (strcasecmp(tmp.c_str(), "V2Xe") == 0) {
    comp->SetType(COMPASS_TYPE_V2XE);
  } else if (strcasecmp(tmp.c_str(), "MicroMag") == 0) {
    comp->SetType(COMPASS_TYPE_MICROMAG);
  }
  
  double dspeed;
  m_MissionReader.GetConfigurationParam("Speed", dspeed);
  int speed = (int)dspeed;
  comp->SetSpeed(speed);
  
  comp->StartCommThread();
  
  m_Comms.Register("GPS_MAGNETIC_DECLINATION", 0);
  
  return true;
}

bool CiPNICompass::Iterate()
{
  // happens AppTick times per second
  
  double heading = prerot + comp->GetHeading() - 180.0; // XXX: check this!
  // jckerken 10/3/04
  double yaw;
  
  while (heading > 180)
    heading -= 360;
  
  while (heading < -180)
    heading += 360;
  
  //heading = -heading;
  yaw = -heading * PI / 180;
  
  m_Comms.Notify("COMPASS_YAW" + suffix, yaw);
  m_Comms.Notify("COMPASS_HEADING" + suffix, heading);
  
  if (comp->GetType() == COMPASS_TYPE_V2XE) {
    // also supports reporting temperature (?)
    
    m_Comms.Notify("COMPASS_TEMPERATURE" + suffix, comp->GetTemperature());
  }
  
  return true;
}

bool CiPNICompass::OnStartUp()
{
  // happens after connection is completely usable
  // ... not when it *should* happen. oh well...
  
  return true;
}


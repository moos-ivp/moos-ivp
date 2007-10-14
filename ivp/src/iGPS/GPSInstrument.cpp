// $Header: /home/cvsroot/project-marine-shell/src/iGPS/GPSInstrument.cpp,v 1.2 2005/09/07 20:08:49 mikerb Exp $
// copyright (2001-2003) Massachusetts Institute of Technology (pnewman et al.)

// GPSInstrument.cpp: implementation of the CGPSInstrument class.
//
//////////////////////////////////////////////////////////////////////

#include "../MOOSLIB/MOOSLib.h"
#include "../MOOSGenLib/MOOSGenLib.h"
#include "GPSInstrument.h"
#include "NMEAMessage.h"

#include <iostream>
#include <strstream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPSInstrument::CGPSInstrument()
{
  m_sType = "VANILLA";
}

CGPSInstrument::~CGPSInstrument()
{
}

/////////////////////////////////////////////
///this is where it all happens..
bool CGPSInstrument::Iterate()
{
  if(GetData()) {
    PublishData();
  }
  return(true);
}


bool CGPSInstrument::OnStartUp()
{
  CMOOSInstrument::OnStartUp();
  
  //set up Geodetic conversions
  double dfLatOrigin;
  double dfLongOrigin;
  
  m_MissionReader.GetConfigurationParam("TYPE", m_sType);
  
  string sVal;
  
  if (m_MissionReader.GetValue("LatOrigin", sVal)) {
    dfLatOrigin = atof(sVal.c_str());
  } else {
    MOOSTrace("LatOrigin not set - FAIL\n");
    return(false);
  }
  
  if(m_MissionReader.GetValue("LongOrigin", sVal)) {
    dfLongOrigin = atof(sVal.c_str());
  } else {
    MOOSTrace("LongOrigin not set - FAIL\n");
    return(false);
  }
  
  if(!m_Geodesy.Initialise(dfLatOrigin, dfLongOrigin)) {
    MOOSTrace("Geodesy Init failed - FAIL\n");
    return(false);
  }
  
  
  //here we make the variables that we are managing
  double dfGPSPeriod = 1.0;
  
  //GPS update @ 2Hz
  AddMOOSVariable("X", "SIM_X", "GPS_X", dfGPSPeriod);
  AddMOOSVariable("Y", "SIM_Y", "GPS_Y", dfGPSPeriod);
  AddMOOSVariable("N", "",      "GPS_N", dfGPSPeriod);
  AddMOOSVariable("E", "",      "GPS_E", dfGPSPeriod);
  AddMOOSVariable("Raw", "",    "GPS_RAW", dfGPSPeriod);
  AddMOOSVariable("Satellites", "", "GPS_SAT", dfGPSPeriod);
  AddMOOSVariable("Time", "", "GPS_TIME", dfGPSPeriod);
  
  if(IsSimulateMode()) {
    //not much to do...
    RegisterMOOSVariables();
  } else {
    //try to open
    if(!SetupPort()) {
      return(false);
    }
    //try 10 times to initialise sensor
    if(!InitialiseSensorN(10, "GPS")) {
      return(false);
    }
  }
  return(true);
}



//-------------------------------------------------------------
// Procedure: OnNewMail

bool CGPSInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
  return(UpdateMOOSVariables(NewMail));
}

//-------------------------------------------------------------
// Procedure: PublishData()

bool CGPSInstrument::PublishData()
{
  return(PublishFreshMOOSVariables());
}


//-------------------------------------------------------------
// Procedure: OnConnectToServer()

bool CGPSInstrument::OnConnectToServer()
{
  if(IsSimulateMode()) {
    //not much to do...
    RegisterMOOSVariables();
  } 

  return(true);
}

//-------------------------------------------------------------
// Procedure: InitialiseSensor
//     Notes: here we initialise sensor, give it start up values

bool CGPSInstrument::InitialiseSensor()
{
  if (MOOSStrCmp(m_sType, "ASHTECH")) {
    char * sInit = "$PASHS,NME,GGA,A,ON\r\n";
    MOOSTrace("Sending %s\n", sInit);
    m_Port.Write(sInit, strlen(sInit));
    
    MOOSPause(2000);
    string sReply;
    double dfTime;
    
    if (m_Port.GetLatest(sReply, dfTime)) {
      MOOSTrace("Rx %s", sReply.c_str());
    } else {
      MOOSTrace("No reply\n");
    }
    
  } else if (MOOSStrCmp(m_sType, "GARMIN")) {

    char *sInitA = "$PGRMO,,2\r\n";
    MOOSTrace("Sending %s\n", sInitA);
    MOOSPause(2000);
    m_Port.Write(sInitA, strlen(sInitA));

    char *sInitB = "$PGRMO,GPGGA,1\r\n";
    MOOSTrace("Sending %s\n", sInitB);
    MOOSPause(2000);
    m_Port.Write(sInitB, strlen(sInitB));

    char *sInitC = "$PGRMO,GPRMC,1\r\n";
    MOOSTrace("Sending %s\n", sInitC);
    MOOSPause(2000);
    m_Port.Write(sInitC, strlen(sInitC));

    
    string sReply;
    double dfTime;
    
    if (m_Port.GetLatest(sReply, dfTime)) {
      MOOSTrace("Rx %s\n", sReply.c_str());
    } else {
      MOOSTrace("No reply\n");
    }
    
    char *sInit = "$PGRMC,,,,,,,,,,,,2,1,\r\n";
    MOOSTrace("Sending %s\n", sInit);
    m_Port.Write(sInit, strlen(sInit));
    
    
    if(m_Port.GetLatest(sReply, dfTime)) {
      MOOSTrace("Rx %s\n", sReply.c_str());
    } else {
      MOOSTrace("No reply\n");
    }
  }
  return true;
}

//----------------------------------------------------------------
// Procedure: GetData()

bool CGPSInstrument::GetData()
{
  if(!IsSimulateMode()) {
    //here we actually access serial ports etc
    
    string sWhat;
    double dfWhen;

    if (m_Port.IsStreaming()) {
      if(!m_Port.GetLatest(sWhat, dfWhen)) {
	return(false);
      }
    } else {
      if(!m_Port.GetTelegram(sWhat, 0.5)) {
	return(false);
      }
    }
    
    //MOOSTrace("Rx:  %s",sWhat.c_str());
    if(PublishRaw()) {
      SetMOOSVar("Raw", sWhat, MOOSTime());
    }
    
    ParseNMEAString(sWhat, dfWhen);
    
  } else {
    //in simulated mode there is nothing to do..all data
    //arrives via comms.
  }
  return(true);
}


//----------------------------------------------------------------
// Procedure: ParseNMEAString

bool CGPSInstrument::ParseNMEAString(string &sNMEAString, double dfWhen)
{
  //keep a copy for later..
  string sCopy = sNMEAString;
  string sWhat = MOOSChomp(sNMEAString, ",");
  bool   bGood = true;
  
  // GGA and GLL headers format the NMEA string differently
  
  if (sWhat == "$GPGGA") {
    // First of all is this a good NMEA string?
    if(!DoNMEACheckSum(sCopy)) {
      MOOSDebugWrite("GPS Failed NMEA check sum");
      return(false);
    }
    
    //OK so extract data...
    string sTmp;
    /////////////////////////////////////
    //          time
    sTmp = MOOSChomp(sNMEAString, ",");
    double dfTime  = atof(sTmp.c_str());
    // MOOSTrace("=> [%s] => %.2f\n\n",sTmp.c_str(),dfTime);
    SetMOOSVar("Time", dfTime, dfWhen);
    /////////////////////////////////////
    //          latitude..
    sTmp = MOOSChomp(sNMEAString, ",");
    if(sTmp.size() == 0)
      bGood = false;
    
    double dfLat = atof(sTmp.c_str());
    
    sTmp = MOOSChomp(sNMEAString, ",");
    string sNS	= sTmp;
    if (sNS == "S") {
      dfLat *= -1.0;
    }
    
    //////////////////////////////////////
    //          longitude
    sTmp = MOOSChomp(sNMEAString, ",");
    if(sTmp.size() == 0)
      bGood = false;
    
    double dfLong = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ",");
    string sEW = sTmp;
    
    if (sEW == "W") {
      dfLong *= -1.0;
    }
    
    ////////////////////////////////////////
    //          quality
    
    sTmp = MOOSChomp(sNMEAString, ",");
    double dfHDOP = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ",");
    double dfSatellites = atof(	sTmp.c_str());
    if (dfSatellites < 4)
      bGood = false;
    
    /////////////////////////////////////////
    //GEODETIC CONVERSION....
    
    double dfLatDecDeg  = m_Geodesy.DMS2DecDeg(dfLat);
    double dfLongDecDeg = m_Geodesy.DMS2DecDeg(dfLong);
    double dfELocal;
    double dfNLocal;
    double dfXLocal;
    double dfYLocal;

    if (m_Geodesy.LatLong2LocalUTM(dfLatDecDeg, dfLongDecDeg, dfNLocal, dfELocal)) {
      //set our GPS local Northings and Eastings variables
      if (bGood) {
	SetMOOSVar("N", dfNLocal, dfWhen);
	SetMOOSVar("E", dfELocal, dfWhen);
      }
    }
    
    if (m_Geodesy.LatLong2LocalGrid(dfLatDecDeg, dfLongDecDeg, dfYLocal, dfXLocal)) {
      //set our GPS local Grid variables
      if (bGood) {
	SetMOOSVar("X", dfXLocal, dfWhen);
	SetMOOSVar("Y", dfYLocal, dfWhen);
      }
    }
    
    //always say how many satellites we have...
    SetMOOSVar("Satellites", dfSatellites, dfWhen);
    
    char tmp[160];
    snprintf(tmp, 160, "time=%lf,n=%lf,e=%lf,x=%lf,y=%lf,sat=%i,lat=%lf,lon=%lf",
	     dfWhen, dfNLocal, dfELocal, dfXLocal,
	     dfYLocal, (int)dfSatellites, dfLatDecDeg, dfLongDecDeg);
    
    m_Comms.Notify("GPS_SUMMARY", tmp);
    m_Comms.Notify("GPS_LATITUDE", dfLatDecDeg);
    m_Comms.Notify("GPS_LONGITUDE", dfLongDecDeg);
    
    return true;
  } else if(sWhat == "$GPRMC") {
    NMEAMessage m(sCopy);
    
    if(strlen(m.Part(7).c_str())) {
      m_Comms.Notify("GPS_SPEED", atof(m.Part(7).c_str()) * 0.51444444,dfWhen);
    }
    
    if(strlen(m.Part(8).c_str())) {
      m_Comms.Notify("GPS_HEADING", atof(m.Part(8).c_str()),dfWhen);
    }
    
    if(strlen(m.Part(10).c_str())) {
      double f = atof(m.Part(10).c_str());
      
      if(m.Part(11)[0] == 'W') f = 0 - f;
      
      m_Comms.Notify("GPS_MAGNETIC_DECLINATION", f,dfWhen);
    }
  }
  
  return false;
}

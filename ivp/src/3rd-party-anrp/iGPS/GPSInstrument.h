// $Header: /home/cvsroot/project-marine-shell/src/iGPS/GPSInstrument.h,v 1.3 2007/07/28 17:18:35 mikerb Exp $
// copyright (2001-2003) Massachusetts Institute of Technology (pnewman et al.)

// GPSInstrument.h: interface for the CGPSInstrument class.
//
//////////////////////////////////////////////////////////////////////


#ifndef GPS_INSTRUMENT_HEADER
#define GPS_INSTRUMENT_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "MOOSGeodesy.h"

class CGPSInstrument : public CMOOSInstrument
{
 public:
  CGPSInstrument();
  virtual ~CGPSInstrument();
  
 protected:
  CMOOSGeodesy m_Geodesy;
  bool ParseNMEAString(std::string & sNMEAString, double dfWhen);
  bool InitialiseSensor();
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();
  bool GetData();
  bool PublishData();
  std::string m_sType;
};

#endif 


// "$GPCGA, TIME, LAT, LON, HDOP, SAT#, 

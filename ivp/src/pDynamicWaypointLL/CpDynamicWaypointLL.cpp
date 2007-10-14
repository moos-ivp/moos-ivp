// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/control/pDynamicWaypointLL/CpDynamicWaypointLL.cpp,v 1.1 2005/09/10 01:41:45 anrp Exp $
// (c) 2004

// CpDynamicWaypointLL.cpp: implementation of the CpDynamicWaypointLL class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CpDynamicWaypointLL.h"
#include <math.h>

CpDynamicWaypointLL::CpDynamicWaypointLL()
{
	// constructor
	hitr = oxp = oyp = mxp = myp = myaw = last_remote_pos = 0.0;
	last_local_pos = 0.0;
	xdist = ydist = oyaw = 0.0;
	latv = longv = "";
	got_pos = false;
	YawPID.SetMaxMinIntegrator(-2, 2);
	YawPID.SetGains(0.5, 0.006, 0.1);
}

CpDynamicWaypointLL::~CpDynamicWaypointLL()
{
	// destructor
}

bool CpDynamicWaypointLL::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.IsSkewed(MOOSTime())) continue;

		if(msg.m_sKey == "GPS_X") {
			mxp = msg.m_dfVal;
			last_local_pos = MOOSTime();
		} else if(msg.m_sKey == "GPS_Y") {
			myp = msg.m_dfVal;
			last_local_pos = MOOSTime();
		} else if(msg.m_sKey == "GPS_HEADING") {
			myaw = msg.m_dfVal;
			myaw *= -M_PI/180.0;
			last_local_pos = MOOSTime();
		} else if(msg.m_sKey == latv) {
			olatpd = msg.m_dfVal;
			if(olongpd != 0) {
				got_pos = true;
			}
		} else if(msg.m_sKey == longv) {
			olongpd = msg.m_dfVal;
			if(olatpd != 0) {
				got_pos = true;
			}
		}
	}

	NewMail.clear();
	
	return true;
}

bool CpDynamicWaypointLL::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	m_MissionReader.GetConfigurationParam("HitRadius", hitr);
	m_MissionReader.GetConfigurationParam("LatVar", latv);
	m_MissionReader.GetConfigurationParam("LongVar", longv);

	m_MissionReader.GetValue("LongOrigin", longorigin);
	m_MissionReader.GetValue("LatOrigin", latorigin);

	m_MissionReader.GetConfigurationParam("DefaultLat", olatpd);
	m_MissionReader.GetConfigurationParam("DefaultLong", olongpd);
	got_pos = true;

	m_Geodesy.Initialise(latorigin, longorigin);

	m_Comms.Register("GPS_X", 0);
	m_Comms.Register("GPS_Y", 0);
	m_Comms.Register("GPS_HEADING", 0);
	m_Comms.Register("DESIRED_THRUST", 0);
	m_Comms.Register("DESIRED_RUDDER", 0);
	m_Comms.Register(latv, 0);
	m_Comms.Register(longv, 0);

	string mydb;
	m_MissionReader.GetConfigurationParam("ThisDBName", mydb);

	return true;
}

bool CpDynamicWaypointLL::Iterate()
{
	// happens AppTick times per second

	double reqV, reqT;

	reqV = reqT = 0.0;

	double oxpd, oypd;

	if(got_pos && last_local_pos + 10.0 > MOOSTime()) {
		// good to do something
		double iny = (oyaw);// - M_PI/2;
		double rd = atan2(ydist, xdist);
		double dist = hypot(xdist, ydist);
		rd = -rd + iny;
		rd -= M_PI;

		m_Geodesy.LatLong2LocalGrid(olatpd, olongpd, oypd, oxpd);

		double d = sqrt((oxpd - mxp) * (oxpd - mxp) +
			        (oypd - myp) * (oypd - myp));
		
		if(d > hitr) {
			if(d > 2*hitr) {
				reqV = 100.0;
			} else {
				reqV = 100 * (d - hitr) / hitr;
			}
		}

		double dt = (myaw) - atan2(oypd - myp, oxpd - mxp);
		double deg_off = dt * 180 / M_PI;

		deg_off += 90;

		while(deg_off < -180) deg_off += 360;
		while(deg_off >  180) deg_off -= 360;
		
		fprintf(stderr, "doff = %f, myaw = %f\n", deg_off, myaw * 180/M_PI);
		double out = YawPID.Run(deg_off, myaw);

		if(out > 180) {
			reqT = 100.0;
		} else if(out < -180) {
			reqT = -100.0;
		} else {
			reqT = out / 180.0 * 100.0;
		}

		MOOSTrace("distance = %f\n", d);
	}

	MOOSTrace("mxp=%f, myp=%f, mt=%f, oxp=%f, oyp=%f\nCommanded: th=%f rud=%f\n",
		  mxp, myp, myaw, oxpd, oypd, reqV, reqT);

	m_Comms.Notify("DESIRED_RUDDER", reqT);
	m_Comms.Notify("DESIRED_THRUST", reqV);
	
	return true;
}

bool CpDynamicWaypointLL::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}


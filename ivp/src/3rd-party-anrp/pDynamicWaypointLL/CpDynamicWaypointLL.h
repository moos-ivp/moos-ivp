// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/control/pDynamicWaypointLL/CpDynamicWaypointLL.h,v 1.1 2005/09/10 01:41:45 anrp Exp $
// (c) 2004 
// CpDynamicWaypointLL.h: interface for the CpDynamicWaypointLL class.
////////////////////////////////////////////////

#ifndef __CpDynamicWaypointLL_h__
#define __CpDynamicWaypointLL_h__

#include "MOOSLib.h"
#include "MOOSGeodesy.h"
#include "SimplePID.h"
#include <string>
#include <map>

#include "SimulatedSurfaceObject.h"

using namespace std;

class CpDynamicWaypointLL : public CMOOSApp
{
public:
	CpDynamicWaypointLL();
	virtual ~CpDynamicWaypointLL();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	double last_remote_pos, last_local_pos;
	double oxp, oyp, oyaw, mxp, myp, myaw;
	double olatpd, olongpd;

	double hitr;
	double xdist, ydist;
	bool got_pos;

	string latv, longv;

	SimplePID YawPID;

	CMOOSGeodesy m_Geodesy;
	double latorigin, longorigin;

};

#endif /* __CpDynamicWaypointLL_h__ */

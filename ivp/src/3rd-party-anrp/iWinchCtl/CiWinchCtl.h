// $Header: /home/cvs/repository/project-plusnet/src/iWinchCtl/CiWinchCtl.h,v 1.1 2006/08/22 16:45:39 cvs Exp $
// (c) 2005 
// CiWinchCtl.h: interface for the CiWinchCtl class.
////////////////////////////////////////////////

#ifndef __CiWinchCtl_h__
#define __CiWinchCtl_h__

#include "MOOSLib.h"

class CiWinchCtl : public CMOOSApp
{
public:
	CiWinchCtl();
	virtual ~CiWinchCtl();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	double depth, depth_time;
	double wanted_depth;
	bool seeking;
	double encposcurrent;
	double encjogdown, encjogup;
	double encseekdown, encseekup;
};

#endif /* __CiWinchCtl_h__ */

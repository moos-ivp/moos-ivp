// $Header: /home/cvsroot/project-marine-shell/src/pGPSReTx/CpGPSReTx.h,v 1.4 2007/08/03 19:58:50 anrp Exp $
// (c) 2004 
// CpGPSReTx.h: interface for the CpGPSReTx class.
////////////////////////////////////////////////

#ifndef __CpGPSReTx_h__
#define __CpGPSReTx_h__

#include "MOOSLib.h"
#include "CClientTCPSocket.h"

class CpGPSReTx : public CMOOSApp
{
public:
	CpGPSReTx();
	virtual ~CpGPSReTx();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	CClientTCPSocket *sock;

	double last_tx;
	double min_time_between_tx;

	std::string last_gprmc;
};

#endif /* __CpGPSReTx_h__ */

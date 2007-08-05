// $Header: /home/cvsroot/project-marine-shell/src/iWifi/CiWifi.h,v 1.1.1.1 2005/08/08 01:17:14 cvsadmin Exp $
// (c) 2004 
// CiWifi.h: interface for the CiWifi class.
////////////////////////////////////////////////

#ifndef __CiWifi_h__
#define __CiWifi_h__

#include "MOOSLib.h"
#include <string>

using namespace std;

class CiWifi : public CMOOSApp
{
public:
	CiWifi();
	virtual ~CiWifi();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	
	string interface;
};

#endif /* __CiWifi_h__ */

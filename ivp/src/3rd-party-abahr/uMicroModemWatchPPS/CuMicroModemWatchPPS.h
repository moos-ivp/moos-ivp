// $Header: /home/cvsroot/project-plus/src/uMicroModemWatchPPS/CuMicroModemWatchPPS.h,v 1.4 2007-08-22 15:03:58 abahr Exp $
// (c) 2005 
// CuMicroModemWatchPPS.h: interface for the CuMicroModemWatchPPS class.
////////////////////////////////////////////////

#ifndef __CuMicroModemWatchPPS_h__
#define __CuMicroModemWatchPPS_h__

#include "MOOSLib.h"
#include "MU.h"
#include <stdio.h>
#include <string>

class CuMicroModemWatchPPS : public CMOOSApp
{
public:
	CuMicroModemWatchPPS();
	virtual ~CuMicroModemWatchPPS();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	
	bool verbose;
	
	int timeout_counter,max_timeout;
	
	string sVal;
	string modem_prefix,modem_to_modem,modem_raw,modem_command,modem_from_modem;
	
	
	bool DoSubscribe();
	bool DoReadConfig();
	bool DoFormVarNames();
	bool DoRegister();
	
	bool DisablePPS();
	bool EnablePPS();
	
};

#endif /* __CuMicroModemWatchPPS_h__ */

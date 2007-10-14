// $Header: /home/cvsroot/project-marine-shell/src/iBatteryMonitor/CiBatteryMonitor.h,v 1.4 2007/08/03 19:58:50 anrp Exp $
// (c) 2004 
// CiBatteryMonitor.h: interface for the CiBatteryMonitor class.
////////////////////////////////////////////////

#ifndef __CiBatteryMonitor_h__
#define __CiBatteryMonitor_h__

#include "MOOSLib.h"
#include "CSerialPort.h"

class CiBatteryMonitor : public CMOOSApp
{
public:
	CiBatteryMonitor();
	virtual ~CiBatteryMonitor();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	CSerialPort *pt;

	pthread_t thr;
	static void *trampoline(void *arg) {
		((CiBatteryMonitor *)arg)->thread();
		return NULL;
	}

	void thread(void);
};

#endif /* __CiBatteryMonitor_h__ */

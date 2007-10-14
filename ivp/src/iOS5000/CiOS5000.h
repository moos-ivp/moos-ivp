// $Header: /home/cvsroot/project-marine-shell/src/iOS5000/CiOS5000.h,v 1.4 2007/08/03 19:58:50 anrp Exp $
// (c) 2004 
// CiOS5000.h: interface for the CiOS5000 class.
////////////////////////////////////////////////

#ifndef __CiOS5000_h__
#define __CiOS5000_h__

#include "MOOSLib.h"
#include "CSerialPort.h"

class CiOS5000 : public CMOOSApp
{
public:
	CiOS5000();
	virtual ~CiOS5000();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	CSerialPort *pt;

	double prerotation;

	pthread_t thr;
	static void *trampoline(void *arg) {
		((CiOS5000 *)arg)->thread();
		return NULL;
	}

	void thread(void);
};

#endif /* __CiOS5000_h__ */

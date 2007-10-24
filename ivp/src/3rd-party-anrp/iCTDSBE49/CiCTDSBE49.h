// $Header: /home/cvsroot/project-plusnet-shell/src/iCTDSBE49/CiCTDSBE49.h,v 1.2 2006/08/29 01:55:42 mikerb Exp $
// (c) 2005 
// CiCTDSBE49.h: interface for the CiCTDSBE49 class.
////////////////////////////////////////////////

#ifndef __CiCTDSBE49_h__
#define __CiCTDSBE49_h__

#include "MOOSLib.h"
#include "CSerialPort.h"

class CiCTDSBE49 : public CMOOSApp
{
public:
	CiCTDSBE49();
	virtual ~CiCTDSBE49();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	pthread_t cTH;
	static void *trampoline(void *);
	void CommThreadProc();
	CSerialPort *pt;
};

#endif /* __CiCTDSBE49_h__ */

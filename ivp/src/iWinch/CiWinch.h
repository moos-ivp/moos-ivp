// $Header: /home/cvs/repository/project-plusnet/src/iWinch/CiWinch.h,v 1.1 2006/08/22 16:45:39 cvs Exp $
// (c) 2005 
// CiWinch.h: interface for the CiWinch class.
////////////////////////////////////////////////

#ifndef __CiWinch_h__
#define __CiWinch_h__

#include "MOOSLib.h"
#include "CWinch.h"

class CiWinch : public CMOOSApp
{
public:
	CiWinch();
	virtual ~CiWinch();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	CWinch *winch;
};

#endif /* __CiWinch_h__ */

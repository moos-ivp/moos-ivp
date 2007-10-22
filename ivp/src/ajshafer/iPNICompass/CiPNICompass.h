// $Header: /home/cvsroot/project-marine-shell/src/iPNICompass/CiPNICompass.h,v 1.1.1.1 2005/08/08 01:17:13 cvsadmin Exp $
// (c) 2004
// CiPNICompass.h: interface for the CiPNICompass class.
////////////////////////////////////////////////

#ifndef __CiPNICompass_h__
#define __CiPNICompass_h__

#include "MOOSLib.h"
#include "CPNICompass.h"

class CiPNICompass : public CMOOSApp
{

	public:
		CiPNICompass();
		virtual ~CiPNICompass();

		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();

	protected:
		// insert local vars here

		CPNICompass *comp;
		string suffix;
		double prerot;
		double magdec;
};

#endif /* __CiPNICompass_h__ */

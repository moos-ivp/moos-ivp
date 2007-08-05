// $Header: /home/cvsroot/project-marine-shell/src/iWifi/CiWifiMain.cpp,v 1.1.1.1 2005/08/08 01:17:14 cvsadmin Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiWifi.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iWifi.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiWifi iWifi;
	
	iWifi.Run("iWifi", sMissionFile);

	return 0;
}


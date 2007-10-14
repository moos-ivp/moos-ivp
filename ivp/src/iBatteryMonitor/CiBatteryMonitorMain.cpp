// $Header: /home/cvsroot/project-marine-shell/src/iBatteryMonitor/CiBatteryMonitorMain.cpp,v 1.2 2007/08/03 19:18:14 mikerb Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiBatteryMonitor.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iBatteryMonitor.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiBatteryMonitor iBatteryMonitor;
	
	iBatteryMonitor.Run("iBatteryMonitor", sMissionFile);

	return 0;
}


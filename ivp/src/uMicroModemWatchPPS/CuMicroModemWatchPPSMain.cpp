// $Header: /home/cvsroot/project-plus/src/uMicroModemWatchPPS/CuMicroModemWatchPPSMain.cpp,v 1.1 2007-08-05 14:03:02 abahr Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CuMicroModemWatchPPS.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "uMicroModemWatchPPS.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CuMicroModemWatchPPS uMicroModemWatchPPS;
	
	uMicroModemWatchPPS.Run("uMicroModemWatchPPS", sMissionFile);

	return 0;
}


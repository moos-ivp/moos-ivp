// $Header: /home/cvsroot/project-plus/src/uMicroModemAddPPS/CuMicroModemAddPPSMain.cpp,v 1.1 2007-08-14 19:41:33 abahr Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CuMicroModemAddPPS.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "uMicroModemAddPPS.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CuMicroModemAddPPS uMicroModemAddPPS;
	
	uMicroModemAddPPS.Run("uMicroModemAddPPS", sMissionFile);

	return 0;
}


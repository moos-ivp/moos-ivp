// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/multi/MOOSBlink/CMOOSBlinkMain.cpp,v 1.1.1.1 2004/11/16 22:37:41 anrp Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CMOOSBlink.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "MOOSBlink.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CMOOSBlink MOOSBlink;
	
	MOOSBlink.Run("MOOSBlink", sMissionFile);

	return 0;
}


// $Header: /home/cvsroot/project-plusnet-shell/src/iCTDSBE49/CiCTDSBE49Main.cpp,v 1.2 2006/08/29 01:55:43 mikerb Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiCTDSBE49.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iCTDSBE49.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiCTDSBE49 iCTDSBE49;
	
	iCTDSBE49.Run("iCTDSBE49", sMissionFile);

	return 0;
}


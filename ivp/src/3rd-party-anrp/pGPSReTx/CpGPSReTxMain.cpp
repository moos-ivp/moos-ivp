// $Header: /home/cvsroot/project-marine-shell/src/pGPSReTx/CpGPSReTxMain.cpp,v 1.2 2007/08/03 19:18:14 mikerb Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CpGPSReTx.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "pGPSReTx.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CpGPSReTx pGPSReTx;
	
	pGPSReTx.Run("pGPSReTx", sMissionFile);

	return 0;
}


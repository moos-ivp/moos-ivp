// $Header: /home/cvsroot/project-marine-shell/src/pMineSim/CpMineSimMain.cpp,v 1.1 2007/06/11 21:00:24 ajshafer Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CpMineSim.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "pMineSim.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CpMineSim pMineSim;
	
	pMineSim.Run("pMineSim", sMissionFile);

	return 0;
}


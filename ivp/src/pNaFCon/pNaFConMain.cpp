// $Header: /home/cvsroot/project-plus/src/pNaFCon/pNaFConMain.cpp,v 1.1.1.1 2007-09-28 16:38:01 ajshafer Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "pNaFCon.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "pNaFCon.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CpNaFCon pNaFCon;
	
	pNaFCon.Run("pNaFCon", sMissionFile);

	return 0;
}


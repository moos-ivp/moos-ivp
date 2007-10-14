// $Header: /home/cvs/repository/project-plusnet/src/iWinchCtl/CiWinchCtlMain.cpp,v 1.1 2006/08/22 16:45:39 cvs Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiWinchCtl.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iWinchCtl.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiWinchCtl iWinchCtl;
	
	iWinchCtl.Run("iWinchCtl", sMissionFile);

	return 0;
}


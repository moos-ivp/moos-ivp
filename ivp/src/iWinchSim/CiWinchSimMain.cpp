// $Header: /home/cvs/repository/project-plusnet/src/iWinch/CiWinchMain.cpp,v 1.1 2006/08/22 16:45:39 cvs Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiWinchSim.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iWinchSim.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiWinchSim iWinch;
	
	iWinch.Run("iWinchSim", sMissionFile);

	return 0;
}


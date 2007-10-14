// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/control/pDynamicWaypointLL/CpDynamicWaypointLLMain.cpp,v 1.1 2005/09/10 01:41:45 anrp Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CpDynamicWaypointLL.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "pDynamicWaypointLL.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CpDynamicWaypointLL pDynamicWaypointLL;
	
	pDynamicWaypointLL.Run("pDynamicWaypointLL", sMissionFile);

	return 0;
}


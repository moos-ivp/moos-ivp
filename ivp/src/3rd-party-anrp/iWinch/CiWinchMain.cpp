// $Header: /home/cvs/repository/project-plusnet/src/iWinch/CiWinchMain.cpp,v 1.1 2006/08/22 16:45:39 cvs Exp $
// (c) 2005

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiWinch.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iWinch.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiWinch iWinch;
	malloc(1048576);
	
	iWinch.Run("iWinch", sMissionFile);

	return 0;
}


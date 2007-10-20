// $Header: /home/cvsroot/project-marine-shell/src/iOS5000/CiOS5000Main.cpp,v 1.2 2007/08/03 19:18:14 mikerb Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiOS5000.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iOS5000.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiOS5000 iOS5000;
	
	iOS5000.Run("iOS5000", sMissionFile);

	return 0;
}


// $Header: /home/cvsroot/project-marine-shell/src/iPWMController/CiPWMControllerMain.cpp,v 1.2 2007/08/03 19:18:14 mikerb Exp $
// (c) 2004

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CiPWMController.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "iPWMController.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CiPWMController iPWMController;
	
	iPWMController.Run("iPWMController", sMissionFile);

	return 0;
}


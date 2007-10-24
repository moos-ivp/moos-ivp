#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "BearingTrack.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "Mission.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CBearings BearingApp;
	
	BearingApp.Run("pBearings", sMissionFile);

	return 0;
}


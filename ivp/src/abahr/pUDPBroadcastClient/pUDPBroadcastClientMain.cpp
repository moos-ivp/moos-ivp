#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "pUDPBroadcastClient.h"

int main(int argc, char *argv[])
{
	char *sMissionFile = "Mission.moos";
	
	if(argc > 1) {
		sMissionFile = argv[1];
	}
	
	CpUDPBroadcastClient pUDPBroadcastClient;
	
	pUDPBroadcastClient.Run("pUDPBroadcastClient", sMissionFile);

	return 0;
}


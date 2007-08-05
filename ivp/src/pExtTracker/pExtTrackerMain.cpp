#include <MOOSLib.h>
#include "ExtTracker.h"

int main(int argc ,char * argv[])
{
	char * sMissionFile = "Mission.moos";
	char * sMOOSName = "pExtTracker";

	switch(argc)
	{
	case 3:
	  sMOOSName = argv[2];
	case 2:
	  sMissionFile = argv[1];
	}

    CExtTracker ExtTracker;

    ExtTracker.Run(sMOOSName,sMissionFile);


    return 0;
}

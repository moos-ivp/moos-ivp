#include <MOOSLib.h>
#include "2BTracker.h"

int main(int argc ,char * argv[])
{
	char * sMissionFile = "Mission.moos";
	char * sMOOSName = "p2BTracker";

	switch(argc)
	{
	case 3:
	  sMOOSName = argv[2];
	case 2:
	  sMissionFile = argv[1];
	}

    C2BTracker Tracker;

    Tracker.Run(sMOOSName,sMissionFile);


    return 0;
}

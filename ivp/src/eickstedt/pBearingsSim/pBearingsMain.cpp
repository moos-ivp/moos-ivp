#include <MOOSLib.h>
#include "Bearings.h"

int main(int argc ,char * argv[])
{
	char * sMissionFile = "Mission.moos";
	char * sMOOSName = "pBearings";

	switch(argc)
	{
	case 3:
	  sMOOSName = argv[2];
	case 2:
	  sMissionFile = argv[1];
	}

    CBearings Bearings;

    Bearings.Run(sMOOSName,sMissionFile);


    return 0;
}

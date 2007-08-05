#include <MOOSLib.h>
#include "Target.h"

int main(int argc ,char * argv[])
{
	char * sMissionFile = "Mission.moos";
	char * sMOOSName = "pTarget";

	switch(argc)
	{
	case 3:
	  sMOOSName = argv[2];
	case 2:
	  sMissionFile = argv[1];
	}

    CTarget Target;

    Target.Run(sMOOSName,sMissionFile);


    return 0;
}

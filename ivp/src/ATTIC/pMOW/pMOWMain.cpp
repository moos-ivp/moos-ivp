
#include "MOW.h"



int main(int argc ,char * argv[])
{
	char * sMissionFile = "Mission.moos";
	char * sMOOSName = "pMOW";

	switch(argc)
	{
	case 3:
		sMOOSName = argv[2];
	case 2:
        sMissionFile = argv[1];
	}

    CMOW TheMOWThang;

    TheMOWThang.Run(sMOOSName,sMissionFile);

    return 0;
}

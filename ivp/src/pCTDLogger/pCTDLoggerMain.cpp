#include <MOOSLib.h>
#include "CTDLogger.h"

int main(int argc ,char * argv[])
{

    char * sMissionFile = "Mission.moos";

    if(argc>1)
    {
        sMissionFile = argv[1];
    }

    CTDLogger Logger;

    Logger.Run("pCTDLogger",sMissionFile);


    return 0;
}

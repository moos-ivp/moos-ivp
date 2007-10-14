#include <MOOSLib.h>
#include "SamplingControl.h"

int main(int argc ,char * argv[])
{

    char * sMissionFile = "Mission.moos";

    if(argc>1)
    {
        sMissionFile = argv[1];
    }

    SamplingControl Control;

    Control.Run("pSamplingControl",sMissionFile);


    return 0;
}

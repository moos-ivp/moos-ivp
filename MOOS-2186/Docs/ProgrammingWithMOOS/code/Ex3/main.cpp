
 #include "Simulator.h"

 //simple "main" file which serves to build and run a CMOOSApp-derived application


int main(int argc ,char * argv[])
{
    //set up some default application parameters

    //what's the name of the configuarion file that the application
    //should look in if it needs to read parameters?
    const char * sMissionFile = "Mission.moos";

    //under what name shoud the application register with the MOOSDB?
    const char * sMOOSName = "Simulator";

    switch(argc)
    {
    case 3:
    //command line says don't register with default name
    sMOOSName = argv[2];
    case 2:
    //command line says don't use default "mission.moos" config file
    sMissionFile = argv[1];
    }

    //make an application
    CSimulator TheApp;

    //run forever pasing registration name and mission file parameters
    TheApp.Run(sMOOSName,sMissionFile);

    //probably will never get here..
    return 0;
}

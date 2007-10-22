/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: SensorSimMain.cpp                               */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "SensorSim.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "SensorSim.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  SensorSim SensorSim;
	
  SensorSim.Run("pSensorSim", sMissionFile);

  return(0);
}


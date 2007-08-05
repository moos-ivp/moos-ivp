/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ArtifactSensorSimMain.cpp                       */
/*    DATE: Dec 29th 1963                                   */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ArtifactSensorSim.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "ArtifactSensorSim.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  ArtifactSensorSim ArtifactSensorSim;
	
  ArtifactSensorSim.Run("pArtifactSensorSim", sMissionFile);

  return(0);
}


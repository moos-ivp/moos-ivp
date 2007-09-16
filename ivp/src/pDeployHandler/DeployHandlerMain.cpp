/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandlerMain.cpp                           */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "DeployHandler.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "DeployHandler.moos";
  
  if(argc > 1)
    sMissionFile = argv[1];
  
  DeployHandler DeployHandler;
	
  DeployHandler.Run("pDeployHandler", sMissionFile);
  
  return(0);
}


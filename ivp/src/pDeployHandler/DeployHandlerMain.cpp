/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandlerMain.cpp                           */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "DeployHandler.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "DeployHandler.moos";
  string sMOOSName = "pDeployHandler";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  DeployHandler DeployHandler;
	
  DeployHandler.Run(sMOOSName.c_str(), sMissionFile.c_str());
  
  return(0);
}





/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelayMain.cpp                             */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ViewerRelay.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "ViewerRelay.moos";
  string sMOOSName = "pViewerRelay";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  ViewerRelay ViewerRelay;
	
  ViewerRelay.Run(sMOOSName.c_str(), sMissionFile.c_str());

  return(0);
}


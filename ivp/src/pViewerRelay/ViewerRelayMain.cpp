/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelayMain.cpp                             */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ViewerRelay.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "ViewerRelay.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  ViewerRelay ViewerRelay;
	
  ViewerRelay.Run("pViewerRelay", sMissionFile);

  return(0);
}


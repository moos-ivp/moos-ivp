/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: main.cpp                                        */
/*    DATE: Jun 26th 2008                                   */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Relayer.h"

int main(int argc, char *argv[])
{

  char *sMissionFile = "Mission.moos";
  char *sMOOSName = "pRelayer";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  Relayer relayer;
	
  relayer.Run(sMOOSName, sMissionFile);

  return(0);
}


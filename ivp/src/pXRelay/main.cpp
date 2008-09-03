/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: main.cpp                                        */
/*    DATE: Jun 26th 2008                                   */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Relayer.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "Mission.moos";
  string sMOOSName = "pXRelay";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  Relayer relayer;
	
  relayer.Run(sMOOSName.c_str(), sMissionFile.c_str());

  return(0);
}


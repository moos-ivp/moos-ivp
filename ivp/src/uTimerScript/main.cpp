/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: main.cpp                                        */
/*    DATE: May 21 2009                                     */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TS_MOOSApp.h"

int main(int argc, char *argv[])
{
  std::string sMissionFile = "TimerScript.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  TS_MOOSApp TimerScript;
	
  TimerScript.Run(argv[0], sMissionFile.c_str());

  return(0);
}


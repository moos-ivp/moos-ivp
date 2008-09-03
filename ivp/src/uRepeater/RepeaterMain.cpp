/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: RepeaterMain.cpp                                */
/*    DATE: Oct 5th 2007                                    */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Repeater.h"

using namespace std;

int main(int argc, char *argv[])
{
  string sMissionFile = "Repeater.moos";
  string sMOOSName = "uRepeater";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  Repeater Repeater;
  
  Repeater.Run(sMOOSName.c_str(), sMissionFile.c_str());

  return(0);
}


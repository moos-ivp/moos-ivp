/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: RepeaterMain.cpp                                */
/*    DATE: Oct 5th 2007                                    */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Repeater.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "Repeater.moos";
  
  if(argc > 1)
    sMissionFile = argv[1];
  
  Repeater Repeater;
  
  Repeater.Run("uRepeater", sMissionFile);

  return(0);
}


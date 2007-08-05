/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: mainTrafficControl.cpp                               */
/*    DATE: July 17th 2007                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TC_MOOSApp.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "pTrafficControl.moos";
  
  if(argc > 1)
    sMissionFile = argv[1];

  TC_MOOSApp traffic_controller;
  
  traffic_controller.Run("pTrafficControl", sMissionFile);

  return(0);
}






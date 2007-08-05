/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: March 2005                                           */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Drone.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "pDrone.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  Drone pDrone;
  
  pDrone.Run("pDrone", sMissionFile);
  
  return 0;
}






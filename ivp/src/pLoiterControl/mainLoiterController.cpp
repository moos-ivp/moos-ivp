/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: mainLoiterControl.cpp                                */
/*    DATE: Feb 9th 2006                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "LoiterController.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "pLoiterController.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  LoiterController loiter_controller;
  
  loiter_controller.Run("pLoiterControl", sMissionFile);

  return 0;
}







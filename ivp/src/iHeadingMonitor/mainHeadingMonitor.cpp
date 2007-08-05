/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: mainHeadingMonitor.cpp                               */
/*    DATE: June 14th 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "HeadingMonitor.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "iHeadingMonitor.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  HeadingMonitor meta_compass;
  
  meta_compass.Run("iHeadingMonitor", sMissionFile);

  return 0;
}






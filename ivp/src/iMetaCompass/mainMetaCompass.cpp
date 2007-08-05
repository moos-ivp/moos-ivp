/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: mainMetaCompass.cpp                                  */
/*    DATE: June 14th 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MetaCompass.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "iMetaCompass.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  MetaCompass meta_compass;
  
  meta_compass.Run("iMetaCompass", sMissionFile);

  return 0;
}






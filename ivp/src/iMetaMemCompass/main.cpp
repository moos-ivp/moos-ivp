/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 3rd 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MetaMemCompass.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "iMetaMemCompass.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  MetaMemCompass meta_compass;
  
  meta_compass.Run("iMetaMemCompass", sMissionFile);

  return 0;
}






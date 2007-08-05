/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Aug 20th 2006                                        */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Eventor.h"

//-----------------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  char *sMissionFile = "pEventor.moos";
  
  if(argc > 1) {
    sMissionFile = argv[1];
  }
  
  Eventor pEventor;
  
  pEventor.Run("pEventor", sMissionFile);

  return(0);
}


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 27th 2007  (MINUS-07)                            */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ProcessWatch.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "ProcessWatch.moos";
  
  if(argc > 1) 
    sMissionFile = argv[1];
  
  ProcessWatch ProcessWatch;
  
  ProcessWatch.Run("uProcessWatch", sMissionFile);
  
  return(0);
}


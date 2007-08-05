/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WatchDog.cpp                                             */
/*    DATE: December 9th 2004                                    */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "WatchDog.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "WatchDog.moos";
	
  if(argc > 1) 
     sMissionFile = argv[1];
	
     WatchDog WatchDog;
	
     WatchDog.Run("WatchDog", sMissionFile);

     return(0);
}


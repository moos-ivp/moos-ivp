/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: LawnmowerMain.cpp                               */
/*    DATE: Nov 27 2007                                     */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "Lawnmower.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "Lawnmower.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  Lawnmower Lawnmower;
	
  Lawnmower.Run("pLawnmower", sMissionFile);

  return(0);
}


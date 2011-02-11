/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: SafetyRetreatMain.cpp                           */
/*    DATE: August 2010                                     */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "SafetyRetreat.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file = "SafetyRetreat.moos";
	
  if(argc > 1)
    mission_file = argv[1];
	
  SafetyRetreat SafetyRetreat;
	
  SafetyRetreat.Run("pSafetyRetreat", mission_file.c_str());

  return(0);
}


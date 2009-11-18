/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: main.cpp                                        */
/*    DATE: May 21 2009                                     */
/************************************************************/

#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uTimerScript");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  std::string sMissionFile = "TimerScript.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  TS_MOOSApp TimerScript;
	
  TimerScript.Run(argv[0], sMissionFile.c_str());

  return(0);
}


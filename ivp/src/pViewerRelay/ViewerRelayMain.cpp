/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelayMain.cpp                             */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ViewerRelay.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("pViewerRelay");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  string sMissionFile = "ViewerRelay.moos";
  string sMOOSName    = "pViewerRelay";

  switch(argc) {
  case 3:
    sMOOSName = argv[2];
  case 2:
    sMissionFile = argv[1];
  }
  
  ViewerRelay ViewerRelay;
	
  ViewerRelay.Run(sMOOSName.c_str(), sMissionFile.c_str());

  return(0);
}


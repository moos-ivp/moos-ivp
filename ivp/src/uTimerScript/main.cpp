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

  int    i;
  string mission_file = "";
  bool   help_requested = false;

  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argv[i];
    else if(strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if((argi == "--help")||(argi=="-h"))
      help_requested = true;
  }

  if((mission_file == "") || help_requested) {
    MOOSTrace("Usage: uTimerScript moosfile.moos \n");
    return(0);
  }

  TS_MOOSApp TimerScript;
  TimerScript.Run(argv[0], mission_file.c_str());

  return(0);
}


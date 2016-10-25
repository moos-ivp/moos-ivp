/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 24th, 2016                                       */
/*****************************************************************/

#include "MBUtils.h"
#include <cstdlib>
#include <iostream>
#include "HdgHandler.h"

using namespace std;

void showHelpAndExit();

//-------------------------------------------------------- 
// Procedure: main

int main(int argc, char *argv[])
{
  HdgHandler handler;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];

    bool handled = true;

    if((argi == "-h") || (argi == "--help") || (argi =="-help"))
      showHelpAndExit();
    else if(strEnds(argi, ".alog"))
      handled = handler.setALogFileIn(argi);
    else if(strEnds(argi, ".klog"))
      handled = handler.setKLogFileOut(argi);
    else if((argi == "--verbose") || (argi == "-v"))
      handler.setVerbose(true);
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      exit(1);
    }
  }

  handler.processALogFile();
  handler.processHeadingDeltas();
  handler.writeKLogFile();
  handler.writeReport();
  return(0);
}


//-------------------------------------------------------- 
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage:                                                    " << endl;
  cout << "  aloghdg file.alog [OPTIONS]                             " << endl;
  cout << "                                                          " << endl;
  cout << "Synopsis:                                                 " << endl;
  cout << "  A utilty for gathering heading information over a whole " << endl;
  cout << "  vehicle trajectory and comparing apparent heading vs    " << endl;
  cout << "  reported heading. The apparent heading is the heading   " << endl;
  cout << "  or angle between two successive waypoints. The reported " << endl;
  cout << "  heading is the value found in the NAV_HEADING variable. " << endl;  
  cout << "                                                          " << endl;
  cout << "Options:                                                  " << endl;
  cout << "  --verbose                  The ownship x-position       " << endl;
  exit (0);
}

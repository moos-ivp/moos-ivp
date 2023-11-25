/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 23rd, 2015                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <string>
#include <cstdlib>
#include <iostream>
#include "MBUtils.h"
#include "OpenURL.h"
#include "ReleaseInfo.h"
#include "CollisionReporter.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  CollisionReporter collision_reporter;

  bool handled = false;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogcd", "gpl");
      return(0);
    }
    else if((argi=="-t") || (argi=="--terse")) 
      collision_reporter.setTerse();
    else if(strEnds(argi, ".alog"))
      handled = collision_reporter.setALogFile(argi);
    else if(strBegins(argi, "--tfile="))
      handled = collision_reporter.setTimeStampFile(argi.substr(8));
    else if((argi == "-w") || (argi == "--web") || (argi == "-web")) {
      openURLX("https://oceanai.mit.edu/ivpman/apps/alogcd");
      exit(0);
    }

    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }
    
  bool ok = collision_reporter.handle();

  if(!ok)
    return(1);

  collision_reporter.printReport();
  if(collision_reporter.hadCollisions())
    return(2);
  if(!collision_reporter.hadEncounters())
    return(3);

  return(0);
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogcd file.alog [OPTIONS]                          " << endl;
  cout << "                                                      " << endl;
  cout << "Synopsis:                                             " << endl;
  cout << "  Scan an alog file for collision detection reports.  " << endl;
  cout << "  Tally totals and averages, and optionally create    " << endl;
  cout << "  a file holding all the timestamps of events.        " << endl;
  cout << "                                                      " << endl;
  cout << "  By default, it scans for events defined by postings " << endl;
  cout << "  to the following three MOOS variables:              " << endl;
  cout << "                                                      " << endl;
  cout << "  (1) COLLISION                                       " << endl;
  cout << "  (2) NEAR_MISS                                       " << endl;
  cout << "  (3) ENCOUNTER                                       " << endl;
  cout << "                                                      " << endl;
  cout << "Options:                                              " << endl;
  cout << "  -h,--help       Displays this help message          " << endl;
  cout << "  -v,--version    Display current release version     " << endl;
  cout << "  -t,--terse      Write terse output.                 " << endl;
  cout << "  --tfile=<file>  Write time-stamped events to file.  " << endl;
  cout << "                                                      " << endl;
  cout << "  --web,-w   Open browser to:                         " << endl;
  cout << "             https://oceanai.mit.edu/ivpman/apps/alogcd " << endl;
  cout << "                                                      " << endl;
  cout << "                                                      " << endl;
  cout << "Returns:                                              " << endl;
  cout << "  0 if alog file ok, has encounters, no collisions.   " << endl;
  cout << "  1 if alog file not ok, unable to open.              " << endl;
  cout << "  2 if alog ok, but collisions were detected          " << endl;
  cout << "  3 if alog ok, no collisions or encounters detected  " << endl;
  cout << "                                                      " << endl;
  cout << "Further Notes:                                        " << endl;
  cout << "  (1) The order of arguments is irrelevent.           " << endl;
  cout << "  (2) Only last specified .alog file is reported on.  " << endl;
  cout << endl;
  exit(0);
}


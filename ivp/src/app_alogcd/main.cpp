/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 3rd, 2008                                       */
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
#include "ReleaseInfo.h"
#include "CollisionReporter.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  string alogfile, tstamp_file;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogcd", "gpl");
      return(0);
    }
    if(strEnds(argi, ".alog"))
      alogfile = argi;
    else if(strBegins(argi, "--tfile="))
      tstamp_file = argi.substr(8);

  }

  if(alogfile == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }

  if((tstamp_file != "") && !okFileToWrite(tstamp_file)) {
    cout << "Time stamp file [" << tstamp_file << " not writeable." << endl;
    cout << "Exiting." << endl;
    exit(1);
  }
    
  CollisionReporter collision_reporter;
  collision_reporter.setTimeStampFile(tstamp_file);
  bool handled = collision_reporter.handle(alogfile);
  
  if(handled)
    collision_reporter.printReport();
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogcd file.alog [OPTIONS]                           " << endl;
  cout << "                                                       " << endl;
  cout << "Synopsis:                                              " << endl;
  cout << "  Scan an alog file for collision detection reports.   " << endl;
  cout << "  Tally the totals and averages, and optionally create " << endl;
  cout << "  a file holding all the timestamps of events.         " << endl;
  cout << "                                                       " << endl;
  cout << "  By default, it scans for events defined by postings  " << endl;
  cout << "  to the following three MOOS variables:               " << endl;
  cout << "                                                       " << endl;
  cout << "  (1) COLLISION                                        " << endl;
  cout << "  (2) NEAR_MISS                                        " << endl;
  cout << "  (3) ENCOUNTER                                        " << endl;
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  -h,--help       Displays this help message           " << endl;
  cout << "  -v,--version    Display current release version      " << endl;
  cout << "  --tfile=file    Write timestamp file to file.txt     " << endl;
  cout << "                                                       " << endl;
  cout << "Further Notes:                                         " << endl;
  cout << "  (1) The order of arguments is irrelevent.            " << endl;
  cout << "  (2) Only first specified .alog file is reported on.  " << endl;
  cout << endl;
  exit(0);
}


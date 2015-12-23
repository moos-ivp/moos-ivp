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

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogcd", "gpl");
    return(0);
  }

  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  aloghelm file.alog [OPTIONS] [MOOSVARS]       " << endl;
    cout << "                                                           " << endl;
    cout << "Synopsis:                                                  " << endl;
    cout << "  Perform one of several optional helm reports based on    " << endl;
    cout << "  helm output logged in the given .alog file.              " << endl;
    cout << "                                                           " << endl;
    cout << "Options:                                                   " << endl;
    cout << "  -h,--help     Displays this help message                 " << endl;
    cout << "  -v,--version  Displays the current release version       " << endl;
    cout << "  -l,--life     Show report on IvP Helm Life Events        " << endl;
    cout << "  -b,--bhvs     Show helm behavior state changes           " << endl;
    cout << "  -m,--modes    Show helm mode changes                     " << endl;
    cout << "  --watch=bhv   Watch a particular behavior for state change" << endl;
    cout << "  --nocolor     Turn off use of color coding               " << endl;
    cout << "  --notrunc     Don't truncate MOOSVAR output (on by default)" << endl;
    cout << "                                                           " << endl;
    cout << "Further Notes:                                             " << endl;
    cout << "  (1) The order of arguments is irrelevent.                " << endl;
    cout << "  (2) Only the first specified .alog file is reported on.  " << endl;
    cout << "  (3) Arguments that are not one of the above options or an" << endl;
    cout << "      alog file, are interpreted as MOOS variables on which" << endl;
    cout << "      to report as encountered.                            " << endl;
    cout << endl;
    return(0);
  }

  string alogfile;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".alog"))
      alogfile = argi;
    
  }
  
  if(alogfile == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }
  
  CollisionReporter collision_reporter;
  bool handled = collision_reporter.handle(alogfile);
  
  if(handled)
    collision_reporter.printReport();
}







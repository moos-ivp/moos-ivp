/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
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
#include "HelmReporter.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("aloghelm", "gpl");
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
    cout << "  (1) The order of arguments is irrelevant.                " << endl;
    cout << "  (2) Only the first specified .alog file is reported on.  " << endl;
    cout << "  (3) Arguments that are not one of the above options or an" << endl;
    cout << "      alog file, are interpreted as MOOS variables on which" << endl;
    cout << "      to report as encountered.                            " << endl;
    cout << endl;
    return(0);
  }

  vector<string> keys;
  vector<string> watch_vars;

  string alogfile_in;
  string watch_behavior;

  bool report_bhv_changes  = false;
  bool report_life_events  = false;
  bool report_mode_changes = false;

  bool use_colors = true;
  bool var_trunc  = true;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi == "-b") || (argi == "--bhvs"))
      report_bhv_changes = true;
    else if((argi == "-m") || (argi == "--modes"))
      report_mode_changes = true;
    else if((argi == "-l") || (argi == "--life"))
      report_life_events = true;
    else if((argi == "-l") || (argi == "--notrunc"))
      var_trunc = false;
    else if(strBegins(argi, "--watch="))
      watch_behavior = argi.substr(8);
    else if(argi == "--nocolor")
      use_colors = false;
    if(strEnds(argi, ".alog") && (alogfile_in == ""))
      alogfile_in = argi;
    else
      watch_vars.push_back(argi);
  }
 
  if(alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }
  
  HelmReporter hreporter;
  if(report_life_events)
    hreporter.reportLifeEvents();
  if(report_mode_changes)
    hreporter.reportModeChanges();
  if(report_bhv_changes)
    hreporter.reportBehaviorChanges();
  if(watch_behavior != "")
    hreporter.setWatchBehavior(watch_behavior);

  hreporter.setColorActive(use_colors);
  hreporter.setUseColor(use_colors);
  hreporter.setVarTrunc(var_trunc);

  for(unsigned int k=0; k<watch_vars.size(); k++)
    hreporter.addWatchVar(watch_vars[k]);
  
  bool handled = hreporter.handle(alogfile_in);
  
  if(handled)
    hreporter.printReport();
}











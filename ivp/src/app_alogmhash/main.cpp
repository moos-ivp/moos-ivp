/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: May 12th, 2023                                       */
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
#include "MHashReporter.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main()

int main(int argc, char *argv[])
{
  MHashReporter mhash_reporter;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];

    bool handled = true;
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogmhash", "gpl");
      return(0);
    }
    else if((argi=="-t") || (argi=="--terse")) 
      mhash_reporter.setTerse();
    else if((argi=="-m") || (argi=="--mhash")) 
      mhash_reporter.setReportMHash();
    else if((argi=="-o") || (argi=="--odo")) 
      mhash_reporter.setReportODist();
    else if((argi=="-d") || (argi=="--duration")) 
      mhash_reporter.setReportDuration();
    else if((argi=="-n") || (argi=="--name")) 
      mhash_reporter.setReportName();
    else if((argi=="-u") || (argi=="--utc")) 
      mhash_reporter.setReportUTC();
    else if((argi=="-x") || (argi=="--xhash")) 
      mhash_reporter.setReportXHash();
    else if((argi=="-a") || (argi=="--all")) 
      mhash_reporter.setReportAll();
    //else if(strBegins(argi, "--xhash="))
    //  mhash_reporter.setReportXHash(argi.substr(8));
    else if(strEnds(argi, ".alog"))
      handled = mhash_reporter.setALogFile(argi);
    else if((argi == "-w") || (argi == "--web") || (argi == "-web")) {
      openURLX("https://oceanai.mit.edu/ivpman/apps/alogmhash");
      exit(0);
    }

    if(!handled) {
      cout << "alogmhash Bad Arg: " << argi << endl;
      exit(1);
    }
  }
    
  bool ok = mhash_reporter.handle();
  if(!ok)
    return(1);

  return(0);
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogmhash file.alog [OPTIONS]                    " << endl;
  cout << "                                                   " << endl;
  cout << "Synopsis:                                          " << endl;
  cout << "  alogmhash will read the one given alog file and  " << endl;
  cout << "  parse for key values used in a .mhash cache file." << endl;
  cout << "  Value Examples:                                  " << endl;
  cout << "    MHASH:    230513-1453B-ICED-OWEN               " << endl;
  cout << "    ODO:      423.2                                " << endl;
  cout << "    DURATION: 874.3                                " << endl;
  cout << "    NAME:     abe                                  " << endl;
  cout << "    UTC:      33678848885.977                      " << endl;
  cout << "  If multiple MISSION_HASH values are present in   " << endl;
  cout << "  the given log file, the posting value with the   " << endl;
  cout << "  highest odemetry distance is the one chosen. This" << endl;
  cout << "  guards against the situation where a shoreside   " << endl;
  cout << "  re-starts after the logging has started and thus " << endl;
  cout << "  generates a new mission hash. Rule of thumb is to" << endl;
  cout << "  regard the MISSION_HASH that prevailed for the   " << endl;
  cout << "  highest odometry as the one true MISSION_HASH.   " << endl;
  cout << "                                                   " << endl;
  cout << "Options:                                           " << endl;
  cout << "  -h,--help       Displays this help message       " << endl;
  cout << "  -v,--version    Display current release version  " << endl;
  cout << "  -t,--terse      terse output (No newline/CRLF)   " << endl;
  cout << "                                                   " << endl;
  cout << "  -m,--mhash      Report MHASH w/ longest odometry " << endl;
  cout << "  -o,--odo        Report total odometry            " << endl;
  cout << "  -d,--duration   Report total duration            " << endl;
  cout << "  -n,--name       Report node/vehicle name         " << endl;
  cout << "  -u,--utc        Report UTC start time            " << endl;
  cout << "  -a,--all        Report mhash,odo,dur,name        " << endl;
  cout << "                                                   " << endl;
  cout << "  --web,-w                                         " << endl;
  cout << "    Open browser to:                               " << endl;
  cout << "    https://oceanai.mit.edu/ivpman/apps/alogmhash  " << endl;
  cout << "                                                   " << endl;
  cout << "Returns:                                           " << endl;
  cout << "  0 if alog file ok.                               " << endl;
  cout << "  1 if alog file not ok, unable to open.           " << endl;
  cout << "                                                   " << endl;
  cout << "Further Notes:                                     " << endl;
  cout << "  (1) The order of arguments is irrelevent.        " << endl;
  cout << "  (2) Only last given .alog file is reported on.   " << endl;
  cout << "  (3) Intended to be used in mhash_tag.sh script.  " << endl;
  cout << "                                                   " << endl;
  cout << "(1) Example:                                       " << endl;
  cout << "  $ alogmhash *.alog --all                         " << endl;
  cout << "    mhash=230512-2147I-ICED-OWEN,odo=1507.5,     \\" << endl;
  cout << "    duration=451.3,name=alpha,utc=33678848885.977  " << endl;
  cout << "(2) Example:                                       " << endl;
  cout << "  $ alogmhash *.alog --mhash                       " << endl;
  cout << "    mhash=230512-2147I-ICED-OWEN                   " << endl;
  exit(0);
}


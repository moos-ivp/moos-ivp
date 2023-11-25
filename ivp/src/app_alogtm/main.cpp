/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 5th, 2020                                        */
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
#include "TGrepHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  TGrepHandler handler;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help")) {
      cout << "Usage: " << endl;
      cout << "  alogtm in.alog [VAR] [OPTIONS]                               " << endl;
      cout << "                                                               " << endl;
      cout << "Synopsis:                                                      " << endl;
      cout << "  Scan the given alog file for the specified args, and         " << endl;
      cout << "  report their times base on a time mark. The time mark is     " << endl;
      cout << "  given by the --tm=<time_mark>                                " << endl;
      cout << "                                                               " << endl;
      cout << "Standard Arguments:                                            " << endl;
      cout << "  in.alog  - The input logfile.                                " << endl;
      cout << "  VAR      - The name of a MOOS variable                       " << endl;
      cout << "                                                               " << endl;
      cout << "Options:                                                       " << endl;
      cout << "  -h,--help         Displays this help message                 " << endl;
      cout << "  -v,--version      Displays the current release version       " << endl;
      cout << "  --verbose         Verbose mode                               " << endl;
      cout << "                                                               " << endl;
      cout << "  --mg=<gap>        Min time gap between like reports          " << endl;
      cout << "  --tm=<mar>        Specify the time mark                      " << endl;
      cout << "  --tdelta=<val>    Specify max_tdelta                         " << endl;
      cout << "  --vdelta=<val>    Specify max_vdelta                         " << endl;
      cout << "  --tf              Output in test format                      " << endl;
      cout << "                                                               " << endl;
      cout << "Examples:                                                      " << endl;
      cout << "$ alogtm --tm=DEPLOY=true *.alog NAV_X --mg=40 --tf --vdelta=8 " << endl;
      cout << "$ alogtm --tm=DEPLOY=true *.alog MISSION_RESULT --tf --tdelta=8" << endl;
      cout << endl;
      return(0);
    }
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogtm", "gpl");
      return(0);
    }

    bool handled = true;
    if(strEnds(argi, ".alog"))
      handled = handler.setLogFile(argi);
    else if(argi == "--tf")
      handler.setTestFormat();
    else if(argi == "--verbose")
      handler.setVerbose();
    else if(strBegins(argi, "--mg="))
      handled = handler.setMinGap(argi.substr(5));
    else if(strBegins(argi, "--tm="))
      handled = handler.setTimeMark(argi.substr(5));
    else if(strBegins(argi, "--tdelta="))
      handled = handler.setMaxTDelta(argi.substr(9));
    else if(strBegins(argi, "--vdelta="))
      handled = handler.setMaxVDelta(argi.substr(9));
    else if(strBegins(argi, "-d"))
      handled = handler.setSigDigits(argi.substr(2));
    else 
      handled = handler.addKey(argi);
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      return(1);
    }
  }

  if(!handler.okALogFile()) {
    cout << "A valid alog file must be given. Exiting. " << endl;
    return(2);
  }
    
  bool handled = handler.handle();
  if(!handled)
    return(2);

  return(0);
}












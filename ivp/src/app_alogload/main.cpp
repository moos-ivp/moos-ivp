/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: December 3rd, 2019                                   */
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
#include "LoadReporter.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  LoadReporter load_reporter;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    bool   handled = true;
 
    if((argi == "-v") || (argi == "--version") || (argi =="-version"))
      showReleaseInfoAndExit("alogload", "gpl");
    else if((argi == "-h") || (argi == "--help") || (argi =="-help"))
      showHelpAndExit();
    else if((argi == "--verbose") || (argi == "-v") || (argi == "-verbose"))
      load_reporter.setVerbose();
    else if((argi == "--terse") || (argi == "-t") || (argi == "-terse"))
      load_reporter.setTerse();
    else if((argi == "--near") || (argi == "-n") || (argi == "-near"))
      load_reporter.setNearMode();
    else if(strEnds(argi, ".alog")) 
      handled = load_reporter.addALogFile(argi);
    else
      handled = false;

    if(!handled) {
      cout << "alogload: Bad Arg: " << argi << endl;
      return(1);
    }      
  }
  
  load_reporter.report();

  return(0);
}



//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogload one.alog two.alog [OPTIONS]                    " << endl;
  cout << "                                                          " << endl;
  cout << "Synopsis:                                                 " << endl;
  cout << "  Scan the given alog files and generate one of several   " << endl;
  cout << "  possible report summaries from data produced by         " << endl;
  cout << "  uLoadWatch.                                             " << endl;
  cout << "                                                          " << endl;
  cout << "Standard Arguments:                                       " << endl;
  cout << "  one.alog  - An input alog file                          " << endl;
  cout << "  two.alog  - Another alog file. Others may be added.     " << endl;
  cout << "                                                          " << endl;
  cout << "Options:                                                  " << endl;
  cout << "  -h,--help        Display this usage/help message.       " << endl;
  cout << "  -v,--version     Display version information.           " << endl;
  cout << "  -t,--terse       Terse output, e.g., just total breaches" << endl;
  cout << "  -n,--near        Count NEAR breaches, not breaches      " << endl;
  cout << "  --verbose        Produce verbose output.                " << endl;
  cout << "                                                          " << endl;
  cout << "Returns:                                                  " << endl;
  cout << "  0 if all OK                                             " << endl;
  cout << "  1 if unhandled command line arg, e.g., unreadable file. " << endl;
  cout << "                                                          " << endl;
  cout << "Further Notes:                                            " << endl;
  cout << "  (1) Order of all command line arguments do not matter.  " << endl;
  cout << endl;
  exit(0);
}


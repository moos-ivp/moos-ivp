/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 5th, 2008                                       */
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
#include <iostream>
#include "cmath"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "TermUtils.h"
#include "ALogClipHandler.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

void display_usage();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  ALogClipHandler handler;

  //===================================================================
  // Pass #1: Check for version/help/batch args
  //===================================================================
  vector<string> pass_two_args;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    bool   handled = true;

    if((argi == "-v") || (argi == "--version") || (argi =="-version")) {
      showReleaseInfo("alogclip", "gpl");
      return(0);
    }
    else if((argi == "-h") || (argi == "--help") || (argi =="-help")) {
      display_usage();
      return(0);
    }
    else if((argi == "-b") || (argi == "--batch") || (argi =="-batch")) 
      handler.setBatch();
    else if(strBegins(argi, "--suffix=")) 
      handled = handler.setSuffix(argi.substr(9));
    else
      pass_two_args.push_back(argi);

    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      exit(1);
    }
  }

  //===================================================================
  // Pass #2: Handle all other args
  //===================================================================

  for(unsigned int i=0; i<pass_two_args.size(); i++) {
    string argi = pass_two_args[i];
    bool   handled = true;

    if(strEnds(argi, ".alog")) 
      handled = handler.addALogFile(argi);
    else if((argi == "--verbose") || (argi == "-v") || (argi == "-verbose"))
      handler.setVerbose();
    else if((argi == "--force") || (argi == "-f") || (argi == "-force"))
      handler.setForceOverwrite();
    else if(isNumber(argi)) 
      handled = handler.setTimeStamp(atof(argi.c_str()));
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      exit(1);
    }
  }
  
  // Now execute the job and note the result.
  bool ok = handler.process();
  if(!ok) 
    return(1);
  return(0);
}


//--------------------------------------------------------
// Procedure: exit_with_usage

void display_usage()
{
  cout << "Usage: " << endl;
  cout << "  alogclip in.alog mintime maxtime [out.alog] [OPTIONS]  " << endl;
  cout << "                                                         " << endl;
  cout << "Synopsis:                                                " << endl;
  cout << "  Create a new MOOS .alog file from a given .alog file   " << endl;
  cout << "  by removing entries outside a given time window.       " << endl;
  cout << "                                                         " << endl;
  cout << "Standard Arguments:                                      " << endl;
  cout << "  in.alog  - The input logfile.                          " << endl;
  cout << "  mintime  - Log entries with timestamps below mintime   " << endl;
  cout << "             will be excluded from the output file.      " << endl;
  cout << "  maxtime  - Log entries with timestamps above mintime   " << endl;
  cout << "             will be excluded from the output file.      " << endl;
  cout << "  out.alog - The newly generated output logfile. If no   " << endl;
  cout << "             file provided, output goes to stdout.       " << endl;
  cout << "                                                         " << endl;
  cout << "Options:                                                 " << endl;
  cout << "  -h,--help     Display this usage/help message.         " << endl;
  cout << "  -v,--version  Display version information.             " << endl;
  cout << "  -f,--force    Overwrite an existing output file.       " << endl;
  cout << "  -q,--quiet    Verbose report suppressed at conclusion. " << endl;
  cout << "  -b,--batch    Batch clip all given alog files.         " << endl;
  cout << "  --suffix=N    Batch clipped file in.alog to in_N.alog. " << endl;
  cout << "                The default suffix is \"_clipped\".      " << endl;
  cout << "                                                         " << endl;
  cout << "Further Notes:                                           " << endl;
  cout << "  (1) The order of arguments may vary. The first alog    " << endl;
  cout << "      file is treated as the input file, and the first   " << endl;
  cout << "      numerical value is treated as the mintime.         " << endl;
  cout << "  (2) Two numerical values, in order, must be given.     " << endl;
  cout << "  (3) Use the --batch option to clip a group of files.   " << endl;
  cout << "  (4) See also: alogscan, alogrm, aloggrep, alogview     " << endl;
  cout << endl;
}






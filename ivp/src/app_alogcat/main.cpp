/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: August 13th, 2018                                    */
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
#include "MBUtils.h"
#include "OpenURL.h"
#include "ReleaseInfo.h"
#include "ALogCatHandler.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  ALogCatHandler handler;

  //===================================================================
  // Pass #1: Check for version/help/batch args
  //===================================================================
  vector<string> pass_two_args;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    bool   handled = true;

    if((argi == "-v") || (argi == "--version") || (argi =="-version"))
      showReleaseInfoAndExit("alogcat", "gpl");
    else if((argi == "-h") || (argi == "--help") || (argi =="-help"))
      showHelpAndExit();

    else if(strBegins(argi, "--new="))
      handled = handler.setNewALogFile(argi.substr(6));
    else if(strEnds(argi, ".alog")) 
      handled = handler.addALogFile(argi);
    else if((argi == "--verbose") || (argi == "-v") || (argi == "-verbose"))
      handler.setVerbose();
    else if((argi == "--force") || (argi == "-f") || (argi == "-force"))
      handler.setForceOverwrite();
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/alogcat");
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      exit(1);
    }
  }

  if(handler.size() < 2) {
    showHelpAndExit();
  }
  
  // Now execute the job and note the result.
  bool ok = handler.process();
  if(!ok) 
    return(1);
  return(0);
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogcat one.alog two.alog --new=three.alog [OPTIONS]    " << endl;
  cout << "                                                          " << endl;
  cout << "Synopsis:                                                 " << endl;
  cout << "  Create a new MOOS .alog file from two or more given     " << endl;
  cout << "  .alog files. Given files are meant to be non-overlapping" << endl;
  cout << "  in time, presumably made by starting and stopping a     " << endl;
  cout << "  single MOOS community.                                  " << endl;
  cout << "                                                          " << endl;
  cout << "Standard Arguments:                                       " << endl;
  cout << "  one.alog  - An input alog file                          " << endl;
  cout << "  two.alog  - Another alog file. Others may be added.     " << endl;
  cout << "                                                          " << endl;
  cout << "Options:                                                  " << endl;
  cout << "  -h,--help        Display this usage/help message.       " << endl;
  cout << "  -v,--version     Display version information.           " << endl;
  cout << "  -f,--force       Overwrite an existing output file.     " << endl;
  cout << "  --verbose        Produce verbose output.                " << endl;
  cout << "  --verbose(again) Produce verbose and debugging output.  " << endl;
  cout << "  --new=file.alog  Name of new concatenated alog file.    " << endl;
  cout << "                                                          " << endl;
  cout << "  --web,-w   Open browser to:                             " << endl;
  cout << "             https://oceanai.mit.edu/ivpman/apps/alogcat  " << endl;
  cout << "                                                          " << endl;
  cout << "Returns:                                                  " << endl;
  cout << "  0 if a new concatenated file created successfuly.       " << endl;
  cout << "  1 otherwise                                             " << endl;
  cout << "                                                          " << endl;
  cout << "Further Notes:                                            " << endl;
  cout << "  (1) Order of all command line arguments do not matter.  " << endl;
  cout << "  (2) All alog files are scanned to ensure they are not   " << endl;
  cout << "      overlapping. Their proper order is worked out       " << endl;
  cout << "      automatically.                                      " << endl;
  cout << endl;
  exit(0);
}







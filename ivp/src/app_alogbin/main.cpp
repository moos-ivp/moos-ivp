/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Aug 26th, 2020                                       */
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
#include "LogBinHandler.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  LogBinHandler handler;

  bool handled = false;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogbin", "gpl");
      return(0);
    }
    else if(argi=="--verbose")
      handled = handler.setVerbose();
    else if(strEnds(argi, ".log"))
      handled = handler.setLogFile(argi);
    else if(strBegins(argi, "--file="))
      handled = handler.setLogFile(argi.substr(7));
    else if(strBegins(argi, "--delta="))
      handled = handler.setDelta(argi.substr(8));
    else if(strBegins(argi, "--min="))
      handled = handler.setDelta(argi.substr(6));

    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  handler.handle();
  exit(0);
}
  
//------------------------------------------------------------
// Procedure: showHelpAndExit()  

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogbin file.log [OPTIONS]                               " << endl;
  cout << "                                                           " << endl;
  cout << "Synopsis:                                                  " << endl;
  cout << "  Given a file of data, one value per line, bin each value " << endl;
  cout << "  into the given delta and count the number in each bin.   " << endl;
  cout << "  For example: --delta=5 --min=15                          " << endl;
  cout << "      23.4                                                 " << endl;
  cout << "      19.5          17.5  2                                " << endl;
  cout << "      15.8    ==>   22.5  2                                " << endl;
  cout << "      27.6          27.5  1                                " << endl;
  cout << "      21.3                                                 " << endl;
  cout << "                                                           " << endl;
  cout << "Standard Arguments:                                        " << endl;
  cout << "  file.log - The input logfile.                            " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  -h,--help         Displays this help message             " << endl;
  cout << "  -v,--version      Displays the current release version   " << endl;
  cout << "  --delta=<val>     Size of each bin                       " << endl;
  cout << "  --min=<val>       Low value of the lowest bin            " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) Order of arguments is irrelevant.                    " << endl;
  cout << endl;
  exit(0);
}







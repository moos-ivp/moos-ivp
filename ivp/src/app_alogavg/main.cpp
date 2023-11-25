/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 15th, 2021                                       */
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
#include "AvgHandler.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  AvgHandler handler;

  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogavg", "gpl");
      return(0);
    }
    else if(argi=="--verbose")
      handler.setVerbose();
    else if(argi=="--noformat")
      handler.setFormatAligned(false);
    else if((argi=="-np") || (argi=="--negpos")) 
      handler.setFormatNegPos();
    else if(strEnds(argi, ".log"))
      handled = handler.setLogFile(argi);
    else if(strBegins(argi, "--file="))
      handled = handler.setLogFile(argi.substr(7));
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/alogavg");

    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  bool ok = handler.handle();
  if(ok)
    return(0);
  else
    return(1);
}
  
//------------------------------------------------------------
// Procedure: showHelpAndExit()  

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogavg file.log [OPTIONS]                               " << endl;
  cout << "                                                           " << endl;
  cout << "Synopsis:                                                  " << endl;
  cout << "  Given a file of data, two values per line, treat each    " << endl;
  cout << "  line as having an x value and y value. This tool will    " << endl;
  cout << "  calculate the average y value given several (x,y) pairs. " << endl;
  cout << "  It will also calculate the min, max, standard deviation  " << endl;
  cout << "  of for each x value.                                     " << endl;
  cout << "                                                           " << endl;
  cout << "  Example Input Log File:                                  " << endl;
  cout << "     2  289.09                                             " << endl;
  cout << "     1  357.331                                            " << endl;
  cout << "     2  287.762                                            " << endl;
  cout << "     1  358.505                                            " << endl;
  cout << "     2  289.645                                            " << endl;
  cout << "     1  358.952                                            " << endl;
  cout << "                                                           " << endl;
  cout << "  Example Output:                                          " << endl;
  cout << "     1 358.262667 357.331 358.952 0.683596                 " << endl;
  cout << "     2 288.832333 287.762 289.645 0.790028                 " << endl;
  cout << "                                                           " << endl;
  cout << "Standard Arguments:                                        " << endl;
  cout << "  file.log - The input logfile.                            " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  -h,--help         Displays this help message             " << endl;
  cout << "  -v,--version      Displays the current release version   " << endl;
  cout << "  --file=file.txt   Use file.txt as the input log file     " << endl;
  cout << "  --noformat        Do not align column output             " << endl;
  cout << "  -np, --negpos     Use Neg/Pos instead of Min/Max         " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) Order of arguments is irrelevant.                    " << endl;
  cout << "  (2) Files ending in .log suffix are assumed to be input  " << endl;
  cout << "      log file. Specifying --file=file.log can be          " << endl;
  cout << "      specified simply with file.log.                      " << endl;
  cout << "  (3) Only one data file may be provided.                  " << endl;
  cout << "  (4) By default, column output is aligned with padded     " << endl;
  cout << "      cells. With --noformat, one space is used instead    " << endl;
  cout << "      between columns.                                     " << endl;
  cout << "  (5) To save the output to a new file, simply use the     " << endl;
  cout << "      command-line redirect operator.                      " << endl;
  cout << endl;
  exit(0);
}


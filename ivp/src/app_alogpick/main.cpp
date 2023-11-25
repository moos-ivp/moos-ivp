/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Aug 13th, 2023                                       */
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
#include "PickHandler.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  PickHandler handler;

  // Must have at least a file name and one field
  if(argc < 3)
    showHelpAndExit();
  
  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogpick", "gpl");
      return(0);
    }
    else if((argi == "-w") || (argi == "--web") || (argi == "-web")) {
      openURLX("https://oceanai.mit.edu/ivpman/apps/alogpick");
      return(0);
    }

    if(i==1)
      handled = handler.setLogFile(argi);
    else if(argi=="--verbose")
      handler.setVerbose();
    else if(argi=="--noformat")
      handler.setFormatAligned(false);
    else
      handled = handler.addField(argi);
    
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
  cout << "  alogpick file.log [OPTIONS]                              " << endl;
  cout << "                                                           " << endl;
  cout << "Synopsis:                                                  " << endl;
  cout << "  Given a file of data, each line in the format of a comma-" << endl;
  cout << "  separated list of param=value pairs, given one or more   " << endl;
  cout << "  fields as arguments, pick out those elements only. By    " << endl;
  cout << "  default, the parameter value is ommitted.                " << endl;
  cout << "                                                           " << endl;
  cout << "  Example Input Log File (file.txt):                       " << endl;
  cout << "     rate=110769,calcs=14400,loops=40,degs=5,spts=72       " << endl;
  cout << "     rate=130909,calcs=14400,loops=40,degs=8,spts=46,      " << endl;
  cout << "     rate=144000,calcs=14400,loops=40,degs=40,spts=10      " << endl;
  cout << "                                                           " << endl;
  cout << "  $ alogpick file.txt degs rate                            " << endl;
  cout << "  Example Output:                                          " << endl;
  cout << "     5 110769                                              " << endl;
  cout << "     8 130909                                              " << endl;
  cout << "     40 144000                                             " << endl;
  cout << "                                                           " << endl;
  cout << "Standard Arguments:                                        " << endl;
  cout << "  file.txt - The input data file.                          " << endl;
  cout << "  file.log - The input data file.                          " << endl;
  cout << "                                                           " << endl;
  cout << "Options:                                                   " << endl;
  cout << "  -h,--help         Displays this help message             " << endl;
  cout << "  -v,--version      Displays the current release version   " << endl;
  cout << "  -w,--web          Open Web browser to documentation.     " << endl;
  cout << "  --noformat        Do not align column output             " << endl;
  cout << "                                                           " << endl;
  cout << "Further Notes:                                             " << endl;
  cout << "  (1) Order of arguments is indeed relevant. First arg     " << endl;
  cout << "      is always the input file.                            " << endl;
  cout << "  (2) Files ending in .log or .txt suffix are assumed to   " << endl;
  cout << "      be input file. Specifying --file=file.log can be     " << endl;
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


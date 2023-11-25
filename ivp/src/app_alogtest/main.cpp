/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: April 19, 2019                                       */
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

#include <iostream>
#include <string>
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "LogTester.h"

using namespace std;

void showHelpAndExit();

int main(int argc, char** argv) 
{
  LogTester tester;
  
  bool handled = false;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogtest", "gpl");
      return(0);
    }
    if((argi=="-f") || (argi == "--force") || (argi=="-force"))
      tester.setOverWrite();
    if((argi == "--verbose") || (argi=="-verbose"))
      tester.setVerbose();
    else if(strEnds(argi, ".alog"))
      handled = tester.setALogFile(argi);
    else if(strBegins(argi, "--tfile="))
      handled = tester.addTestFile(argi.substr(8));
    else if(strBegins(argi, "--ofile="))
      handled = tester.setMarkFile(argi.substr(8));

    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  bool result = tester.test();
  return(result);
}


//------------------------------------------------------------  
// Procedure: showHelpAndExit()                                               

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogtest file.alog [OPTIONS]                      " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  Scan an alog file, apply test criteria and return " << endl;
  cout << "  a pass/fail result. Used for automated mission    " << endl;
  cout << "  testing.                                           " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  -h,--help       Displays this help message        " << endl;
  cout << "  -v,--version    Display current release version   " << endl;
  cout << "                                                    " << endl;
  cout << "Returns:                                            " << endl;
  cout << "  0 if the test passes.                             " << endl;
  cout << "  1 if the test does not pass.                      " << endl;
  cout << "                                                    " << endl;
  cout << "Further Notes:                                      " << endl;
  cout << "  (1) The order of arguments is irrelevent.         " << endl;
  cout << "  (2) Only last specified .alog file is reported on." << endl;
  cout << endl;
  exit(0);
}









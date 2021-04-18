/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 6th, 2020                                        */
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
#include "ALogEvaluator.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  ALogEvaluator evaluator;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    cout << "argi: " << argi << endl;
    if((argi=="-h") || (argi == "--help") || (argi=="-help")) {
      cout << "Usage: " << endl;
      cout << "  alogeval in.alog criteria.txt [OPTIONS]                      " << endl;
      cout << "                                                               " << endl;
      cout << "Synopsis:                                                      " << endl;
      cout << "  Scan the given alog file and apply the test criteria found   " << endl;
      cout << "  in criteria.txt. The criteria should be in the same format   " << endl;
      cout << "  as would be provided in pMissionEval.                        " << endl;
      cout << "                                                               " << endl;
      cout << "Standard Arguments:                                            " << endl;
      cout << "  in.alog       - The input logfile.                           " << endl;
      cout << "  criteria.txt  - The test criteria file                       " << endl;
      cout << "                                                               " << endl;
      cout << "Options:                                                       " << endl;
      cout << "  -h,--help         Displays this help message                 " << endl;
      cout << "  -v,--version      Displays the current release version       " << endl;
      cout << "  --verbose         Verbose mode                               " << endl;
      cout << "                                                               " << endl;
      cout << "Examples:                                                      " << endl;
      cout << "$ alogeval  " << endl;
      cout << "$ alogeval  " << endl;
      cout << endl;
      return(0);
    }
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogeval", "gpl");
      return(0);
    }

    bool handled = true;
    if(strEnds(argi, ".alog"))
      handled = evaluator.setALogFile(argi);
    else if(strEnds(argi, ".txt"))
      handled = evaluator.setTestFile(argi);
    else if(argi == "--verbose")
      evaluator.setVerbose();
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      return(1);
    }
  }

  if(!evaluator.okALogFile()) {
    cout << "A valid alog file must be given. Exiting. " << endl;
    return(2);
  }    
  if(!evaluator.okALogFile()) {
    cout << "A valid criteria file must be given. Exiting. " << endl;
    return(3);
  }
    
  bool handled = evaluator.handle();
  if(!handled)
    return(4);
  evaluator.print();

  
  return(0);
}

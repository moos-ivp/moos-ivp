/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 3rd, 2008                                       */
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
#include "PareEngine.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  string in_alog, out_alog;
  double pare_window = 30;

  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogpare", "gpl");
      return(0);
    }
    else if(strEnds(argi, ".alog") && (in_alog == ""))
      in_alog = argi;
    else if(strEnds(argi, ".alog") && (out_alog == ""))
      out_alog = argi;
    else if(strBegins(argi, "--pare_window=")) {
      string str = argi.substr(14);
      handled = setNonNegDoubleOnString(pare_window, str);
    }
    else 
      handled = false;
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  PareEngine pare_engine;
  bool ok = pare_engine.setALogFileIn(in_alog);
  ok = ok && pare_engine.setALogFileOut(out_alog);
  ok = ok && pare_engine.addMarkListVar("ENCOUNTER");

  pare_engine.defaultHitList();
  pare_engine.defaultPareList();
  pare_engine.setPareWindow(pare_window);
  pare_engine.pareFile();
  
  if(ok)
    pare_engine.printReport();
  return(0);
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogpare .alog [out.alog] [OPTIONS]                  " << endl;
  cout << "                                                       " << endl;
  cout << "Synopsis:                                              " << endl;
  cout << "  Scan an alog file for certain events. Pare back the  " << endl;
  cout << "  the file by removing entries not within the given    " << endl;
  cout << "  time window of any detected event.                   " << endl;
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  -h,--help       Displays this help message           " << endl;
  cout << "  -v,--version    Display current release version      " << endl;
  cout << "                                                       " << endl;
  cout << "Further Notes:                                         " << endl;
  cout << "  (1) The order of arguments is irrelevent.            " << endl;
  cout << "  (2) Only first specified .alog file is reported on.  " << endl;
  cout << endl;
  exit(0);
}


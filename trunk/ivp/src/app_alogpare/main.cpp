/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 25th, 2015                                       */
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

  PareEngine pare_engine;
  
  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogpare", "gpl");
      return(0);
    }
    else if(strEnds(argi, ".alog") && (in_alog == "")) {
      handled = pare_engine.setALogFileIn(argi);
      in_alog = argi;
    }
    else if(strEnds(argi, ".alog") && (out_alog == "")) {
      handled = pare_engine.setALogFileOut(argi);
      out_alog = argi;
    }
    else if(argi == "--verbose")
      pare_engine.setVerbose(true);
    else if(strBegins(argi, "--pare_window=")) {
      string str = argi.substr(14);
      handled = setNonNegDoubleOnString(pare_window, str);
    }
    else if(strBegins(argi, "--markvars=")) 
      handled = pare_engine.addMarkListVars(argi.substr(11));
    else if(strBegins(argi, "--hitvars=")) 
      handled = pare_engine.addHitListVars(argi.substr(10));
    else if(strBegins(argi, "--parevars=")) 
      handled = pare_engine.addPareListVars(argi.substr(11));
    else if(strBegins(argi, "--test")) {
      handled = pare_engine.addMarkListVars("ENCOUNTER");
      handled = pare_engine.addHitListVars("*ITER_GAP,*ITER_LEN,DB_QOS");
      handled = pare_engine.addHitListVars("NODE_REPORT*,PSHARE*");
      handled = pare_engine.addPareListVars("BHV_IPF,VIEW_SEGLIST");
    }
    else 
      handled = false;
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  pare_engine.defaultHitList();
  pare_engine.defaultPareList();
  pare_engine.setPareWindow(pare_window);
  pare_engine.pareTheFile();
  pare_engine.printReport();
  return(0);
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogpare .alog [out.alog] [OPTIONS]                    " << endl;
  cout << "                                                         " << endl;
  cout << "Synopsis:                                                " << endl;
  cout << "  Pare back the given alog file in a two-pass manner.    " << endl;
  cout << "  First pass detects events defined by given mark vars.  " << endl;
  cout << "  The second pass removes lines with vars on the pare    " << endl;
  cout << "  list if they are not within pare_window seconds of     " << endl;
  cout << "  an event line. It also removes lines with vars on the  " << endl;
  cout << "  hitlist unconditionally. Latter could also be done     " << endl;
  cout << "  with alogrm.                                           " << endl;
  cout << "  The original alog file is not altered.                 " << endl;
  cout << "                                                         " << endl;
  cout << "Options:                                                 " << endl;
  cout << "  -h,--help         Displays this help message           " << endl;
  cout << "  -v,--version      Display current release version      " << endl;
  cout << "  --verbose         Enable verbose output                " << endl;
  cout << "  --markvars=<L>    Comma-separated list of mark vars    " << endl;
  cout << "  --hitvars=<L>     Comma-separated list of hit vars     " << endl;
  cout << "  --parevars=<L>    Comma-separated list of pare vars    " << endl;
  cout << "  --pare_window=<N> Set window to N seconds (default 30) " << endl;
  cout << "                                                         " << endl;
  cout << "Examples:                                                " << endl;
  cout << "  alogpare --markvars=ENCOUNTER --parevars=BHV_IPF       " << endl;
  cout << "           original.alog smaller.alog                    " << endl;
  cout << "  alogpare --markvars=ENCOUNTER                          " << endl;
  cout << "           --parevars=BHV_IPF,VIEW_*                     " << endl;
  cout << "           --hitvars=*ITER_GAP,*ITER_LEN,DB_QOS          " << endl;
  cout << "           --pare_window=10                              " << endl;
  cout << "           original.alog smaller.alog                    " << endl;
  cout << "                                                         " << endl;
  cout << "Further Notes:                                           " << endl;
  cout << "  (1) The order of alogfile args IS significant.         " << endl;
  cout << "  (2) The order of non alogfile args is not significant. " << endl;
  cout << endl;
  exit(0);
}






/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: January 16th, 2016                                   */
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
#include "EPlotEngine.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  string in_alog, out_file;

  EPlotEngine eplot_engine;
  
  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogeplot", "gpl");
      return(0);
    }
    else if(strEnds(argi, ".alog") && (in_alog == "")) {
      handled = eplot_engine.setALogFileIn(argi);
      in_alog = argi;
    }
    else if(argi == "--verbose")
      eplot_engine.setVerbose(true);
    else 
      handled = false;
    
    if(!handled) {
      cout << "Unhandled command line argument: " << argi << endl;
      cout << "Use --help for usage. Exiting.   " << endl;
      exit(1);
    }
  }

  eplot_engine.generate();
  return(0);
}


//------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage: " << endl;
  cout << "  alogeplot .alog [out.alog] [OPTIONS]                  " << endl;
  cout << "                                                       " << endl;
  cout << "Synopsis:                                              " << endl;
  cout << "  The original alog file is not altered.               " << endl;
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  -h,--help         Displays this help message         " << endl;
  cout << "  -v,--version      Display current release version    " << endl;
  cout << "  --verbose         Enable verbose output              " << endl;
  cout << "                                                       " << endl;
  cout << "Examples:                                              " << endl;
  cout << "  alogeplot original.alog                              " << endl;
  cout << "                                                       " << endl;
  cout << endl;
  exit(0);
}


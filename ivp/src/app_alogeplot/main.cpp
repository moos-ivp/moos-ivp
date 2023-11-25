/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  EPlotEngine eplot_engine;
  
  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogeplot", "gpl");
      return(0);
    }
    else if(strEnds(argi, ".alog")) 
      handled = eplot_engine.addALogFile(argi);
    else if(strEnds(argi, ".scn")) 
      handled = eplot_engine.setSceneFile(argi);
    else if(strBegins(argi, "--pcolor="))
      handled = eplot_engine.setPointColor(argi.substr(9));
    else if(strBegins(argi, "--psize="))
      handled = eplot_engine.setPointSize(argi.substr(8));

    else if(strBegins(argi, "--pwid=")) 
      handled = eplot_engine.setPlotWidCM(argi.substr(7));
    else if(strBegins(argi, "--phgt=")) 
      handled = eplot_engine.setPlotHgtCM(argi.substr(7));

    else if(strBegins(argi, "--gwid=")) 
      handled = eplot_engine.setGridWidCM(argi.substr(7));
    else if(strBegins(argi, "--ghgt=")) 
      handled = eplot_engine.setGridHgtCM(argi.substr(7));

    else if((argi == "--verbose") || (argi == "-v"))
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
  cout << "  alogeplot file.alog [file.scn] [file.png] [OPTIONS] " << endl;
  cout << "                                                      " << endl;
  cout << "Synopsis:                                             " << endl;
  cout << "  The original alog file is not altered.              " << endl;
  cout << "                                                      " << endl;
  cout << "Options:                                              " << endl;
  cout << "  --help,-h                                           " << endl;
  cout << "    Displays this help message.                        " << endl;
  cout << "  --version                                           " << endl;
  cout << "    Display the current release version.              " << endl;
  cout << "  --verbose,-v                                        " << endl;
  cout << "    Enable verbose outputs.                           " << endl;
  cout << "                                                      " << endl;
  cout << "  --pwid=N                                            " << endl;
  cout << "    Set plot width to N centimeters. Default is 30cm. " << endl;
  cout << "  --phgt=N                                            " << endl;
  cout << "    Set plot height to N centimeters. Default is 20cm." << endl;
  cout << "  --gwid=N                                            " << endl;
  cout << "    Set grid width to N centimeters. Default is 30cm. " << endl;
  cout << "  --ghgt=N                                            " << endl;
  cout << "    Set grid height to N centimeters. Default is 20cm." << endl;
  cout << "                                                      " << endl;
  cout << "  --pcolor=COLOR                                      " << endl;
  cout << "    Set the point color. Default is darkblue.         " << endl;
  cout << "  --psize=SIZE                                        " << endl;
  cout << "    Set the point size. Default is 0.1 centimeters.   " << endl;
  cout << "                                                      " << endl;
  cout << "Examples:                                             " << endl;
  cout << "  $ alogeplot original.alog                           " << endl;
  cout << "  $ alogeplot original.alog file.scn --pcolor=green   " << endl;
  cout << "  $ alogeplot original.alog file.scn --psize=0.03     " << endl;
  cout << "  $ alogeplot original.alog --pwid=60 --phgt=40       " << endl;
  cout << "                                                      " << endl;
  cout << "Notes:                                                " << endl;
  cout << "  Looks only for following two variables from alog:   " << endl;
  cout << "     COLLISION_DETECT_PARAMS and ENCOUNTER_SUMMARY    " << endl;
  cout << "  So normally this tool is run on an alog file that   " << endl;
  cout << "     has been reduced to these two variables.         " << endl;
  cout << endl;
  exit(0);
}







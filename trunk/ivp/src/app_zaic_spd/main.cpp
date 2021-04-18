/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 2nd, 2015                                       */
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
#include <cstdlib>
#include "ZAIC_SPD_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

void idleProc(void *);

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  Fl::add_idle(idleProc);
  ZAIC_SPD_GUI* gui = new ZAIC_SPD_GUI(700, 460, "ZAIC_SPD-Viewer");

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi == "--version") || (argi=="-version"))
      showReleaseInfoAndExit("zaic_spd", "gpl");

    else if(strBegins(argi, "--verbose")) 
      gui->setVerbose(true);
    else if(strBegins(argi, "--domain=")) 
      gui->setParam("domain", argi.substr(9));
    else if(strBegins(argi, "--medspd=")) 
      gui->setParam("medspd", argi.substr(9));
    else if(strBegins(argi, "--lowspd=")) 
      gui->setParam("lowspd", argi.substr(9));
    else if(strBegins(argi, "--hghspd=")) 
      gui->setParam("hghspd", argi.substr(9));
    else if(strBegins(argi, "--lowspd_util=")) 
      gui->setParam("lowspd_util", argi.substr(14));
    else if(strBegins(argi, "--hghspd_util=")) 
      gui->setParam("hghspd_util", argi.substr(14));
    else {
      cout << "Exiting due to Unhandled arg: " << argi << endl;
      exit(1);
    }      
  }
      
  gui->updateOutput();

  // Enter the GUI event loop.
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << endl;
  cout << "Usage: zaic_spd [OPTIONS]                           " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h           Display this help message    " << endl;
  cout << "  --domain=410         Set upper value of domain    " << endl;
  cout << "  --verbose,           Enable verbose output        " << endl;
  cout << "  --version, -v,       Display the release version  " << endl;
  cout << "                                                    " << endl;
  cout << "  --medspd=200                                      " << endl;
  cout << "  --hghspd=220                                      " << endl;
  cout << "  --lowspd=180                                      " << endl;
  cout << "  --lowspd_util=120                                 " << endl;
  cout << "  --hghspd_util=230                                 " << endl;
  cout << "                                                    " << endl;
  cout << "Example:                                            " << endl;
  cout << " $ zaic_spd --domain=500 --verbose                  " << endl;
  cout << " $ zaic_spd --domain=100 --medspd=120 --lowspd=110 --hghspd=150 " << endl;
  exit(0);
}





/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
    else if(strBegins(argi, "--med_spd=")) 
      gui->setParam("med_spd", argi.substr(10));
    else if(strBegins(argi, "--low_spd=")) 
      gui->setParam("low_spd", argi.substr(10));
    else if(strBegins(argi, "--hgh_spd=")) 
      gui->setParam("hgh_spd", argi.substr(10));
    else if(strBegins(argi, "--low_spd_util=")) 
      gui->setParam("low_spd_util", argi.substr(15));
    else if(strBegins(argi, "--hgh_spd_util=")) 
      gui->setParam("hgh_spd_util", argi.substr(15));
    else if(strBegins(argi, "--max_spd_util=")) 
      gui->setParam("max_spd_util", argi.substr(15));
    else if(strBegins(argi, "--min_spd_util=")) 
      gui->setParam("min_spd_util", argi.substr(15));
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
  cout << "Usage: zaic_spd [OPTIONS]                            " << endl;
  cout << "Options:                                             " << endl;
  cout << "  --help, -h             Display this help message   " << endl;
  cout << "  --domain=speed,0,4,41  Set the IvP domain domain   " << endl;
  cout << "  --verbose,             Enable verbose output       " << endl;
  cout << "  --version, -v,         Display the release version " << endl;
  cout << "                                                     " << endl;
  cout << "  --med_spd=200                                      " << endl;
  cout << "  --hghspd=220                                       " << endl;
  cout << "  --lowspd=180                                       " << endl;
  cout << "  --lowspd_util=120                                  " << endl;
  cout << "  --hghspd_util=230                                  " << endl;
  cout << "                                                     " << endl;
  cout << "Example:                                             " << endl;
  cout << " $ zaic_spd --domain=speed,0,5,51 --verbose          " << endl;
  cout << " $ zaic_spd --domain=speed,0,8,41 --med_spd=4.5 --spd_max_util=80 " << endl;
  exit(0);
}






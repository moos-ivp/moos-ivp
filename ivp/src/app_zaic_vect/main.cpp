/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 16th, 2016                                       */
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
#include "PopulatorVZAIC.h"
#include "ZAIC_VECT_GUI.h"
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
  bool verbose = false;
  ZAIC_Vector *zaic = 0;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi == "--version"))
      showReleaseInfoAndExit("zaic_vect", "gpl");
    else if(strEnds(argi, ".zaic")) {
      PopulatorVZAIC populator;
      populator.readFile(argi);
      zaic = populator.buildZAIC();
      if(!zaic) {
	cout << "Unable to find or build ZAIC from file[" << argi << endl;
	exit(1);
      }
    }
    else if(argi == "--verbose")
      verbose = true;
    else {
      cout << "Exiting due to Unhandled arg: " << argi << endl;
      exit(1);
    }      
  }

  if(!zaic) {
    cout << "No ZAIC_Vector file provided. Exiting now." << endl;
    exit(1);
  }
      
  Fl::add_idle(idleProc);
  ZAIC_VECT_GUI* gui = new ZAIC_VECT_GUI(700, 460, "ZAIC_VECT-Viewer");

  gui->setZAIC(zaic);
  gui->setVerbose(verbose);

  // Enter the GUI event loop.
  return Fl::run();
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << endl;
  cout << "Usage: zaic_vect [OPTIONS]                          " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h           Display this help message    " << endl;
  cout << "  --domain=360         Set upper value of domain    " << endl;
  cout << "  --verbose,           Enable verbose output        " << endl;
  cout << "  --version, -v,       Display the release version  " << endl;
  cout << "                                                    " << endl;
  cout << "Example:                                            " << endl;
  cout << " $ zaic_vect --domain=500 --verbose                 " << endl;
  exit(0);
}

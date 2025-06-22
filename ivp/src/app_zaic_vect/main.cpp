/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
// Procedure: idleProc()

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main()

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

  IvPFunction *ipf = zaic->extractIvPFunction();
  ipf->getPDMap()->print();

  // Enter the GUI event loop.
  return Fl::run();
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << endl;
  cout << "Usage: zaic_vect file.zaic [OPTIONS]                 " << endl;
  cout << "  zaic_vect [OPTIONS                                 " << endl;
  cout << "                                                     " << endl;
  cout << "Synopsis:                                            " << endl;
  cout << "  The zaic_vect utility renders a configured instance" << endl;
  cout << "  of the ZAIC_VECT utility in the ivpbuild toolbox.  " << endl;
  cout << "  This allow the user to visualize the effects of    " << endl;
  cout << "  parameter settings for this too. Normally this     " << endl;
  cout << "  tool is created and parameterized within an IvP    " << endl;
  cout << "  behavior. This tool may give behavior authors some " << endl;
  cout << "  insight into how the tool works. This tool was     " << endl;
  cout << "  also used as an engineering/validation tool when   " << endl;
  cout << "  the original C++ code was written. This tool wraps " << endl;
  cout << "  the very same ZAIC_VECT C++ class as is used in the" << endl;
  cout << "  behaviors.                                         " << endl;
  cout << "                                                     " << endl;
  cout << "Options:                                             " << endl;
  cout << "  --help, -h           Display this help message     " << endl;
  cout << "  --verbose,           Enable verbose output         " << endl;
  cout << "  --version, -v,       Display the release version   " << endl;
  cout << "                                                     " << endl;
  cout << "Example:                                             " << endl;
  cout << " $ zaic_vect test.zaic --verbose                     " << endl;
  exit(0);
}


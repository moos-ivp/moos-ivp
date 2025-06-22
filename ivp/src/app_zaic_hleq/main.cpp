/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: June 20th, 2005                                      */
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
#include "ZAIC_HLEQ_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();
void idleProc(void*);

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
  int  domain  = 410;
  
  bool handled = true;
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-v") || (argi == "--version") || (argi=="-version")) {
      showReleaseInfo("zaic_hleq", "gpl");
      return(0);
    }
    else if(strBegins(argi, "--domain=")) {
      string domain_str = argi.substr(9);
      domain = vclip(atoi(domain_str.c_str()), 100, 1000);
    }
    else if(strBegins(argi, "--verbose")) 
      verbose = true;
    else
      handled = false;

    if(!handled) {
      cout << "Exiting due to Unhandled arg: " << argi << endl;
      exit(1);
    }      
  }

  Fl::add_idle(idleProc);
  ZAIC_HLEQ_GUI* gui = new ZAIC_HLEQ_GUI(domain+300, 450, "ZAIC-HLEQ-Viewer");

  gui->updateOutput();
  gui->setDomain((unsigned int)(domain));
  gui->setVerbose(verbose);

  // Enter the GUI event loop.
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << endl;
  cout << "Usage: zaic_hleq [OPTIONS]                           " << endl;
  cout << "  zaic_hleq [OPTIONS]                                " << endl;
  cout << "                                                     " << endl;
  cout << "Synopsis:                                            " << endl;
  cout << "  The zaic_hleq utility renders a configured instance" << endl;
  cout << "  of the ZAIC_HLEQ utility in the ivpbuild toolbox.  " << endl;
  cout << "  This allow the user to visualize the effects of    " << endl;
  cout << "  parameter settings for this tool. Normally this    " << endl;
  cout << "  tool is created and parameterized within an IvP    " << endl;
  cout << "  behavior. This tool may give behavior authors some " << endl;
  cout << "  insight into how the tool works. This tool was     " << endl;
  cout << "  also used as an engineering/validation tool when   " << endl;
  cout << "  the original C++ code was written. This tool wraps " << endl;
  cout << "  the very same ZAIC_HLEQ C++ class as is used in the" << endl;
  cout << "  behaviors.                                         " << endl;
  cout << "                                                     " << endl;
  cout << "Options:                                             " << endl;
  cout << "  --help, -h           Display this help message     " << endl;
  cout << "  --domain=410         Set upper value of domain     " << endl;
  cout << "  --verbose,           Enable verbose output         " << endl;
  cout << "  --version, -v,       Display the release version   " << endl;
  cout << "                                                     " << endl;
  cout << "Example:                                             " << endl;
  cout << " $ zaic_hleq --domain=500 --verbose                  " << endl;
  exit(0);
}



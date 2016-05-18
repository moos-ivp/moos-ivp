/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "ZAIC_SPD_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void showHelpAndExit();

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
      showReleaseInfo("zaic_spd", "gpl");
      return(0);
    }
    else if(strBegins(argi, "--domain=")) {
      string domain_str = argi.substr(9);
      domain = atoi(domain_str.c_str());
      if(domain < 100)
	domain = 100;
      if(domain > 1000)
	domain = 1000;   
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
  ZAIC_SPD_GUI* gui = new ZAIC_SPD_GUI(domain+300, 450, "ZAIC_SPD-Viewer");

  cout << "Starting Domain: " << domain << endl;
  
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
  cout << "Usage: zaic_spd [OPTIONS]                           " << endl;
  cout << "Options:                                            " << endl;
  cout << "  --help, -h           Display this help message    " << endl;
  cout << "  --domain=410         Set upper value of domain    " << endl;
  cout << "  --verbose,           Enable verbose output        " << endl;
  cout << "  --version, -v,       Display the release version  " << endl;
  cout << "                                                    " << endl;
  cout << "Example:                                            " << endl;
  cout << " $ zaic_spd --domain=500 --verbose                  " << endl;
  exit(0);
}

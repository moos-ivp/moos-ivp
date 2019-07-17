/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 4th 2012                                        */
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
#include "MBUtils.h"
#include "TermUtils.h"
#include "ColorParse.h"
#include "AppCastMonitor.h"
#include "AppCastMonitor_Info.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];
  bool   terse_mode = false;
  string initial_proc;
  string initial_node;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if((argi == "-t") || (argi == "--terse"))
      terse_mode = true;
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(strBegins(argi, "--node="))
      initial_node = argi.substr(7);
    else if(strBegins(argi, "--proc="))
      initial_proc = argi.substr(7);
    else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();

  // Build a unique run name based on the random number
  srand(time(NULL));
  int    rand_int = rand() % 10000;
  string rand_str = intToString(rand_int);
  run_command += "_" + rand_str;

  cout << termColor("green");
  cout << "uMAC launching as " << run_command << endl;
  cout << termColor() << endl;

  AppCastMonitor UMAC;
  UMAC.setTerseMode(terse_mode);
  UMAC.setInitialNode(initial_node);
  UMAC.setInitialProc(initial_proc);
  // start the UMAC in its own thread
  MOOSAppRunnerThread appRunner(&UMAC, (char*)(run_command.c_str()), 
				mission_file.c_str(), argc, argv);

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3)))   // ASCII 03 is control-c
      quit = true;
    else {
      UMAC.handleCommand(c);
    }
  }

  cout << "Quitting....." << endl;
  appRunner.quit();
  return(0);
}










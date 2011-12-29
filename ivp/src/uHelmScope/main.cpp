/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 12th 2008                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "HelmScope.h"
#include "HelmScope_Info.h"

#ifdef _WIN32
  #include <process.h>
  #define getpid _getpid
#endif

#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  HelmScope theHelmScope;
  string    mission_file;
  string    run_command = argv[0];

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
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if((argi == "-r") || (argi == "-s") || (argi == "--stream"))
      theHelmScope.setPaused(false);
    else if((argi == "-l") || (argi == "--life") || (argi == "-life"))
      theHelmScope.handleCommand('L');
    else if((argi == "-x") || (argi == "--noscope") || (argi == "-noscope"))
      theHelmScope.setDisplayXMS(false);
    else if((argi == "-p") || (argi == "--noposts") || (argi == "-noposts"))
      theHelmScope.setDisplayPosts(false);
    else if((argi == "-c") || (argi == "--clean") || (argi == "-clean"))
      theHelmScope.setIgnoreFileVars(true);
    else if((argi == "-t") || (argi == "--trunc") || (argi == "-trunc"))
      theHelmScope.setDisplayTrunc(true);
    else if((argi == "-g") || (argi == "--novirgins") || (argi == "-novirgins"))
      theHelmScope.setDisplayVirgins(false);
    else // This check needs to be last:
      theHelmScope.addVariable(argi, "user");
  }
  
  if(mission_file == "")
    showHelpAndExit();

  bool seed = true;
  if(seed) {
    unsigned long tseed = time(NULL)+1;
    unsigned long pid = (long)getpid()+1;
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }
  
  int rand_int = rand() % 10000;
  run_command += "_" + intToString(rand_int);

  MOOSAppRunnerThread appThread(&theHelmScope, 
				(char*)(run_command.c_str()), 
				mission_file.c_str());

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3))) { // ASCII 03 is control-c
      cout << "Quitting..." << endl;
      quit = true;
    }
    else
      theHelmScope.handleCommand(c);
  }

  appThread.quit();
  return(0);
}


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 12th 2008                                        */
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
#include "TermUtils.h"
#include "MBUtils.h"
#include "HelmScope.h"
#include "HelmScope_Info.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  HelmScope theHelmScope;
  string    mission_file;
  string    run_command = argv[0];
  string    server_host;
  string    server_port;

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
    else if(strBegins(argi, "--serverhost="))
      server_host = argi.substr(13);
    else if(strBegins(argi, "--serverport="))
      server_port = argi.substr(13);
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else // This check needs to be last:
      theHelmScope.addScopeVariable(argi);
  }
  
  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(mission_file == "") {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(server_host == "") {
      server_host = "localhost";
      cout << "Enter IP address:  [localhost] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n')
	  server_host = buff;
      }    
    }
    // If server_port info was not on the command line, prompt here.
    if(server_port == "") {
      server_port = "9000";
      cout << "Enter Port number: [9000] ";
      if(fgets(buff, 999, stdin) != NULL) {
	if(buff[0] != '\n')
	  server_port = buff; 
      }
    }
  }
  
  theHelmScope.setTermServerHost(server_host);
  theHelmScope.setTermServerPort(server_port);


  srand(time(NULL));
  int rand_int = rand() % 10000;
  run_command += "_" + intToString(rand_int);

  MOOSAppRunnerThread appThread(&theHelmScope, (char*)(run_command.c_str()), 
				mission_file.c_str(), argc, argv);
  
  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3))) // ASCII 03 is control-c
      quit = true;
    else
      theHelmScope.handleCommand(c);
  }

  cout << "Quitting..." << endl;
  appThread.quit();
  return(0);
}









/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: May 27th 2007                                        */
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

#include <cstring>
#include "XMS.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "OpenURL.h"
#include "XMS_Info.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  XMS    theXMS;

  string run_command = argv[0];
  string mission_file;
  string server_host;
  string server_port;
  
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
    else if((argi == "-g") || (argi == "--novirgins") || (argi=="-novirgins"))
      theXMS.setDispVirgins(false);
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if((argi == "-c") || (argi == "--clean") || (argi == "-clean"))
      theXMS.ignoreFileVars(true);
    else if((argi == "-a") || (argi == "--all"))
      theXMS.setDispAll(true);
    else if(argi == "--alll") {
      theXMS.setDispAll(true);
      theXMS.setDispAllReally(true);
    }
    else if(argi == "-t")
      theXMS.setTruncData("25");    
    else if(argi == "-p")
      theXMS.setRefreshMode("paused");    
    else if(argi == "-s")
      theXMS.setDisplayColumns("source");    
    else if(argi == "-st")
      theXMS.setDisplayColumns("source,time");    

    else if(strBegins(argi, "--server_host="))
      server_host = argi.substr(14);
    else if(strBegins(argi, "--serverhost="))
      server_host = argi.substr(13);
    else if(strBegins(argi, "--moos_host="))
      server_host = argi.substr(12);
    else if(strBegins(argi, "--mooshost="))
      server_host = argi.substr(11);

    else if(strBegins(argi, "--server_port="))
      server_port = argi.substr(14);
    else if(strBegins(argi, "--serverport="))
      server_port = argi.substr(13);
    else if(strBegins(argi, "--moos_port="))
      server_port = argi.substr(12);
    else if(strBegins(argi, "--moosport="))
      server_port = argi.substr(11);

    else if(strBegins(argi, "--termint="))
      theXMS.setTermReportInterval(argi.substr(10));
    else if(strBegins(argi, "--src="))
      theXMS.addSource(argi.substr(6));
    else if(strBegins(argi, "--filter="))
      theXMS.setFilter(argi.substr(9));
    else if(strBegins(argi, "--history="))
      theXMS.addVariable(argi.substr(10), true); // true means history variable
    else if(strBegins(argi, "--mode="))
      theXMS.setRefreshMode(argi.substr(7));    
    else if(strBegins(argi, "--show="))
      theXMS.setDisplayColumns(argi.substr(7));
    else if(strBegins(argi, "--colormap="))
      theXMS.setColorMappingsPairs(argi.substr(11));
    else if(strBegins(argi, "--colorany="))
      theXMS.setColorMappingsAny(argi.substr(11));
    else if(strBegins(argi, "--trunc="))
      theXMS.setTruncData(argi.substr(8));
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/uXMS");

    // To support pAntler convention where argv[2] indicates an alternative
    // procname, we allow this when argv[2] begins with uXMS. In this case
    // it is likely that argv[2] does not represent a var to be scoped.
    else if((i==2) && strBegins(argi, "uXMS"))
      run_command = argi;
    else 
      theXMS.addVariable(argi);
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
  
  theXMS.setTermServerHost(server_host);
  theXMS.setTermServerPort(server_port);

  if(mission_file == "") {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
  }
  else
    cout << "Mission File was provided: " << mission_file << endl;

  // Prior to augmenting the run_command, store the base name.
  theXMS.setAppNameNoIndex(run_command);

  if(run_command == "uXMS") {
    // Build a unique run name based on the random number
    srand(time(NULL));
    int    rand_int = rand() % 1000;
    string rand_str = intToString(rand_int);
    run_command += "_" + rand_str;
  }

  // start the XMS in its own thread
  MOOSAppRunnerThread appRunner(&theXMS, 
				(char*)(run_command.c_str()), 
				mission_file.c_str(), argc, argv);
  
  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3)))   // ASCII 03 is control-c
      quit = true;
    else
      theXMS.handleCommand(c);
  }

  cout << "Quitting....." << endl;
  //appRunner.quit();
  exit(0);
}










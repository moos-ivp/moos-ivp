/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main_uXMS.cpp                                        */
/*    DATE: May 27th 2007                                        */
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

#include <cstring>
#include "XMS.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "XMS_Info.h"

#ifdef _WIN32
   #include <process.h>
#endif

#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  string mission_file = "";
  string run_command = argv[0];

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
  }
  
  string server_host     = "localhost";
  bool   server_host_set = false;
  int    server_port     = 9000;
  bool   server_port_set = false;
  bool   seed = true;
  for(int i=1; i<argc; i++) {
    string argi = tolower(argv[i]);
    if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(argi == "-noseed")
      seed = false;
    else if(strContains(argi, "=")) {
      string left  = stripBlankEnds(biteString(argi, '='));
      string right = stripBlankEnds(argi);
      string lleft = tolower(left);
      if((lleft == "server_host") || (lleft == "serverhost") ||
	 (lleft == "--server_host") || (lleft == "--serverhost")) {
	server_host     = right;
	server_host_set = true;
      }
      else if((lleft == "server_port") || (lleft=="serverport") ||
	      (lleft == "--server_port") || (lleft=="--serverport")) {
	if(isNumber(right)) {
	  server_port     = atoi(right.c_str());
	  server_port_set = true;
	}
      }
    }
  }

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(mission_file == "") {
    char buff[1000];
    // If server_host info was not on the command line, prompt here.
    if(!server_host_set) {
      cout << "Enter IP address:  [localhost] ";
      fgets(buff, 999, stdin);
      if(buff[0] != '\n') {
	server_host     = buff;    
	server_host_set = true;
      }
    }
    // If server_port info was not on the command line, prompt here.
    if(!server_port_set) {
      cout << "Enter Port number: [9000] ";
      fgets(buff, 999, stdin);
      if(buff[0] != '\n') {
	server_port     = atoi(buff); 
	server_port_set = true;
      }
    }
  }
  
  XMS g_theXMS(server_host, server_port);

  if(mission_file == "") {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    g_theXMS.setConfigureCommsLocally(true);
  }
  else {
    cout << "Mission File was provided: " << mission_file << endl;
  }

  // Handle the building of the uXMS process name.
  if(seed) {
    // Add 1 to each in case one returns a zero in an error case
    unsigned long tseed = time(NULL) + 1;
#ifdef _WIN32
    unsigned long hostid = 0; 
    char hostname[256];
    if(gethostname(hostname, 256) == 0 ){
      hostent *host = gethostbyname(hostname);
      if(host != NULL){
	hostid = *(u_long *)host->h_addr_list[0];
      }
    }
    hostid += 1;
#else
    unsigned long hostid = gethostid() + 1; 
#endif
    unsigned long pid = (long)getpid() + 1;
    unsigned long seed = (tseed%999999);
    seed = ((rand()) * seed * hostid) % 999999;
    seed = (seed * pid) % 999999;
    srand(seed);
    int    rand_int = rand() % 1000;
    string rand_str = intToString(rand_int);
    run_command += "_" + rand_str;
  }

  // start the XMS in its own thread
  MOOSAppRunnerThread appRunner(&g_theXMS, 
				(char*)(run_command.c_str()), 
				mission_file.c_str());

  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if((str == "-c") || (str == "--clean") || (str == "-clean"))
      g_theXMS.ignoreFileVars(true);
    
    else if(strBegins(str, "--mode=")) {
      biteString(str, '=');
      str = tolower(str);
      if(str=="events")
	g_theXMS.setRefreshMode("events");
      else if(str=="paused")
	g_theXMS.setRefreshMode("paused");
      else if(str=="streaming")
	g_theXMS.setRefreshMode("streaming");
    }

    else if(strBegins(str, "--mask=")) {
      biteString(str, '=');
      str = tolower(str);
      if((str=="virgin") || (str == "virgins"))
	g_theXMS.setDispVirgins(false);
      else if(str=="empty")
	g_theXMS.setDispEmptyStrings(false);
    }

    else if(strBegins(str, "--show=")) {
      biteString(str, '=');
      str = tolower(str);
      vector<string> svector = parseString(str, ',');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	if(svector[i]=="source")
	  g_theXMS.setDispSource(true);
	else if(svector[i]=="time")
	  g_theXMS.setDispTime(true);
	else if(svector[i]=="community")
	  g_theXMS.setDispCommunity(true);
	else if(svector[i]=="aux")
	  g_theXMS.setDispAuxSource(true);
      }
    }

    else if((str == "-a") || (str == "--all"))
      g_theXMS.setDispAll(true);

    else if(strBegins(str, "--colormap=")) {
      biteString(str, '=');
      string varname = biteString(str, ',');
      string colname = tolower(stripBlankEnds(str));
      g_theXMS.setColorMapping(varname, colname);
    }
    
    else if(strBegins(str, "--trunc=")) {
      biteString(str, '=');
      if(isNumber(str)) 
	g_theXMS.setTruncData(atof(str.c_str()));
    }
    
    else if(str == "-t")
      g_theXMS.setTruncData(45);
    
    else if(strBegins(str, "--colorany=")) {
      biteString(str, '=');
      vector<string> svector = parseString(str, ',');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	string varname = stripBlankEnds(svector[i]);
	g_theXMS.setColorMapping(varname, "any");
      }
    }

    else if(strBegins(str, "--src=")) {
      biteString(str, '=');
      vector<string> svector = parseString(str, ',');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	string srcname = stripBlankEnds(svector[i]);
	g_theXMS.addSource(srcname);
      }
    }

    else if(strBegins(str, "--filter=")) {
      biteString(str, '=');
      g_theXMS.setFilter(str);
    }

    else if(strBegins(str, "--history="))
      g_theXMS.addVariable(str.substr(10), true);

    // "history" without the double dashes is deprecated
    else if(strBegins(str, "history="))
      g_theXMS.addVariable(str.substr(8), true);
    
    else if((!strContains(str, ".moos")) && (str != "-noseed"))
      g_theXMS.addVariable(str);
  }

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3))) {   // ASCII 03 is control-c
      quit = true;
      cout << "Quitting....." << endl;
    }
    else
      g_theXMS.handleCommand(c);
  }

  appRunner.quit();

  return(0);
}








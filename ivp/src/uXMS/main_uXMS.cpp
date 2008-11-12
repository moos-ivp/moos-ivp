/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

#include <string>
#include <cstring>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "XMS.h"
#include "TermUtils.h"
#include "MBUtils.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

const char*  g_sMissionFile = "uXMS.moos";
pthread_t    g_threadID;

struct ThreadParams {
    CMOOSApp *app;
    char *name;
};

//--------------------------------------------------------
// Procedure: RunProc

void* RunProc(void *lpParameter)
{
  void **params = (void **)lpParameter;
  
  CMOOSApp *app = (CMOOSApp *)params[0];
  char *name = (char *) params[1];
  
  MOOSTrace("starting %s thread\n", name);
  app->Run(name, g_sMissionFile);	
  
  return(NULL);
}

//--------------------------------------------------------
// Procedure: spawn_thread

pthread_t spawn_thread(ThreadParams *pParams)
{
  pthread_t tid;
  if(pthread_create(&tid,NULL, RunProc, pParams) != 0) {
    MOOSTrace("failed to start %s thread\n", pParams->name);
    tid = (pthread_t) -1;
  }
  else 
    MOOSTrace("%s thread spawned\n", pParams->name);
  
  return(tid);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uXMS");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: uXMS [file.moos] [-nav] [-helm] [-pid] [-proc] " << endl;
    cout << "       [--clean|-c] [--resumed|-r] [--virgins|-v]     " << endl;
    cout << "       [--source|-s] [--time|-t] [--all|-a]           " << endl;
    cout << "       [server_host=value] [server_port=value]        " << endl;
    cout << "       [MOOS_VARIABLES]                               " << endl;
    cout << "                                                      " << endl;
    cout << "[file.moos] Filename to get configuration parameters  " << endl;
    cout << "[-nav]      Auto-subscribe for NAV_* variables        " << endl;
    cout << "[-helm]     Auto-subscribe for IvPHelm variables      " << endl;
    cout << "[-pid]      Auto-subscribe for PID (DESIRED_*) vars   " << endl;
    cout << "[-proc]     Auto-subscribe for uProcessWatch vars     " << endl;
    cout << "[-c]        Ignore scope variables in file.moos       " << endl;
    cout << "[-r]        Start in data-streaming mode              " << endl;
    cout << "[-v]        Don't display virgin variables            " << endl;
    cout << "[-s]        Show the Source field in the data report  " << endl;
    cout << "[-t]        Show the Time field in the data report    " << endl;
    cout << "[-a]        Show ALL MOOS variables in the MOOSDB     " << endl;
    cout << "[server_host=value]  Connect to MOOSDB at IP=value    " << endl;
    cout << "[server_port=value]  Connect to MOOSDB at port=value  " << endl;
    cout << endl;
    return(0);
  }

  string server_host     = "localhost";
  bool   server_host_set = false;
  int    server_port     = 9000;
  bool   server_port_set = false;
  bool   seed = true;
  for(int i=1; i<argc; i++) {
    string str = tolower(argv[i]);
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
    else if(str == "-noseed")
      seed = false;
    else if(strContains(str, "=")) {
      string left  = stripBlankEnds(biteString(str, '='));
      string right = stripBlankEnds(str);
      string lleft = tolower(left);
      if((lleft == "server_host") || (lleft == "serverhost")) {
	server_host     = right;
	server_host_set = true;
      }
      else if((lleft == "server_port") || (lleft=="serverport")) {
	if(isNumber(right)) {
	  server_port     = atoi(right.c_str());
	  server_port_set = true;
	}
      }
    }
  }

  bool mission_file_provided = false;
  if(strcmp(g_sMissionFile, "uXMS.moos"))
    mission_file_provided = true;

  // If the mission file is not provided, we prompt the user if the 
  // server_host or server_port information is not on command line.
  if(!mission_file_provided) {
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

  if(mission_file_provided == false) {
    cout << "Mission File not provided. " << endl;
    cout << "  server_host  = " << server_host << endl;
    cout << "  server_port  = " << server_port << endl;
    g_theXMS.setConfigureCommsLocally(true);
  }
  else {
    cout << "Mission File was provided: " << g_sMissionFile << endl;
  }



  // Handle the building of the uXMS process name.
  string process_name = "uXMS";
  if(seed) {
    // Add 1 to each in case one returns a zero in an error case
    unsigned long tseed = time(NULL) + 1;
    unsigned long hostid = gethostid() + 1;
    unsigned long pid = (long)getpid() + 1;
    unsigned long seed = (tseed%999999);
    seed = ((rand()) * seed * hostid) % 999999;
    seed = (seed * pid) % 999999;
    srand(seed);
    int    rand_int = rand() % 1000;
    string rand_str = intToString(rand_int);
    process_name = "uXMS_" + rand_str;
  }

  // start the XMS in its own thread
  ThreadParams params = {&g_theXMS, (char*)(process_name.c_str())};
  g_threadID = spawn_thread(&params);	

  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(str == "-nav") {
      g_theXMS.addVariable("NAV_X");
      g_theXMS.addVariable("NAV_Y");
      g_theXMS.addVariable("NAV_HEADING");
      g_theXMS.addVariable("NAV_SPEED");
      g_theXMS.addVariable("NAV_DEPTH");
    }
    else if(str ==  "-helm") {
      g_theXMS.addVariable("DESIRED_HEADING");
      g_theXMS.addVariable("DESIRED_SPEED");
      g_theXMS.addVariable("DESIRED_DEPTH");
      g_theXMS.addVariable("BHV_WARNING");
      g_theXMS.addVariable("BHV_ERROR");
      g_theXMS.addVariable("HELM_IPF_COUNT");
      g_theXMS.addVariable("HELM_ACTIVE_BHV");
      g_theXMS.addVariable("HELM_NONIDLE_BHV");
      g_theXMS.addVariable("MOOS_MANUAL_OVERIDE");
      g_theXMS.addVariable("DEPLOY");
      g_theXMS.addVariable("RETURN");
      g_theXMS.addVariable("STATION_KEEP");
    }
    else if(str == "-pid") {
      g_theXMS.addVariable("DESIRED_RUDDER");
      g_theXMS.addVariable("DESIRED_THRUST");
      g_theXMS.addVariable("DESIRED_ELEVATOR");
    }
    else if(str == "-proc") {
      g_theXMS.addVariable("PROC_WATCH_SUMMARY");
      g_theXMS.addVariable("PROC_WATCH_EVENT");
    }

    else if((str == "-c") || (str == "--clean") || (str == "-clean"))
      g_theXMS.ignoreVars(true);
    
    else if((str == "-r") || (str == "--resumed"))
      g_theXMS.setPaused(false);

    else if((str == "-v") || (str == "--virgins"))
      g_theXMS.setDispVirgins(false);

    else if(str == "-e") 
      g_theXMS.setDispEmptyStrings(false);

    else if((str == "-s") || (str == "--source"))
      g_theXMS.setDispSource(true);

    else if((str == "-t") || (str == "--time"))
      g_theXMS.setDispTime(true);

    else if((str == "-a") || (str == "--all"))
      g_theXMS.setDispAll(true);
    
    else if((!strContains(argv[i], ".moos")) && (str != "-noseed"))
      g_theXMS.addVariable(argv[i]);
  }

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else
      g_theXMS.handleCommand(c);
  }
 
  return(0);
}







/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 12th 2008                                        */
/*****************************************************************/

#include <iostream>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "HelmScope.h"
#include "TermUtils.h"
#include "MBUtils.h"

#ifdef _WIN32
  #include <process.h>
  #define getpid _getpid
#endif

#include "MOOSAppRunnerThread.h"

using namespace std;

string getUserLine();

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uHelmScope");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for help or usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage:                                                   " << endl;
    cout << "  uHelmScope moosfile.moos [OPTIONS] [MOOSVARS]          " << endl;
    cout << "                                                         " << endl;
    cout << "Synopsis:                                                " << endl;
    cout << "  A scope onto a running IvP Helm process, and key MOOS  " << endl;
    cout << "  variables. Shows behavior summaries/activity states.   " << endl;
    cout << "                                                         " << endl;
    cout << "Options:                                                 " << endl;
    cout << "  -h,--help      Displays this help message              " << endl;
    cout << "  -v,--version   Displays the current release version    " << endl;
    cout << "  -t,--trunc     Column output truncation is enabled     " << endl;
    cout << "  -c,--clean     MOOS variables specified in given .moos " << endl;
    cout << "                 file are excluded from  MOOSDB-Scope    " << endl;
    cout << "  -x,--noscope   Suppress MOOSDB-Scope output block      " << endl;
    cout << "  -p,--noposts   Suppress Behavior-Posts output block    " << endl;
    cout << "  -g,--novirgins Suppress virgin vars in MOOSDB-Scope    " << endl;
    cout << "  -s,--stream    Streaming (unpaused) output enabled     " << endl;
    cout << "  -l,--life      Launch in Life Events History mode      " << endl;
    cout << "                                                         " << endl;
    cout << "MOOS Variables                                           " << endl;
    cout << "  MOOSVAR_1 MOOSVAR_2 .... MOOSVAR_N                     "<< endl;
    cout << "                                                         " << endl;
    cout << "Further Notes:                                           " << endl;
    cout << "  (1) The order of command line arguments is irrelevant. " << endl;
    cout << "  (2) Any MOOS variable used in behavior run conditions, " << endl;
    cout << "      or used in hiearchical mode declarations will be   " << endl;
    cout << "      automatically subscribed for in the MOOSDB scope.  " << endl;
    cout << "                                                         " << endl;
    return(0);
  }

  int    i;
  string mission_file = "";
  
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
  }

  HelmScope theHelmScope;
  
  if(mission_file == "") {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n");
    return(0);
#if 0
    cout << "Server Address (default LOCALHOST): ";
    string server_address = getUserLine();
    cout << "Server Port (default 9000): ";
    string server_port_s = getUserLine();
    double server_port = atof(server_port_s.c_str());
    cout << "AppTick (default 2): ";
    string app_tick_s = getUserLine();
    double app_tick = atof(app_tick_s.c_str());
   
    cout << "Launching with: " << endl;
    cout << "  Server Address: " << server_address << endl;
    cout << "  Server Port:    " << server_port << endl;
    cout << "  AppTick:        " << app_tick << endl;
  
    theHelmScope.setAppTick(app_tick);
    theHelmScope.setServer(server_address.c_str(), server_port);
#endif
  }
    
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
  string rand_str = intToString(rand_int);
  
  string process_name = "uHelmScope_" + rand_str;

  MOOSAppRunnerThread appThread(&theHelmScope, 
				(char*)(process_name.c_str()), 
				mission_file.c_str());

  bool life_mode = false;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    
    cout << "arg[" << i << "]:  [" << str << "]"<< endl;

    if((str == "-r") || (str == "-s") || (str == "--stream"))
      theHelmScope.setPaused(false);
    else if((str == "-l") || (str == "--life"))
      theHelmScope.handleCommand('L');
    else if((str == "-x") || (str == "-noscope"))
      theHelmScope.setDisplayXMS(false);
    else if((str == "-p") || (str == "-noposts"))
      theHelmScope.setDisplayPosts(false);
    else if((str == "-c") || (str == "--clean") || (str == "-clean"))
      theHelmScope.setIgnoreFileVars(true);
    else if((str == "-t") || (str == "--trunc") || (str == "-trunc"))
      theHelmScope.setDisplayTrunc(true);
    else if((str == "-g") || (str == "--novirgins") || (str == "--virgins"))
      theHelmScope.setDisplayVirgins(false);
    // This check needs to be last:
    else if(!strContains(str, ".moos"))
      theHelmScope.addVariable(str, "user");
  }

  cout << "LifeMode:" << boolToString(life_mode) << endl;

  //if(life_mode)
  //  theHelmScope.setDisplayLEHistory();

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3)))  // ASCII 03 is control-c
      quit = true;
    else
      theHelmScope.handleCommand(c);
  }

  appThread.quit();
  return(0);
}




//-------------------------------------------------------------
// Procedure: getUserLine

string getUserLine()
{
  char buffer[1000];
  int  ix = 0;
  while((buffer[ix]=getchar())!='\n')
    ix++;
  buffer[ix] = '\0';
  string rval = buffer;
  return(rval);
}
      

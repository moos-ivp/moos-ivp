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
    MOOSTrace("Usage: uHelmScope moosfile.moos [switches] [MOOSVARS]    \n");
    MOOSTrace("  -t:  Column truncation is on (off by default)          \n");
    MOOSTrace("  -c:  Exclude MOOS Vars in MOOS file from MOOSDB-Scope  \n");
    MOOSTrace("  -x:  Suppress MOOSDB-Scope output block                \n");
    MOOSTrace("  -p:  Suppress Behavior-Posts output block              \n");
    MOOSTrace("  -v:  Suppress display of virgins in MOOSDB-Scope block \n");
    MOOSTrace("  -r:  Streaming (unpaused) output of helm iterations    \n");
    MOOSTrace("  MOOSVAR_1 MOOSVAR_2 .... MOOSVAR_N\n\n");
    return(0);
  }

  const char * g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
  }

  HelmScope theHelmScope;
  
  if(!g_sMissionFile) {
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

  MOOSAppRunnerThread appThread(
    &theHelmScope, (char*)(process_name.c_str()), g_sMissionFile);

  for(int i=1; i<argc; i++) {
    string str = argv[i];
    
    if((str == "-r") || (str == "--resume"))
      theHelmScope.setPaused(false);
    else if(str == "-x")
      theHelmScope.setDisplayXMS(false);
    else if(str == "-p")
      theHelmScope.setDisplayPosts(false);
    else if((str == "-c") || (str == "--clean") || (str == "-clean"))
      theHelmScope.setIgnoreFileVars(true);
    else if((str == "-t") || (str == "--trunc") || (str == "-trunc"))
      theHelmScope.setDisplayTrunc(true);
    else if((str == "-v") || (str == "--virgins") || (str == "-virgins"))
      theHelmScope.setDisplayVirgins(false);

    else if(!strContains(argv[i], ".moos"))
      theHelmScope.addVariable(argv[i], "user");
    
  }

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
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
      

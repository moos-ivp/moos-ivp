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

using namespace std;

string getUserLine();

// ----------------------------------------------------------
// global variables here

char*       g_sMissionFile = 0;
HelmScope   g_theHelmScope;
pthread_t   g_threadID;

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

  g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
  }
  
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
  
    g_theHelmScope.setAppTick(app_tick);
    g_theHelmScope.setServer(server_address.c_str(), server_port);    
#endif
  }
    
  bool seed = true;
  if(seed) {
    unsigned long tseed = time(NULL);
    unsigned long hostid = gethostid();
    unsigned long pid = (long)getpid();
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed*hostid)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }
  
  int rand_int = rand() % 10000;
  string rand_str = intToString(rand_int);
  
  string process_name = "uHelmScope_" + rand_str;
  
  // start the HelmScope in its own thread
  ThreadParams params = {&g_theHelmScope, (char*)(process_name.c_str())};
  g_threadID = spawn_thread(&params);	

  for(int i=1; i<argc; i++) {
    string str = argv[i];
    
    if((str == "-r") || (str == "--resume"))
      g_theHelmScope.setPaused(false);
    else if(str == "-x")
      g_theHelmScope.setDisplayXMS(false);
    else if(str == "-p")
      g_theHelmScope.setDisplayPosts(false);
    else if((str == "-c") || (str == "--clean") || (str == "-clean"))
      g_theHelmScope.setIgnoreFileVars(true);
    else if((str == "-t") || (str == "--trunc") || (str == "-trunc"))
      g_theHelmScope.setDisplayTrunc(true);
    else if((str == "-v") || (str == "--virgins") || (str == "-virgins"))
      g_theHelmScope.setDisplayVirgins(false);

    else if(!strContains(argv[i], ".moos"))
      g_theHelmScope.addVariable(argv[i], "user");
    
  }

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else
      g_theHelmScope.handleCommand(c);
  }
 
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
      

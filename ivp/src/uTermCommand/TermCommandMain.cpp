/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommandMain.cpp                                  */
/*    DATE: June 26th 2007                                       */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "TermCommand.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

char*        g_sMissionFile = 0;
TermCommand  g_theTermCommand;
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
    vector<string> svector = getReleaseInfo("uTermCommand");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for help or usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    MOOSTrace("Usage: uTermCommand moosfile.moos          \n");
    return(0);
  }

  g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
  }

  if(!g_sMissionFile) {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n\n");
    return(0);
  }
 
  // start the TermCommand in its own thread
  ThreadParams params = {&g_theTermCommand, argv[0]};
  g_threadID = spawn_thread(&params);	

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else {
      g_theTermCommand.m_tc_mutex.Lock();
      g_theTermCommand.handleCharInput(c);
      g_theTermCommand.m_tc_mutex.UnLock();
    }
  }

  return(0);
}


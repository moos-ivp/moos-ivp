/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: uFunctionVisMain.cpp                                 */
/*    DATE: May 12th 2006                                        */
/*****************************************************************/

#include <iostream>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "FV_MOOSApp.h"
#include "FV_GUI.h"
#include "MBUtils.h"

using namespace std;

// -------------------------------------------------------
// global variables here
//

char*           g_sMissionFile = 0;
FV_MOOSApp      g_thePort;
pthread_t       g_portThreadID;
pthread_mutex_t g_mutex;

struct ThreadParams {
    CMOOSApp *app;
    char *name;
};

//--------------------------------------------------------
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: uFunctionViewer file.moos" << endl;
  exit(-1);
}


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
  if (pthread_create(&tid,NULL, RunProc, pParams) != 0) {
    MOOSTrace("failed to start %s thread\n", pParams->name);
    tid = (pthread_t) -1;
  }
  else 
    MOOSTrace("%s thread spawned\n", pParams->name);
  
  return tid;
}

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  MOOSPause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strContains(argi, ".moos"))
      g_sMissionFile = argv[i];
  }

  if(g_sMissionFile == 0)
    exit_with_usage();

  // start the MOOSPort in its own thread
  ThreadParams params = {&g_thePort, "uFunctionViewer"};
  g_portThreadID = spawn_thread(&params);	
  
  Fl::add_idle(idleProc);
  
  FV_GUI* gui = new FV_GUI(900,750, "IvPFunction-Viewer");
  if(gui)
    g_thePort.setViewer(gui->getViewer());

  FV_Model* model = new FV_Model();
  gui->setModel(model);
  g_thePort.setModel(model);
  g_thePort.setGUI(gui);
 
  return Fl::run();
}

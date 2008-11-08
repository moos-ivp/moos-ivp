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
#include "MOOSAppRunnerThread.h"

using namespace std;

// -------------------------------------------------------
// global variables here
//

char*           g_sMissionFile = 0;
FV_MOOSApp      g_thePort;

//--------------------------------------------------------
// Procedure: exit_with_usage

void exit_with_usage()
{
  cout << "Usage: uFunctionViewer file.moos" << endl;
  exit(0);
}

//--------------------------------------------------------
// Procedure: exit_with_version

void exit_with_version()
{
  vector<string> svector = getReleaseInfo("uFunctionVis");
  for(int i=0; i<svector.size(); i++)
    cout << svector[i] << endl;
  exit(0);
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
  bool version_requested = false;
  for(int i=1; i<argc; i++) {
    string str  = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested)
    exit_with_version();
  
  if(g_sMissionFile == 0)
    exit_with_usage();

  // start the MOOSPort in its own thread
  MOOSAppRunnerThread runner(&g_thePort, "uFunctionViewer", g_sMissionFile);
  
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

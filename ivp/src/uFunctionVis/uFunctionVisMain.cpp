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
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: uFunctionViewer file.moos" << endl;
  exit(-1);
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

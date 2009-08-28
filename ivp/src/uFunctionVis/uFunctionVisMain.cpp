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
// Procedure: main

int main(int argc, char *argv[])
{
  char* sMissionFile = 0;
  
  bool version_requested = false;
  for(int i=1; i<argc; i++) {
    string str  = argv[i];
    if(strContains(str, ".moos"))
      sMissionFile = argv[i];
    if((str=="-v") || (str=="--version") || (str=="-version"))
      version_requested = true;
  }
  
  if(version_requested)
    exit_with_version();
  
  if(sMissionFile == 0)
    exit_with_usage();

  FV_MOOSApp thePort;
  MOOSAppRunnerThread runner(&thePort, "uFunctionViewer", sMissionFile);
  
  FV_GUI* gui = new FV_GUI(900,750, "IvPFunction-Viewer");
  if(gui)
    thePort.setViewer(gui->getViewer());

  FV_Model* model = new FV_Model();
  gui->setModel(model);
  thePort.setModel(model);
  thePort.setGUI(gui);

  Fl::lock();
  while (Fl::wait() > 0) {
    // We use the posting of a thread message (Fl::awake()) entirely
    // to cause Fl::wait() to return.
    thePort.process_demuxer_content();
  }

//   thePort.return_from_Run();
  runner.quit();
//   runner.join();

  return 0;
}

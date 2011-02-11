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
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uFunctionVis");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  string mission_file = "";
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argi;
    else if(strEnds(argi, ".moos++"))
      mission_file = argi;
  }
  
  if(mission_file == "") {
    cout << "Usage: uFunctionViewer file.moos" << endl;
    return(0);
  }

  FV_MOOSApp thePort;
  MOOSAppRunnerThread runner(&thePort, "uFunctionViewer", mission_file.c_str());
  
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

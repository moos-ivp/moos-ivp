/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: uFunctionVisMain.cpp                                 */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "FV_MOOSApp.h"
#include "FV_GUI.h"
#include "FV_Info.h"
#include "MBUtils.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "uFunctionVis launching as " << run_command << endl;
  cout << termColor() << endl;

  FV_MOOSApp thePort;
  MOOSAppRunnerThread runner(&thePort, run_command.c_str(), 
			     mission_file.c_str(), argc, argv);
  
  FV_GUI* gui = new FV_GUI(850,550, "IvPFunction-Viewer");
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

  return(0);
}










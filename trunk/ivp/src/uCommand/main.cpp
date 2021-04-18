/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: July 1st, 2016                                       */
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
#include "MBUtils.h"
#include "ColorParse.h"
#include "Threadsafe_pipe.h"
#include "MOOS_event.h"
#include "MOOSAppRunnerThread.h"
#include "UCMD_MOOSApp.h"
#include "UCMD_GUI.h"
#include "UCMD_Info.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

Threadsafe_pipe<MOOS_event> g_pending_moos_events;

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
  cout << "uCommand launching as " << run_command << endl;
  cout << termColor() << endl;

  UCMD_GUI* gui = new UCMD_GUI(500,500, run_command.c_str());
  if(!gui) {
    cout << "Unable to instantiate the GUI - exiting." << endl;
    return(-1);
  }

  UCMD_MOOSApp theUMV;

  theUMV.setGUI(gui);
  theUMV.setPendingEventsPipe(& g_pending_moos_events);

  // start the MOOSPort in its own thread
  MOOSAppRunnerThread appRunner(&theUMV, (char*)(run_command.c_str()),
				mission_file.c_str(), argc, argv);
  
  Fl::lock();
  
  while (Fl::wait() > 0) {
    // We use the posting of a thread message (Fl::awake()) entirely
    // to cause Fl::wait() to return.  That should minimize the
    // latency between when we enqueue MOOS_events into
    // g_pending_moos_events, and when this thread gets a chance to
    // act on them.
    while (!g_pending_moos_events.empty()) {
      // This is the only thread performing dequeues, so this call
      // should never block, since we already confirmed the pipe isn't
      // empty.
      MOOS_event e;
      bool success = g_pending_moos_events.dequeue(e);
      assert(success);
      
      if(e.type == "OnNewMail")
        theUMV.handleNewMail(e);
      else if(e.type == "Iterate")
        theUMV.handleIterate(e);
      else if(e.type == "OnStartUp")
        theUMV.handleStartUp(e);
    }
  }

  cout << "Quitting....." << endl;
  appRunner.quit();
  delete gui;
  return(0);
}










/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main_mviewer.cpp                                     */
/*    DATE:                                                      */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "Threadsafe_pipe.h"
#include "MOOS_event.h"
#include "MOOSAppRunnerThread.h"
#include "PMV_MOOSApp.h"
#include "PMV_GUI.h"
#include "PMV_Info.h"

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
  cout << "pMarineViewer launching as " << run_command << endl;
  cout << termColor() << endl;

  // For document screen shots:
  //PMV_GUI* gui = new PMV_GUI(1100,640, "pMarineViewer");

  // For lecture resolution:
  //PMV_GUI* gui = new PMV_GUI(880,480, "pMarineViewer");

  PMV_GUI* gui = new PMV_GUI(1100,850, "pMarineViewer");
  if(!gui) {
    cout << "Unable to instantiate the GUI - exiting." << endl;
    return(-1);
  }

  PMV_MOOSApp thePort;

  thePort.setGUI(gui);
  thePort.setPendingEventsPipe(& g_pending_moos_events);
  
  // start the MOOSPort in its own thread
  
  // At least on OS X we can get a complete pathanme as argv[0],
  // rather than just a simple file name for the executable.  Since
  // sections are looked up in the .moos file based on the simple
  // filename, we need to strip off other pathname components.

  string name = parseAppName(argv[0]);
  char * appFilename = const_cast<char*>(name.c_str());

  MOOSAppRunnerThread portAppRunnerThread(&thePort, appFilename, 
					  mission_file.c_str());

  Fl::lock();
  
  while (Fl::wait() > 0) {
    // We use the posting of a thread message (Fl::awake()) entirely
    // to cause Fl::wait() to return.  That should minimize the
    // latency between when we enqueue MOOS_events into
    // g_pending_moos_events, and when this thread gets a chance to
    // act on them.
    while (! g_pending_moos_events.empty()) {
      // This is the only thread performing dequeues, so this call
      // should never block, since we already confirmed the pipe isn't
      // empty.
      MOOS_event e;
      bool success = g_pending_moos_events.dequeue(e);
      assert(success);
      
      if (e.type == "OnNewMail") {
        thePort.handleNewMail(e);
      }
      else if (e.type == "Iterate") {
        thePort.handleIterate(e);
      }
      else if (e.type == "OnStartUp") {
        thePort.handleStartUp(e);
      }
    }
  }

  portAppRunnerThread.quit();
  delete gui;
  return 0;
}


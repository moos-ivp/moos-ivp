/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Sep 25th, 2011                                       */
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
#include "ULH_MOOSApp.h"
#include "ULH_GUI.h"
#include "LauncherULH.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "Threadsafe_pipe.h"
#include "MOOS_event.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

// ----------------------------------------------------------
// global variables here
//

Threadsafe_pipe<MOOS_event> g_pending_moos_events;
ULH_GUI *gui = 0;


//--------------------------------------------------------
// Procedure: exit_with_usage

void exit_with_usage()
{
  cout << "Usage: uLogViewHelm file.alog" << endl;
  exit(-1);
}

//--------------------------------------------------------
// Procedure: exit_with_switches

void exit_with_switches()
{
  cout << "uLogViewHelm switches:                        " << endl;
  exit(-1);
}


//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("uLogViewHelm", "gpl");
    return(0);
  }

  string mission_file = "";

  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help"))
    exit_with_usage();
  
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argv[i];
  }
  
  LauncherULH launcher;
  gui = launcher.launch(argc, argv);
  if(!gui)
    exit(0);

  string vehicle_name = launcher.getVName();
  ULH_MOOSApp thePort;

  thePort.setGUI(gui);
  thePort.setPendingEventsPipe(& g_pending_moos_events);
  
  // start the MOOSPort in its own thread
  
  // At least on OS X we can get a complete pathanme as argv[0],
  // rather than just a simple file name for the executable.  Since
  // sections are looked up in the .moos file based on the simple
  // filename, we need to strip off other pathname components.

  string name = parseAppName(argv[0]);
  name += ("_" + vehicle_name);

  char *appFilename = const_cast<char*>(name.c_str());

  MOOSAppRunnerThread portAppRunnerThread(&thePort, appFilename, 
					  mission_file.c_str());

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
      
      if (e.type == "OnNewMail")
        thePort.handleNewMail(e);
      else if (e.type == "Iterate")
        thePort.handleIterate(e);
      else if (e.type == "OnStartUp")
        thePort.handleStartUp(e);
    }
  }

  portAppRunnerThread.quit();
  delete(gui);
  return(0);
}

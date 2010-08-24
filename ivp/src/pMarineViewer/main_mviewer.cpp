/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include "PMV_MOOSApp.h"
#include "PMV_GUI.h"
#include "MBUtils.h"
#include "Threadsafe_pipe.h"
#include "MOOS_event.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

// ----------------------------------------------------------
// global variables here
//

Threadsafe_pipe<MOOS_event> g_pending_moos_events;

//--------------------------------------------------------
// Procedure: exit_with_usage

void exit_with_usage()
{
  cout << "Usage: pMarineViewer file.moos [file.tif] [-noimg]" << endl;
  exit(-1);
}

//--------------------------------------------------------
// Procedure: exit_with_switches

void exit_with_switches()
{
  cout << "pMarineViewer switches:                        " << endl;
  cout << "(viewer switches)                              " << endl;
  cout << "   center_view                 true            " << endl; 
  cout << "   center_view_sticky          false           " << endl; 
  cout << "   hash_view                   false           " << endl; 
  cout << "   hash_delta                  100             " << endl; 
  cout << "   hash_shade                  0.65            " << endl; 
  cout << "(vehicles)                                     " << endl;
  cout << "   trails_color                white           " << endl;
  cout << "   trails_connect_viewable     false           " << endl;
  cout << "   trails_history_size         10,000          " << endl;
  cout << "   trails_length               100             " << endl;
  cout << "   trails_point_size           1               " << endl;
  cout << "   trails_viewable             true            " << endl;
  cout << "   vehicle_active_color        red             " << endl;
  cout << "   vehicles_inactive_color     yellow          " << endl;
  cout << "   vehicles_name_active        a known name    " << endl;
  cout << "   vehicles_name_center        a known name    " << endl;
  cout << "   vehicles_name_color         white           " << endl;
  cout << "   vehicles_name_viewable      false           " << endl;
  cout << "   vehicles_shape_scale        0.1             " << endl;
  cout << "   vehicles_viewable           true            " << endl;
  cout << "(geoshapes)                                    " << endl;
  cout << "   circle_edge_width           2.0             " << endl;
  cout << "   circle_edge_color           yellow          " << endl;
  cout << "   grid_edge_color             white           " << endl;
  cout << "   grid_edge_width             2.0             " << endl;
  cout << "   point_vertex_color          yellow          " << endl;
  cout << "   point_vertex_size           4.0             " << endl;
  cout << "   point_viewable_all          true            " << endl;
  cout << "   point_viewable_labels       true            " << endl;
  cout << "   polygon_edge_color          yellow          " << endl;
  cout << "   polygon_edge_width          1.0             " << endl;
  cout << "   polygon_label_color         khaki           " << endl;
  cout << "   polygon_viewable_all        true            " << endl;
  cout << "   polygon_viewable_labels     true            " << endl;
  cout << "   polygon_vertex_size         5.0             " << endl;
  cout << "   polygon_vertex_color        red             " << endl;
  cout << "   seglist_edge_color          white           " << endl;
  cout << "   seglist_edge_width          1.0             " << endl;
  cout << "   seglist_label_color         orange          " << endl;
  cout << "   seglist_viewable_all        true            " << endl;
  cout << "   seglist_viewable_labels     true            " << endl;
  cout << "   seglist_vertex_color        dark_blue       " << endl;
  cout << "   seglist_vertex_size         2.0             " << endl;
  cout << "(markers)                                      " << endl;
  cout << "   markers_viewable            true            " << endl;
  cout << "   markers_labels_viewable     true            " << endl;
  cout << "   markers_scale_global        1               " << endl;
  cout << "   markers_label_color         white           " << endl;
  exit(-1);
}


//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("pMarineViewer");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  string mission_file = "";

  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help"))
    exit_with_usage();
  
  bool switches = false;
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argv[i];
    else if(strEnds(argi, "._moos"))
      mission_file = argv[i];
    else if(strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strContains(argi, "-sw"))
      switches = true;
    else if(!strContains(argi, "pMarineViewer"))
      exit_with_usage();
  }
  
  if(switches)
    exit_with_switches();

  if(mission_file == "")
    exit_with_usage();
  
  // For document screen shots:
  // PMV_GUI* gui = new PMV_GUI(1100,640, "pMarineViewer");

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

  MOOSAppRunnerThread portAppRunnerThread(&thePort, appFilename, mission_file.c_str());

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

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

using namespace std;

// ----------------------------------------------------------
// global variables here
//

char*        g_sMissionFile = 0;
PMV_MOOSApp  g_thePort;
pthread_t    g_portThreadID;

struct ThreadParams {
    CMOOSApp *app;
    char *name;
};

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
  cout << "(vehicles)                                     " << endl;
  cout << "   vehicle_shape_scale         0.1             " << endl;
  cout << "   trails_point_size           1               " << endl;
  cout << "   trails_length               100             " << endl;
  cout << "   trails_gap                  1               " << endl;
  cout << "   trails_viewable             true            " << endl;
  cout << "   trails_connect_viewable     false           " << endl;
  cout << "   vehicles_viewable           true            " << endl;
  cout << "   vehicle_names_viewable      false           " << endl;
  cout << "   trails_color                white           " << endl;
  cout << "   active_vehicle_color        red             " << endl;
  cout << "   inactive_vehicle_color      yellow          " << endl;
  cout << "   vehicle_names_color         white           " << endl;
  cout << "(geoshapes)                                    " << endl;
  cout << "   polygon_edge_color          yellow          " << endl;
  cout << "   polygon_vertex_color        red             " << endl;
  cout << "   polygon_label_color         khaki           " << endl;
  cout << "   seglist_edge_color          white           " << endl;
  cout << "   seglist_vertex_color        dark_blue       " << endl;
  cout << "   seglist_label_color         orange          " << endl;
  cout << "   circle_edge_color           yellow          " << endl;
  cout << "   grid_edge_color             white           " << endl;
  cout << "   point_vertex_color          yellow          " << endl;
  cout << "   polygon_viewable_all        true            " << endl;
  cout << "   polygon_viewable_labels     true            " << endl;
  cout << "   seglist_viewable_all        true            " << endl;
  cout << "   seglist_viewable_labels     true            " << endl;
  cout << "   point_viewable_all          true            " << endl;
  cout << "   point_viewable_labels       true            " << endl;
  cout << "   polygon_edge_width          1.0             " << endl;
  cout << "   polygon_vertex_size         5.0             " << endl;
  cout << "   seglist_edge_width          1.0             " << endl;
  cout << "   seglist_vertex_size         2.0             " << endl;
  cout << "   grid_edge_width             2.0             " << endl;
  cout << "   circle_edge_width           2.0             " << endl;
  cout << "   point_vertex_size           4.0             " << endl;
  cout << "(markers)                                      " << endl;
  cout << "   markers_viewable            true            " << endl;
  cout << "   markers_labels_viewable     true            " << endl;
  cout << "   markers_scale_global        1               " << endl;
  cout << "   markers_label_color         white           " << endl;
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
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("pMarineViewer");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help"))
    exit_with_usage();

  bool switches = false;
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strContains(argi, ".moos"))
      g_sMissionFile = argv[i];
    else if(strContains(argi, "-sw"))
      switches = true;
    else if(!strContains(argi, "pMarineViewer"))
      exit_with_usage();
  }
  
  if(switches)
    exit_with_switches();

  if(g_sMissionFile == 0)
    exit_with_usage();
  

  PMV_GUI* gui = new PMV_GUI(1100,850, "pMarineViewer");
  if(!gui) {
    cout << "Unable to instantiate the GUI - exiting." << endl;
    return(-1);
  }

  g_thePort.setGUI(gui);

  // start the MOOSPort in its own thread
  ThreadParams params = {&g_thePort, argv[0]};
  g_portThreadID = spawn_thread(&params);	

  Fl::add_idle(idleProc);

  return Fl::run();
}



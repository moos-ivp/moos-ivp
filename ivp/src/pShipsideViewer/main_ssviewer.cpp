/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main_ssviewer.cpp                                    */
/*    DATE: September 11th, 2007                                 */
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
#include "SSV_MOOSApp.h"
#include "SSV_GUI.h"
#include "MBUtils.h"

using namespace std;

// ----------------------------------------------------------
// global variables here
//

char*        g_sMissionFile = 0;
SSV_MOOSApp  g_thePort;
pthread_t    g_portThreadID;

struct ThreadParams {
    CMOOSApp *app;
    char *name;
};

//--------------------------------------------------------
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: pShipsideViewer file.moos [file.tif] [-noimg]" << endl;
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
  string viewer_size = "large";

  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strContains(argi, ".moos"))
      g_sMissionFile = argv[i];
    else if(strContains(argi, "-med"))
      viewer_size = "medium";
    else if(strContains(argi, "-small"))
      viewer_size = "small";
    else if(argi != "pShipsideViewer")
      exit_with_usage();
  }
  
  if(g_sMissionFile == 0)
    exit_with_usage();
  
  cout << "vsize:" << viewer_size << endl;


  SSV_GUI* gui = 0;
  if(viewer_size == "small")
    gui = new SSV_GUI(1050,850, "ShipSideViewer");
  else if(viewer_size == "medium")
    gui = new SSV_GUI(1190,935, "ShipSideViewer");
  else 
    gui = new SSV_GUI(1400,1100, "ShipSideViewer");


  gui->readTiff("Default.tif");
  gui->readTiffB("DefaultB.tif");

  g_thePort.setGUI(gui);

  // start the MOOSPort in its own thread
  ThreadParams params = {&g_thePort, "pShipsideViewer"};
  g_portThreadID = spawn_thread(&params);	

  Fl::add_idle(idleProc);

  return Fl::run();
}


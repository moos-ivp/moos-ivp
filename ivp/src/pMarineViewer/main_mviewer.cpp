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
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: pMarineViewer file.moos [file.tif] [-noimg]" << endl;
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
  string tif_file_a = "";
  string tif_file_b = "";

  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strContains(argi, ".moos"))
      g_sMissionFile = argv[i];
    else if(strContains(argi, ".tif")) {
      if(tif_file_a == "")
	tif_file_a = argv[i];
      else if(tif_file_b == "")
	tif_file_b = argv[i];
    }
    else if(strContains(argi, "-noimg")) {
      tif_file_a = "";
      tif_file_b = "";
    }
    else 
      if(!strContains(argi, "pMarineViewer"))
	exit_with_usage();
  }
  
  if(g_sMissionFile == 0)
    exit_with_usage();
  

  PMV_GUI* gui = new PMV_GUI(1100,850, "pMarineViewer");
  if(!gui) {
    cout << "Unable to instantiate the GUI - exiting." << endl;
    return(-1);
  }

  if(tif_file_a == "")
    tif_file_a = "Default.tif";
  if(tif_file_b == "")
    tif_file_b = "DefaultB.tif";

  cout << "Reading tiff-file: " << tif_file_a << endl;
  bool oka = gui->readTiff(tif_file_a);
  if(oka) 
    cout << "Successfully read the tiff-file. Proceeding..." << endl;
  else
    cout << "Could not read tiff-file. Continuing without it...." << endl;

  
  cout << "Reading the 2nd, optional tiff-file: " << tif_file_b << endl;
  bool okb = gui->readTiffB(tif_file_b);
  if(oka) 
    cout << "Successfully read the 2nd tiff-file. Proceeding..." << endl;
  else
    cout << "Did not read 2nd tiff-file. Continuing without it...." << endl;

  g_thePort.setGUI(gui);

  // start the MOOSPort in its own thread
  ThreadParams params = {&g_thePort, "pMarineViewer"};
  g_portThreadID = spawn_thread(&params);	

  Fl::add_idle(idleProc);

  return Fl::run();
}



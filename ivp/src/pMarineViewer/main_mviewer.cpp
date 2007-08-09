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
#include "MOOSAppRunnerThread.h"

using namespace std;

// ----------------------------------------------------------
// global variables here
//

char*        g_sMissionFile = 0;
PMV_MOOSApp  g_thePort;

//--------------------------------------------------------
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: pMarineViewer file.moos [file.tif] [-noimg]" << endl;
  exit(-1);
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
  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    if(strContains(argi, ".moos"))
      g_sMissionFile = argv[i];
    else if(argi == "pMarineViewer") 
      cout << "pAntler is WHACKED!!!" << endl << flush;
    else 
      exit_with_usage();
  }
  
  if(g_sMissionFile == 0)
    exit_with_usage();
  
  // start the MOOSPort in its own thread
  MOOSAppRunnerThread runner(&g_thePort, "pMarineViewer", g_sMissionFile);

  Fl::add_idle(idleProc);

  PMV_GUI* gui = new PMV_GUI(900,750, "Marine-Viewer");
  g_thePort.setGUI(gui);

  return Fl::run();
}


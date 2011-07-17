/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "LogViewLauncher.h"

using namespace std;

void help_message();

REPLAY_GUI* gui = 0;

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  if(gui) gui->conditional_step();
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogview", "gpl");
    return(0);
  }
  
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    help_message();
    return(0);
  }
 
  LogViewLauncher launcher;
  gui = launcher.launch(argc, argv);
  
  // Set up some convenient initial conditions
  if(gui) {
    // It's only polite to center the view on the center of the 
    // bounding box of the total vehicle trails.
    gui->cmviewer->setParam("center_view", "average");

    // Let the GUI choose a convenient non-zero start time.
    gui->setCurrTime();
  }


  if(gui) {
    Fl::add_idle(idleProc);
    return(Fl::run());
  }
  else
    return(0);
}

//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << "Usage:                                                    " << endl;
  cout << "  alogview file.alog [another_file.alog] [OPTIONS]        " << endl;
  cout << "                                                          " << endl;
  cout << "Synopsis:                                                 " << endl;
  cout << "  Renders vehicle paths from multiple MOOS .alog files.   " << endl;
  cout << "  Renders time-series plots for any logged numerical data." << endl;
  cout << "  Renders IvP Helm mode information vs. vehicle position. " << endl;
  cout << "  Renders IvP Helm behavior objective functions.          " << endl;
  cout << "                                                          " << endl;
  cout << "Standard Arguments:                                       " << endl;
  cout << "  file.alog - The input logfile.                          " << endl;
  cout << "                                                          " << endl;
  cout << "Options:                                                  " << endl;
  cout << "  -h,--help       Displays this help message              " << endl;
  cout << "  -v,--version    Displays the current release version    " << endl;
  cout << "  --mintime=val   Clip data with timestamps < val         " << endl;
  cout << "  --maxtime=val   Clip data with timestamps > val         " << endl;
  cout << "  --nowtime=val   Set the initial startup time            " << endl;
  cout << "  --geometry=val  Viewer window pixel size in HGTxWID     " << endl;
  cout << "                  or large, medium, small, xsmall         " << endl;
  cout << "                  Default size is 1400x1100 (large)       " << endl;
  //cout << "  --image                                                 " << endl;
  //cout << "  --ipfs                                                  " << endl;
  cout << "                                                          " << endl;
  cout << "Further Notes:                                            " << endl;
  cout << "  (1) Multiple .alog files ok - typically one per vehicle " << endl;
  cout << "  (2) Non alog files will be scanned for polygons         " << endl;
  cout << "  (3) See also: alogscan, alogrm, alogclip, aloggrep      " << endl;
  cout << endl;
}



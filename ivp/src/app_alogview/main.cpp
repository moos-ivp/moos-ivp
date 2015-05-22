/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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

void idleProc(void*);

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  if(gui) gui->conditionalStep();
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  LogViewLauncher launcher;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version")) {
      showReleaseInfo("alogview", "gpl");
      return(0);
    }
    else if((argi == "-h") || (argi == "--help") || (argi=="-help")) {
      help_message();
      return(0);
    }
  }

  gui = launcher.launch(argc, argv);

  if(gui) {
    gui->updateXY();
    gui->resizeWidgetsShape();
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
  cout << "Usage:                                                         " << endl;
  cout << "  alogview file.alog [another_file.alog] [OPTIONS]             " << endl;
  cout << "                                                               " << endl;
  cout << "Synopsis:                                                      " << endl;
  cout << "  Renders vehicle paths from multiple MOOS .alog files. Replay " << endl;
  cout << "  logged data or step through manually. Supports several       " << endl;
  cout << "  specialized pop-up windows for viewing helm state, objective " << endl;
  cout << "  functions, any logged variable across vehicles. If multiple  " << endl;
  cout << "  alog files are given, they will synchronized. Upon launch,   " << endl;
  cout << "  the original alog files are split into dedicated directories  " << endl;
  cout << "  to cache data base on the MOOS variable name.                " << endl;
  cout << "                                                               " << endl;
  cout << "Standard Arguments:                                            " << endl;
  cout << "  file.alog - The input logfile.                               " << endl;
  cout << "                                                               " << endl;
  cout << "Options:                                                       " << endl;
  cout << "  -h,--help       Displays this help message                   " << endl;
  cout << "  -v,--version    Displays the current release version         " << endl;
  cout << "                                                               " << endl;
  cout << "  --bg=file.tiff  Specify an alternate background image.       " << endl;
  cout << "                                                               " << endl;
  cout << "  --lp=VEH:VAR    Specify starting left log plot.              " << endl;
  cout << "  --rp=VEH:VAR    Specify starting right log plot.             " << endl;
  cout << "                  Example: --lp=henry:NAV_X                    " << endl;
  cout << "                  Example: --rp=NAV_SPEED                      " << endl;
  cout << "                                                               " << endl;
  cout << "  --nowtime=val   Set the initial startup time                 " << endl;
  cout << "  --mintime=val   Clip all times/vals below this time          " << endl;
  cout << "  --maxtime=val   Clip all times/vals above this time          " << endl;
  cout << "                                                               " << endl;
  cout << "  --quick,-q      Quick start (no geo shapes, logplots)        " << endl;
  cout << "  --altnav=PREF   Alt nav solution prefix, e.g., NAV_GT_       " << endl;
  cout << "                                                               " << endl;
  cout << "  --zoom=val      Set initial zoom value (default: 1)          " << endl;
  cout << "  --panx=val      Set initial panx value (default: 0)          " << endl;
  cout << "  --pany=val      Set initial pany value (default: 0)          " << endl;
  cout << "                                                               " << endl;
  cout << "  --geometry=xsmall  Open GUI with dimensions 770x605          " << endl;
  cout << "  --geometry=small   Open GUI with dimensions 980x770          " << endl;
  cout << "  --geometry=medium  Open GUI with dimensions 1190x935         " << endl;
  cout << "  --geometry=large   Open GUI with dimensions 1400x1100        " << endl;
  cout << "  --geometry=WxH     Open GUI with dimensions WxH              " << endl;
  cout << "                                                               " << endl;
  cout << "Further Notes:                                                 " << endl;
  cout << "  (1) Multiple .alog files ok - typically one per vehicle      " << endl;
  cout << "  (2) See also: alogscan, alogrm, alogclip, aloggrep           " << endl;
  cout << "      alogsort, alogiter, aloghelm                             " << endl;
  cout << endl;
}

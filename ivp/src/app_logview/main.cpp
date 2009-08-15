/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include <stdlib.h>
#include <string.h>
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "LogViewLauncher.h"

using namespace std;

void help_message();
void version_message();

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
  LogViewLauncher launcher;
  gui = launcher.launch(argc, argv);
  
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
  cout << endl;
  cout << "Usage: logview file1.alog [file2.alog] [-h] [--help] " << endl;
  cout << "   At least one .alog file must be provided " << endl;
  cout << "   Non alog files will be scanned for polygons " << endl;
  cout << endl;
}

//--------------------------------------------------------
// Procedure: version_message()

void version_message()
{
  vector<string> svector = getReleaseInfo("logview");
  for(int j=0; j<svector.size(); j++)
    cout << svector[j] << endl;
}

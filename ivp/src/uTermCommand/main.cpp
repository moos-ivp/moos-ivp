/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 26th 2007                                       */
/*          Aug  25th 2011 Mods                                  */
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

#include <cstring>
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "TermCommand.h"
#include "ColorParse.h"
#include "UTC_ExampleConfig.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("uTermCommand", "gpl");
    return(0);
  }

  // Look for a request for example configuration information
  if(scanArgs(argc, argv, "-e", "--example", "-example")) {
    showExampleConfig();
    return(0);
  }

  int    i;
  string mission_file = "";
  string run_command  = argv[0];
  bool   help_requested = false;
  string verbose_setting;

  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".moos"))
      mission_file = argv[i];
    else if(strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if((argi == "--help")||(argi=="-h"))
      help_requested = true;
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(strBegins(argi, "--verbose="))
      verbose_setting = tolower(argi.substr(10));
  }
  
  if((mission_file == "") || help_requested) {
    cout << "Usage: uTermCommand file.moos [OPTIONS]                " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --alias=<ProcessName>                                " << endl;
    cout << "      Launch uTermCommand with the given process name. " << endl;
    cout << "  --example, -e                                        " << endl;
    cout << "      Display example MOOS configuration block         " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --verbose=Boolean (true/false)                       " << endl;
    cout << "      Display diagnostics messages. Default is true.   " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of uTermCommand.     " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "uTermCommand running as: " << run_command << endl;
  cout << termColor() << endl;

  TermCommand  theTermCommand;
  MOOSAppRunnerThread appThread(& theTermCommand, run_command.c_str(), 
				mission_file.c_str());

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3)))   // ASCII 03 is control-c
      quit = true;
    else {
      theTermCommand.m_tc_mutex.Lock();
      theTermCommand.handleCharInput(c);
      theTermCommand.m_tc_mutex.UnLock();
    }
  }

  appThread.quit();
  return(0);
}



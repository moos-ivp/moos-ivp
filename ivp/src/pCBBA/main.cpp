/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Aug 17th 2011 (Waterloo)                             */
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
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CBA_MOOSApp.h"
#include "MBUtils.h"
#include "CBA_ExampleConfig.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("pCBBA", "gpl");
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
    cout << "Usage: pCBBA file.moos [OPTIONS]                       " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --alias=<ProcessName>                                " << endl;
    cout << "      Launch pCBBA with the given process              " << endl;
    cout << "      name rather than pCBBA.                          " << endl;
    cout << "  --example, -e                                        " << endl;
    cout << "      Display example MOOS configuration block         " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --verbose=Boolean (true/false)                       " << endl;
    cout << "      Display diagnostics messages. Default is true.   " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of pCBBA.  " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "pCBBA running as: " << run_command << endl;
  cout << termColor() << endl;

  CBA_MOOSApp cbba_app;

  //if(verbose_setting != "")
  //  sim_active_sonar.setVerbose(verbose_setting);

  cbba_app.Run(run_command.c_str(), mission_file.c_str());
 
  return(0);
}



/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 21 2009                                          */
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

#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TS_MOOSApp.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "ColorParse.h"
#include "TS_ExampleConfig.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("uTimerScript", "gpl");
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
  bool   help_requested  = false;
  bool   verbose_setting = true;
  bool   shuffle_setting = true;

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
    else if(strBegins(argi, "--verbose=")) {
      string val = tolower(argi.substr(10));
      setBooleanOnString(verbose_setting, val);
    }
    else if(strBegins(argi, "--shuffle=")) {
      string val = tolower(argi.substr(10));
      setBooleanOnString(shuffle_setting, val);
    }
  }
  
  if((mission_file == "") || help_requested) {
    cout << "Usage: uTimerScript file.moos [OPTIONS]                " << endl;
    cout << "                                                       " << endl;
    cout << "Options:                                               " << endl;
    cout << "  --alias=<ProcessName>                                " << endl;
    cout << "      Launch uTimerScript with the given process name  " << endl;
    cout << "      rather than uTimerScript.                        " << endl;
    cout << "  --example, -e                                        " << endl;
    cout << "      Display example MOOS configuration block         " << endl;
    cout << "  --help, -h                                           " << endl;
    cout << "      Display this help message.                       " << endl;
    cout << "  --shuffle=Boolean (true/false)                       " << endl;
    cout << "      If true, script is recalculated on each reset.   " << endl;
    cout << "      If event times configured with random range, the " << endl;
    cout << "      ordering may change after a reset.               " << endl;
    cout << "      The default is true.                             " << endl;
    cout << "  --verbose=Boolean (true/false)                       " << endl;
    cout << "      Display script progress and diagnostics if true. " << endl;
    cout << "      The default is true.                             " << endl;
    cout << "  --version,-v                                         " << endl;
    cout << "      Display the release version of uTimerScript.     " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "uTimerScript running as: " << run_command << endl;
  cout << termColor() << endl;

  TS_MOOSApp timer_script;
  timer_script.setVerbose(verbose_setting);
  timer_script.setShuffle(shuffle_setting);
  timer_script.Run(run_command.c_str(), mission_file.c_str());

  return(0);
}



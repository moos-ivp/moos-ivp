/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: main.cpp                                        */
/*    DATE: May 21 2009                                     */
/************************************************************/

#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TS_MOOSApp.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uTimerScript");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
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
    cout << "  --version,-h                                         " << endl;
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


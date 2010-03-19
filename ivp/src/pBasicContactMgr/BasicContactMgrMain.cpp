/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: BasicContactMgrMain.cpp                         */
/*    DATE: Feb 27th 2010                                   */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "BasicContactMgr.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("pBasicContactMgr");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  int    i;
  string mission_file = "";
  string run_command  = argv[0];
  bool   help_requested  = false;
  bool   verbose_setting = true;

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
  }
  
  if((mission_file == "") || help_requested) {
    cout << "Usage: pBasicContactMgr file.moos [OPTIONS]               " << endl;
    cout << "                                                          " << endl;
    cout << "Options:                                                  " << endl;
    cout << "  --alias=<ProcessName>                                   " << endl;
    cout << "      Launch pBasicContactMgr with the given process name " << endl;
    cout << "      rather than pBasicContactMgr.                       " << endl;
    cout << "  --help, -h                                              " << endl;
    cout << "      Display this help message.                          " << endl;
    cout << "  --verbose=<Boolean>                                     " << endl;
    cout << "      Display status updates and diagnostics if true.     " << endl;
    cout << "      The default is true.                                " << endl;
    cout << "  --version,-h                                            " << endl;
    cout << "      Display the release version of pBasicContactMgr.    " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "pBasicContactMgr running as: " << run_command << endl;
  cout << termColor() << endl;

  BasicContactMgr BasicContactMgr;
	
  BasicContactMgr.setVerbose(verbose_setting);
  BasicContactMgr.Run(run_command.c_str(), mission_file.c_str());

  return(0);
}


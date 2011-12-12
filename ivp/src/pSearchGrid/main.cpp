/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: main.cpp                                             */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "PSG_ExampleConfig.h"
#include "PSG_MOOSApp.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file = "";
  string run_command  = argv[0];
  bool   help_requested = false;
  bool   vers_requested = false;
  bool   exam_requested = false;

  int    i;
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      vers_requested = true;
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      exam_requested = true;
    else if((argi == "--help")||(argi=="-h"))
      help_requested = true;
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    // If arg is otherwise not recognized and it happens to be argv[2]
    // then interpret this as a run_command alias by pAntler convention.
    else if(i ==2 )
      run_command = argi;
  }
  
  if(vers_requested) {
    showReleaseInfo("pSearchGrid", "gpl");
    return(0);
  }

  if(exam_requested) {
    showExampleConfig();
    return(0);
  }

  if((mission_file == "") || help_requested) {
    cout << "Usage: pSearchGrid file.moos [OPTIONS]              " << endl;
    cout << "                                                    " << endl;
    cout << "Options:                                            " << endl;
    cout << "  --alias=<ProcessName>                             " << endl;
    cout << "      Launch pSearchGrid with the given process     " << endl;
    cout << "      name rather than pSearchGrid.                 " << endl;
    cout << "  --example, -e                                     " << endl;
    cout << "      Display example MOOS configuration block.     " << endl;
    cout << "  --help, -h                                        " << endl;
    cout << "      Display this help message.                    " << endl;
    cout << "  --version,-v                                      " << endl;
    cout << "      Display the release version of pSearchGrid.   " << endl;
    cout << "Note: If argv[2] is not of one of the above formats " << endl;
    cout << "      this will be interpreted as a run alias. This " << endl;
    cout << "      is to support pAntler launching conventions.  " << endl;
    return(0);
  }

  cout << termColor("green");
  cout << "pSearchGrid running as: " << run_command << endl;
  cout << termColor() << endl;

  PSG_MOOSApp SearchGrid;

  SearchGrid.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}


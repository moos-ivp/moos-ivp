/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: main.cpp                                             */
/*   DATE: Sep 14th 2014                                        */
/****************************************************************/

#include "TermUtils.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "DiffThrust_ExampleConfig.h"
#include "DiffThrust.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

int main(int argc, char *argv[])
{
  DiffThrust DiffThrust;

  string mission_file;
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
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      help_requested = true;
    else if((argi=="-tmd") || (argi == "--post_thrust_mode_differential_true"))
      DiffThrust.setPostThrustModeDif(true);
    else if((argi=="-tmr") || (argi == "--post_thrust_mode_reverse_true"))
      DiffThrust.setPostThrustModeRev(true);
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    // If argument is otherwise not recognized and it is argv[2]
    // then interpret this a run_command alias by pAntler convention.
    else if(i == 2)
      run_command = argi;
  }
  
  if(vers_requested) {
    showReleaseInfo("iDiffThrust", "gpl");
    return(0);
  }

  if(exam_requested) {
    showExampleConfig();
    return(0);
  }

  if((mission_file == "") || help_requested) {
    cout << "Usage: iDiffThrust file.moos [OPTIONS]               " << endl;
    cout << "                                                     " << endl;
    cout << "Options:                                             " << endl;
    cout << "  --alias=<ProcessName>                              " << endl;
    cout << "      Launch iDiffThrust with the given process      " << endl;
    cout << "      name rather than iDiffThrust.                  " << endl;
    cout << "  --example, -e                                      " << endl;
    cout << "      Display example MOOS configuration block.      " << endl;
    cout << "  -tmd, --post_thrust_mode_differential_true         " << endl;
    cout << "      Post THRUST_MODE_DIFFERENTIAL=true upon launch " << endl;
    cout << "  -tmr, --post_thrust_mode_reverse_true              " << endl;
    cout << "      Post THRUST_MODE_REVERSE=true upon launch      " << endl;
    cout << "  --help, -h                                         " << endl;
    cout << "      Display this help message.                     " << endl;
    cout << "  --version,-v                                       " << endl;
    cout << "      Display the release version of iDiffThrust.    " << endl;
    cout << "Note: If argv[2] is not of one of the above formats  " << endl;
    cout << "      this will be interpreted as a run alias. This  " << endl;
    cout << "      is to support pAntler launching conventions.   " << endl;

    return(0);
  }

  cout << termColor("green");
  if(run_command == "iDiffThrust")
    cout << "iDiffThrust Launching... " << endl;
  else
    cout << "iDiffThrust running as: " << run_command << endl;
  cout << termColor() << endl;

  MOOSAppRunnerThread appThread(&DiffThrust, 
				run_command.c_str(), 
				mission_file.c_str(), argc, argv);

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if((c=='q') || (c==(char)(3))) {  // ASCII 03 is control-c
      quit = true;
      cout << "Quitting....." << endl;
    }
    else {
      DiffThrust.m_tc_mutex.Lock();
      DiffThrust.handleCharInput(c);
      DiffThrust.m_tc_mutex.UnLock();
    }
  }

  appThread.quit(); 
  return(0);
}


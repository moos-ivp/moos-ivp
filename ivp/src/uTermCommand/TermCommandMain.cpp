/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommandMain.cpp                                  */
/*    DATE: June 26th 2007                                       */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "TermCommand.h"

  #include "MOOSAppRunnerThread.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

// char*        sMissionFile = 0;
// TermCommand  g_theTermCommand;
// MOOSAppRunnerThread* g_threadID;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    vector<string> svector = getReleaseInfo("uTermCommand");
    for(unsigned int j=0; j<svector.size(); j++)
      cout << svector[j] << endl;    
    return(0);
  }

  // Look for a request for help or usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    MOOSTrace("Usage: uTermCommand moosfile.moos          \n");
    return(0);
  }

  const char * sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      sMissionFile = argv[i];
  }

  if(!sMissionFile) {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n\n");
    return(0);
  }

  TermCommand  theTermCommand;
  MOOSAppRunnerThread appThread(& theTermCommand, argv[0], sMissionFile);

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
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


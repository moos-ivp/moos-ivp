/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommandMain.cpp                                  */
/*    DATE: June 26th 2007                                       */
/*****************************************************************/

#include <cstring>
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

  // Application name connected to MOOSDB is by default argv[0]
  string app_name = argv[0];

  // Look for a request for help or usage information
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--alias=", 8)) {
      string argi  = argv[i];
      string front = biteString(argi, '=');
      string value = argi;
      
      if(value != "") 
	app_name = value;
    }
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
  MOOSAppRunnerThread appThread(& theTermCommand, app_name.c_str(), sMissionFile);

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


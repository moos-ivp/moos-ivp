/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: uXMSMain.cpp                                         */
/*    DATE: May 27th 2007                                        */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "XMS.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "MOOSAppRunnerThread.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

char*       g_sMissionFile = 0;
XMS         g_theXMS;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  bool help_requested = false;

  g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];

    if((str == "-h") || (str == "--help") || (str == "-help"))
      help_requested = true;
  }
  
  if(help_requested) {
    MOOSTrace("Usage: uXMS moosfile.moos VAR-1 VAR-2 ... VAR-N \n");
    return(0);
  }
 
  if(!g_sMissionFile) {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n\n");
    return(0);
  }
 
  bool seed = true;
  if(seed) {
    unsigned long tseed = time(NULL);
    unsigned long hostid = gethostid();
    unsigned long pid = (long)getpid();
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed*hostid)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }

  int rand_int = rand() % 1000;
  string rand_str = intToString(rand_int);

  string process_name = "uXMS_" + rand_str;


  // start the XMS in its own thread
  MOOSAppRunnerThread runner(&g_theXMS, (char*)(process_name.c_str()), 
    g_sMissionFile);

  for(int i=1; i<argc; i++) {
    if(!strcmp(argv[i], "-nav")) {
      g_theXMS.addVariable("NAV_X");
      g_theXMS.addVariable("NAV_Y");
      g_theXMS.addVariable("NAV_HEADING");
      g_theXMS.addVariable("NAV_SPEED");
    }
    else if(!strcmp(argv[i], "-helm")) {
      g_theXMS.addVariable("DESIRED_HEADING");
      g_theXMS.addVariable("DESIRED_SPEED");
      g_theXMS.addVariable("DESIRED_DEPTH");
      g_theXMS.addVariable("MOOS_MANUAL_OVERIDE");
      g_theXMS.addVariable("DEPLOY");
      g_theXMS.addVariable("RETURN");
      g_theXMS.addVariable("STATION_KEEP");
    }
    else if(!strcmp(argv[i], "-proc")) {
      g_theXMS.addVariable("PROC_WATCH_SUMMARY");
      g_theXMS.addVariable("PROC_WATCH_EVENT");
    }
    else if(!strcmp(argv[i], "-proc200")) {
      g_theXMS.addVariable("PROC_SUMMARY_200");
      g_theXMS.addVariable("PROC_EVENT_200");
    }
    else if(!strcmp(argv[i], "-proc201")) {
      g_theXMS.addVariable("PROC_SUMMARY_201");
      g_theXMS.addVariable("PROC_EVENT_201");
    }
    else if(!strcmp(argv[i], "-proc202")) {
      g_theXMS.addVariable("PROC_SUMMARY_202");
      g_theXMS.addVariable("PROC_EVENT_202");
    }
    else if(!strcmp(argv[i], "-proc203")) {
      g_theXMS.addVariable("PROC_SUMMARY_203");
      g_theXMS.addVariable("PROC_EVENT_203");
    }
    else if(!strcmp(argv[i], "-proc204")) {
      g_theXMS.addVariable("PROC_SUMMARY_204");
      g_theXMS.addVariable("PROC_EVENT_204");
    }
    else if(!strcmp(argv[i], "-proc206")) {
      g_theXMS.addVariable("PROC_SUMMARY_206");
      g_theXMS.addVariable("PROC_EVENT_206");
    }
    else if(!strContains(argv[i], ".moos"))
      g_theXMS.addVariable(argv[i]);

  }

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else
      g_theXMS.handleCommand(c);
  }
 
  return(0);
}






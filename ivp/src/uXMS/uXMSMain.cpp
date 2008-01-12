/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: uXMSMain.cpp                                         */
/*    DATE: May 27th 2007                                        */
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
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "XMS.h"
#include "TermUtils.h"
#include "MBUtils.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

char*       g_sMissionFile = 0;
XMS         g_theXMS;
pthread_t   g_threadID;

struct ThreadParams {
    CMOOSApp *app;
    char *name;
};

//--------------------------------------------------------
// Procedure: RunProc

void* RunProc(void *lpParameter)
{
  void **params = (void **)lpParameter;
  
  CMOOSApp *app = (CMOOSApp *)params[0];
  char *name = (char *) params[1];
  
  MOOSTrace("starting %s thread\n", name);
  app->Run(name, g_sMissionFile);	
  
  return(NULL);
}

//--------------------------------------------------------
// Procedure: spawn_thread

pthread_t spawn_thread(ThreadParams *pParams)
{
  pthread_t tid;
  if(pthread_create(&tid,NULL, RunProc, pParams) != 0) {
    MOOSTrace("failed to start %s thread\n", pParams->name);
    tid = (pthread_t) -1;
  }
  else 
    MOOSTrace("%s thread spawned\n", pParams->name);
  
  return(tid);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  bool help_requested   = false;
  bool ignore_file_vars = false;

  g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];

    if((str == "-h") || (str == "--help") || (str == "-help"))
      help_requested = true;
  }
  
  if(help_requested) {
    MOOSTrace("Usage: uXMS moosfile.moos -nav -helm -pid -clean [VARS] \n");
    return(0);
  }
 
  if(!g_sMissionFile) {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n");
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
  ThreadParams params = {&g_theXMS, (char*)(process_name.c_str())};
  g_threadID = spawn_thread(&params);	

  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(str == "-nav") {
      g_theXMS.addVariable("NAV_X");
      g_theXMS.addVariable("NAV_Y");
      g_theXMS.addVariable("NAV_HEADING");
      g_theXMS.addVariable("NAV_SPEED");
      g_theXMS.addVariable("NAV_DEPTH");
    }
    else if(str ==  "-helm") {
      g_theXMS.addVariable("DESIRED_HEADING");
      g_theXMS.addVariable("DESIRED_SPEED");
      g_theXMS.addVariable("DESIRED_DEPTH");
      g_theXMS.addVariable("BHV_WARNING");
      g_theXMS.addVariable("BHV_ERROR");
      g_theXMS.addVariable("HELM_IPF_COUNT");
      g_theXMS.addVariable("HELM_ACTIVE_BHV");
      g_theXMS.addVariable("HELM_NONIDLE_BHV");
      g_theXMS.addVariable("MOOS_MANUAL_OVERIDE");
      g_theXMS.addVariable("DEPLOY");
      g_theXMS.addVariable("RETURN");
      g_theXMS.addVariable("STATION_KEEP");
    }
    else if(str == "-pid") {
      g_theXMS.addVariable("DESIRED_RUDDER");
      g_theXMS.addVariable("DESIRED_THRUST");
      g_theXMS.addVariable("DESIRED_ELEVATOR");
    }
    else if(str == "-proc") {
      g_theXMS.addVariable("PROC_WATCH_SUMMARY");
    }

    else if((str == "-c") || (str == "--clean") || (str == "-clean"))
      g_theXMS.ignoreVars(true);
    
    else if(str == "-r") 
      g_theXMS.setPaused(false);

    else if(str == "-v") 
      g_theXMS.setDispVirgins(false);

    else if(str == "-e") 
      g_theXMS.setDispEmptyStrings(false);

    else if(str == "-s") 
      g_theXMS.setDispSource(true);

    else if(str == "-t") 
      g_theXMS.setDispTime(true);

    else if(str == "-c") 
      g_theXMS.setDispCommunity(true);

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







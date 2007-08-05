/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: uTMSMain.cpp                                         */
/*    DATE: Mar 23rd 2006                                        */
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

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "TMS.h"
#include "TermUtils.h"
#include "MBUtils.h"

// ----------------------------------------------------------
// global variables here
//

char*       g_sMissionFile = 0;
TMS         g_theTMS;
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
  char *sMissionFile = "Example.moos";
  
  if(argc>1)
    g_sMissionFile = argv[1];
  
  for(int i=1; i<argc; i++) {
    if(!strContains(argv[i], ".moos"))
      g_theTMS.addVariable(argv[i], true);
  }

  // start the TMS in its own thread
  ThreadParams params = {&g_theTMS, "uTMS"};
  g_threadID = spawn_thread(&params);	

  bool quit = false;

  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else
      g_theTMS.handleCommand(c);
  }
 
  return(0);
}







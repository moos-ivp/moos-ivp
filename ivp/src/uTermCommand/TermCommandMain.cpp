/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommandMain.cpp                                  */
/*    DATE: June 26th 2007                                       */
/*****************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MOOSThread.h"
#include "TermUtils.h"
#include "MBUtils.h"
#include "MOOSAppRunnerThread.h"
#include "TermCommand.h"

using namespace std;

// ----------------------------------------------------------
// global variables here

char*        g_sMissionFile = 0;
TermCommand  g_theTermCommand;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  g_sMissionFile = 0;
  for(int i=1; i<argc; i++) {
    string str = argv[i];
    if(strContains(str, ".moos"))
      g_sMissionFile = argv[i];
  }

  if(!g_sMissionFile) {
    MOOSTrace("Failed to provide a MOOS (.moos) file... Exiting now.\n\n");
    return(0);
  }

  MOOSAppRunnerThread runner(&g_theTermCommand, "uTermCommand", 
    g_sMissionFile);

  bool quit = false;
  while(!quit) {
    char c = getCharNoWait();
    if(c=='q')
      quit = true;
    else
      g_theTermCommand.handleCharInput(c);
  }
 
  return(0);
}


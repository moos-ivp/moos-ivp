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
#include "MOOSAppRunnerThread.h"

// ----------------------------------------------------------
// global variables here
//

char*       g_sMissionFile = 0;
TMS         g_theTMS;

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

  MOOSAppRunnerThread runner(&g_theTMS, "uTermCommand", g_sMissionFile);

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







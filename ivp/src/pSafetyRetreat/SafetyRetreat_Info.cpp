/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PSR_ExampleConfig.cpp                                */
/*    DATE: Dec 14th 2011                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#include <cstdlib>
#include <iostream>
#include "ReleaseInfo.h"
#include "ColorParse.h"
#include "SafetyRetreat_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pSafetyRetreat application posts a safe loiter position   ");
  blk("  upon a user-defined cue. Presumably this works in coordination");
  blk("  with the IvP Helm, and also could have been implemented as a  ");
  blk("  helm behavior.                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pSafetyRetreat file.moos [OPTIONS]                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pSafetyRetreat with the given process name rather  ");
  blk("      than pSafetyRetreat.                                      ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pSafetyRetreat.            ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pSafetyRetreat Example MOOS Configuration                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pSafetyRetreat                                  ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  polygon  = radial:: x=20, y=-60, radius=20, pts=8, snap=1     ");
  blk("  polygon  = radial:: x=30, y=-100, radius=15, pts=12, snap=1   ");
  blk("  duration = 45                                                 ");
  blk("                                                                ");
  blk("  retreat_cue_var     = THREAT_DETECTED       // default        ");
  blk("  retreat_message_var = UP_RETREAT            // default        ");
  blk("  retreat_notify_var  = SAFETY_RETREAT        // default        ");
  blk("  verbose             = true                  // default        ");
  blk("                                                                ");
  blk("}                                                               ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uSimCurrent INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  THREAT_DETECTED  = jackal                                     ");
  blk("  NAV_X            = 123.4                                      ");
  blk("  NAV_Y            = -987.6                                     ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  UP_RETREAT       = polygon = pts={-20,-40:-40,-75:-20,-110:   ");
  blk("                         20,-110:40,-75:20,-40},label=RETREAT,  ");
  blk("                         edge_color=gray80,vertex_color=green,  ");
  blk("                         vertex_size=0,edge_size=1              ");
  blk("  SAFETY_RETREAT   = true                                       ");
  blk("  CONTACT_RESOLVED = jackal                                     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pSafetyRetreat", "gpl");
  exit(0);
}






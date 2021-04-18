/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EchoVar_Info.cpp                                     */
/*    DATE: Dec 14th 2011                                        */
/*    DATE: Aug 17th 2017 Added Boolean Switching, mikerb        */
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
#include "EchoVar_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pEchoVar tool may be used to subscribe for a variable     ");
  blk("  and re-publish it under a different name. It also may be used ");
  blk("  to pull out certain fields in string publications consisting  ");
  blk("  of comma-separated parameter=value pairs, publishing the new  ");
  blk("  string using different parameters.                            ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pEchoVar file.moos [OPTIONS]                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pEchoVar with the given process name rather        ");
  blk("      than pEchoVar.                                            ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pEchoVar.                  ");
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
  blu("pEchoVar Example MOOS Configuration                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pEchoVar                                        ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  Echo    = MARINESIM_X       ->  NAV_X                         ");
  blk("  Echo    = MARINESIM_Y       ->  NAV_Y                         ");
  blk("  Echo    = MARINESIM_YAW     ->  NAV_YAW                       ");
  blk("  Echo    = MARINESIM_HEADING ->  NAV_HEADING                   ");
  blk("  Echo    = MARINESIM_SPEED   ->  NAV_SPEED                     ");
  blk("                                                                ");
  blk("  Echo    = FOO -> BAR                                          ");
  blk("                                                                ");
  blk("  // If Boolean string published to HOT, opposite Boolean       ");
  blk("  // string will be published to COLD                           ");
  blk("  Echo    = HOT !-> COLD                                        ");
  blk("                                                                ");
  blk("  FLIP:1    = source_variable  = MVIEWER_LCLICK                 ");
  blk("  FLIP:1    = dest_variable    = UP_LOITERA                     ");
  blk("  FLIP:1    = source_separator = ,                              ");
  blk("  FLIP:1    = dest_separator   = #                              ");
  blk("  FLIP:1    = type == redeploy                                  ");
  blk("  FLIP:1    = x -> xcenter_assign                               ");
  blk("  FLIP:1    = y -> xcenter_assign                               ");
  blk("                                                                ");
  blk("  condition = (CHECK1 = hello) or (CHECK2 < 10)                 ");
  blk("                                                                ");
  blk("  hold_messages = false                                         ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pEchoVar INTERFACE                                              ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Subscriptions are determined by which variables the user      ");
  blk("  configures for echoing, and which variables are involved in   ");
  blk("  condition statements.                                         ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Publications are determined by which variables the user       ");
  blk("  configures for echoing.                                       ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pEchoVar", "gpl");
  exit(0);
}









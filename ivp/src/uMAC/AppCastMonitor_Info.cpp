/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppCastMonitor_Info.cpp                              */
/*    DATE: June 4th 2012                                        */
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
#include "AppCastMonitor_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uMAC application is a utility for Monitoring AppCasts.    ");
  blk("  It is launched and run in a terminal window and will parse    ");
  blk("  appcasts generated within its own MOOS community or those from");
  blk("  other MOOS communities bridged or shared to the local MOOSDB. ");
  blk("                                                                ");
  blk("  The primary advantage of uMAC vs. other appcast monitoring    ");
  blk("  tools is that one may remotely log into a vehicle via ssh and ");
  blk("  launch uMAC locally in a terminal.                            ");
  blk("                                                                ");
  blk("  See also: uMACView and uMACView integrated in pMarineViewer.  ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uMAC file.moos [OPTIONS]                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uMAC with the given process name rather than uMAC. ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uMAC.                      ");
  mag("  --terse,-t                                                    ");
  blk("      Super condensed uMAC header output for small terminals    ");
  mag("  --proc","=<ProcessName>                                       ");
  blk("      Specify initial process to scope on.                      ");
  mag("  --node","=<NodeName>                                          ");
  blk("      Specify initial node to scope on.                         ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uMAC ");
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
  blu("uMAC Example MOOS Configuration                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uMAC                                            ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // No parameters defined specific to uMAC                     ");
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
  blu("uMAC INTERFACE                                                  ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  APPCAST = proc=pHostInfo!@#iter=48!@#node=shoreside!@#        ");
  blk("            iter=48!@#messages=...                              ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPACAST_REQ =       node=shoreside,app=any,duration=3.0,     ");
  blk("                       key=pMarineViewer,thresh=run_warning     ");
  blk("  APPACAST_REQ_HENRY = node=henry,app=pHelmIvP,duration=3.0,    ");
  blk("                       key=pMarineViewer,thresh=any             ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uMAC", "gpl");
  exit(0);
}











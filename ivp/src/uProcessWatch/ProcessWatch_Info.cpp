/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ProcessWatch_Info.cpp                                */
/*    DATE: Aug 8th 2011                                         */
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
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "ProcessWatch_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uProcessWatch application monitors the presence of MOOS   ");
  blk("  apps on a watch-list. If one or more are noted to be absent,  ");
  blk("  it will be so noted on the MOOS variable PROC_WATCH_SUMMARY.  ");
  blk("  uProcessWatch is appcast enabled and will produce a succinct  ");
  blk("  table summary of watched processes and the CPU load reported  ");
  blk("  by the processes themselves. The items on the watch list may  ");
  blk("  be named explicitly in the config file or inferred from the   ");
  blk("  Antler block or from list of DB_CLIENTS. An application may be");
  blk("  excluded from the watch list if desired.                      ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uProcessWatch file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uProcessWatch with the given process name rather   ");
  blk("      than uProcessWatch.                                       ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uProcessWatch.             ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uProcessWatch ");
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
  blu("=============================================================== ");
  blu("uProcessWatch Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uProcessWatch                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  watch_all = true   // The default is true.                    ");
  blk("                                                                ");
  blk("  watch  = pMarinePID:PID_OK                                    ");
  blk("  watch  = uSimMarine:USM_OK                                    ");
  blk("                                                                ");
  blk("  nowatch = uXMS*                                               ");
  blk("                                                                ");
  blk("  allow_retractions = true   // Always allow run-warnings to be ");
  blk("                             // retracted if proc re-appears    ");
  blk("                                                                ");
  blk("  // A negative value means summary only when status changes.   ");
  blk("  summary_wait = 10 // Seconds. Default is -1.                  ");
  blk("                                                                ");
  blk("  post_mapping = PROC_WATCH_FULL_SUMMARY, UPW_FULL_SUMMARY      ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
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
  blu("uProcessWatch INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DB_CLIENTS  = uXMS_419,pMarineViewer,pHelmIvP,pMarinePID,     ");
  blk("                uSimMarine,uProcessWatch,pNodeReporter,pLogger, ");
  blk("                DBWebServer,                                    ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  If configured by the user, will publish a dedicated variable  ");
  blk("  for watching a given process, e.g., WATCH=pHelmIvP:HELM_OK    ");
  blk("  will publish the variable HELM_OK regarding pHelmIvP status.  ");
  blk("                                                                ");
  blk("  PROC_WATCH_FULL_SUMMARY = pMarineViewer(1/0),pHelmIvP(1/0),   ");
  blk("                            uSimMarine(1/0),pMarinePID(1/0)     ");
  blk("  PROC_WATCH_SUMMARY      = All Present                         ");
  blk("  PROC_WATCH_EVENT        = Process [pMarinePID] is noted to be ");
  blk("                            present.                            ");
  blk("  PROC_WATCH_ALL_OK       = true/false                          ");
  blk("  PROC_WATCH_TIME_WARP    = 10                                  ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uProcessWatch", "gpl");
  exit(0);
}











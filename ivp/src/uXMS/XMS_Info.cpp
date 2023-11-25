/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XMS_Info.cpp                                         */
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
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "XMS_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  uXMS is a terminal-based (non GUI) tool for scoping a MOOSDB. ");
  blk("  Users may precisely configure the set of variables they wish  ");
  blk("  to scope on by naming them explicitly on the command line or  ");
  blk("  in the MOOS configuration block. The variable set may also be ");
  blk("  configured by naming one or more MOOS proceses on which all   ");
  blk("  variables published by those processes will be scoped.        ");
  blk("  Users may also scope on the *history* of a single variable.   ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uXMS [file.moos] [OPTIONS]                               ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uXMS with the given process name rather than uXMS. ");
  mag("  --all,-a                                                      ");
  blk("    Show ALL MOOS variables in the MOOSDB                       ");
  mag("  --clean,-c                                                    ");
  blk("    Ignore scope variables in file.moos                         ");
  mag("  --colormap","=<MOOSVar>,<color>                               ");
  blk("    Display all entries where the variable, source, or community");
  blk("    has VAR as substring. Allowable olors: blue, red, magenta,  ");
  blk("    cyan, or green.                                             ");
  mag("  --colorany","=<MOOSVar>,<MOOSVar>,...                         ");
  blk("    Display all entries where the variable, community, or source");
  blk("    has VAR as substring. Color auto-chosen from unused colors. ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help,-h                                                     ");
  blk("      Display this help message.                                ");
  mag("  --history=<MOOSVar>                                           ");
  blk("    Allow history-scoping on variable                           ");
  mag("  --interface,-i                                                ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --novirgins,-g                                                ");
  blk("    Don't display virgin variables                              ");
  mag("  --mode","=[paused,EVENTS,streaming]                           ");
  blk("    Determine display mode. Paused: scope updated only on user  ");
  blk("    request. Events: data updated only on change to a scoped    "); 
  blk("    variable. Streaming: updates continuously on each app-tick. ");
  mag("  --serverhost","=<IPAddress>                                   ");
  blk("    Connect to MOOSDB at IP=value, not from the .moos file.     ");
  mag("  --serverport","=<PortNumber>                                  "); 
  blk("    Connect to MOOSDB at port=value, not from the .moos file.   ");
  mag("  --show","=[source,time,community,aux]                         ");
  blk("    Turn on data display in the named column, source, time, or  ");
  blk("    community. All off by default enabling aux shows the        ");
  blk("    auxilliary source in the souce column.                      ");
  mag("  --src","=<MOOSApp>,<MOOSApp>, ...                             ");
  blk("    Scope only on vars posted by the given MOOS processes       ");
  mag("  --trunc","=value [10,1000]                                    ");
  blk("    Truncate the output in the data column.                     ");
  mag("  --termint","=value [0,10]  (default is 0.6)                   ");
  blk("    Minimum real-time seconds between terminal reports.         ");
  mag("  --version,-v                                                  ");
  blk("    Display the release version of uXMS.                        ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uXMS ");
  blk("                                                                ");
  blk("Shortcuts                                                       ");
  blk("                                                                ");
  blk("  -t  Short for --trunc=25                                      ");
  blk("  -p  Short for --mode=paused                                   ");
  blk("  -s  Short for --show=source                                   ");
  blk("  -st Short for --show=source,time                              ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uXMS Example MOOS Configuration                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uXMS                                            ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  var    = NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING                 ");
  blk("  var    = PROC_WATCH_SUMMARY                                   ");
  blk("  var    = PROC_WATCH_EVENT                                     ");
  blk("  source = pHelmIvP, pMarineViewer                              ");
  blk("                                                                ");
  blk("  history_var           = DB_CLIENTS                            ");
  blk("                                                                ");
  blk("  display_virgins       = true    // default                    ");
  blk("  display_source        = false   // default                    ");
  blk("  display_aux_source    = false   // default                    ");
  blk("  display_time          = false   // default                    ");
  blk("  display_community     = false   // default                    ");
  blk("  display_all           = false   // default                    ");
  blk("  trunc_data            = 40      // default is no trucation.   ");
  blk("                                                                ");
  blk("  term_report_interval  = 0.6     // default (seconds)          ");
  blk("                                                                ");
  blk("  colormap    = pHelmIvP, red   // All postings by pHelmIvP red ");
  blk("  colormap    = NAV_SPEED, blue // Only var NAV_SPEED is blue   ");
  blk("                                                                ");
  blk("  refresh_mode = events     // default (or streaming/paused)    ");
  blk("  content_mode = scoping    // default (or history,procs)       ");
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
  blu("uXMS INTERFACE                                                  ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  USER-DEFINED       = Variables added by user to scope list.   ");
  blk("  APPCAST_REQ        = This is an appcast enabled MOOS app.     ");
  blk("  DB_CLIENTS:        = Handle requests to scope on all vars.    ");
  blk("  DB_UPTIME:         = To determine the MOOSDB start time.      ");
  blk("  PROC_WATCH_SUMMARY = List of AWOL apps from uProcessWatch     ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPCAST: This is an appcast enabled MOOS application.         ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uXMS", "gpl");
  exit(0);
}











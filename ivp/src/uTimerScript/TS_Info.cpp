/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TS_Info.cpp                                          */
/*    DATE: July 7th 2011                                        */
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
#include "TS_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Allows the user to script a set of pre-configured pokes to a  ");
  blk("  MOOSDB with each entry in the script happening after a speci- ");
  blk("  fied amount of time. Script may be paused or fast-forwarded.  ");
  blk("  Events may also be configured with random values and happen   ");
  blk("  randomly in a chosen window of time.                          ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                               ");
  blu("============================================================== ");
  blu("Usage: uTimerScript file.moos [OPTIONS]                        ");
  blu("============================================================== ");
  blk("                                                               ");
  showSynopsis();
  blk("                                                               ");
  blk("Options:                                                       ");
  mag("  --alias","=<ProcessName>                                     ");
  blk("      Launch uTimerScript with the given process               ");
  blk("      name rather than uTimerScript.                           ");
  mag("  --example, -e                                                ");
  blk("      Display example MOOS configuration block                 ");
  mag("  --help, -h                                                   ");
  blk("      Display this help message.                               ");
  mag("  --interface, -i                                              ");
  blk("      Display MOOS publications and subscriptions.             ");
  mag("  --shuffle","=Boolean (true/false)                            ");
  blk("      If true, script is recalculated on each reset. If event  ");
  blk("      times configured with random range, the ordering may     ");
  blk("      change after a reset. The default is true.               ");
  mag("  --verbose","=Boolean (true/false)                            ");
  blk("      Display script progress & diagnostics if true.           ");
  blk("      The default is true.                                     ");
  mag("  --version,-v                                                 ");
  blk("      Display the release version of uTimerScript.             ");
  mag("  --noterm_appcast                                             ");
  blk("      Do not generate appcasts to the terminal.                ");
  blk("                                                               ");
  blk("Note: If argv[2] does not otherwise match a known option,      ");
  blk("      then it will be interpreted as a run alias. This is      ");
  blk("      to support pAntler launching conventions.                ");
  blk("                                                               ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uTimerScript Example MOOS Configuration                         ");
  blu("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  blk("                                                                ");
  blk("ProcessConfig = uTimerScript                                    ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // List of apps to block/pause on until they appear to the DB ");
  blk("  block_on       = pGenerateInfo, pPostValues                   ");
  blk("  // Logic condition that must be met for script to be unpaused ");
  blk("  condition      = WIND_GUSTS = true                            ");
  blk("  // Seconds added to each event time, on each script pass      ");
  blu("  delay_reset    = 0                                            ");
  blk("  // Seconds added to each event time, on first pass only       ");
  blu("  delay_start    = 0                                            ");
  blk("  // Event(s) are the key components of the script              ");
  blk("  event          = var=SBR_RANGE_REQUEST, val=\"name=archie\", time=25:35  ");
  blk("  // A MOOS variable for taking cues to forward time            ");
  blu("  forward_var    = UTS_FORWARD  "," // or other MOOS variable   ");
  blk("  // If true script is paused upon launch                       ");
  blu("  paused         = false   "," // or {true}                     ");
  blk("  // A MOOS variable for receiving pause state cues             ");
  blu("  pause_var      = UTS_PAUSE    "," // or other MOOS variable   ");
  blk("  // Declaration of random var macro expanded in event values   ");
  blk("  randvar        = varname=ANG, min=0, max=359, key=at_reset    ");
  blk("  // Maximum number of resets allowed                           ");
  blu("  reset_max      = nolimit "," // or in range [0,inf)           ");
  blk("  // A point when the script is reset                           ");
  blu("  reset_time     = none    "," // or {all-posted} or range (0,inf) ");
  blk("  // A MOOS variable for receiving reset cues                   ");
  blu("  reset_var      = UTS_RESET    "," // or other MOOS variable   ");
  blk("  // If true script will complete if conditions suddenly fail   ");
  blu("  script_atomic  = false   "," // or {true}                     ");
  blk("  // A hopefully unique name given to the script                ");
  blu("  script_name    = unnamed                                      ");
  blk("  // If true timestamps are recalculated on each script reset   ");
  blu("  shuffle        = true                                         ");
  blk("  // If true progress is generated to the console               ");
  blu("  verbose        = true    "," // or {false}                    ");
  blk("  // Reset or restart script upon conditions being met after failure ");
  blu("  upon_awake     = n/a     "," // or {reset,resstart}           ");
  blk("  // A MOOS variable for posting the status summary             ");
  blu("  status_var     = UTS_STATUS   "," // or other MOOS variable   ");
  blk("  // Rate at which time is accelerated in execuing the script   ");
  blu("  time_warp      = 1                                            ");
  blk("  // Base time upon which script event times are based          ");
  blu("  time_zero      = db_start "," // Or script_start (default)    ");
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
  blu("uTimerScript INTERFACE                                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  UTS_NEXT    = next                                            ");
  blk("  UTS_RESET   = reset                                           ");
  blk("  UTS_FORWARD = 10                                              ");
  blk("  UTS_PAUSE   = true                                            ");
  blk("  APPCAST_REQ = node=henry,app=pHostInfo,duration=10,           ");
  blk("                key=uMAC_438                                    ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  The primary publications are the events configured by the     ");
  blk("  user-defined scripts.                                         ");
  blk("                                                                ");
  blk("  UTS_STATUS = name=RND_TEST, elapsed_time=2.00, posted=1,      ");
  blk("               pending=4, paused=false, conditions_ok=true,     ");
  blk("               time_warp=3, start_delay=0, shuffle=false,       ");
  blk("               upon_awake=reset, resets=0/4                     ");
  blk("  APPCAST    = name=uTimerScript!@#iter=917!@#node=hotel!@#     ");
  blk("               messages=...                                     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uTimerScript", "gpl");
  exit(0);
}










/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: QueryDB_Info.cpp                                     */
/*    DATE: Dec 29th 2015                                        */
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
#include "QueryDB_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  uQueryDB is a command-line tool for querying a MOOSDB with a  ");
  blk("  one or more logic condition provided either on the command    ");
  blk("  line or in a mission (.moos) file.                            ");
  blk("  It finds the MOOSDB via mission file provided on the command  ");
  blk("  line, or the IP address and port number given on the command  ");
  blk("  line. It will connect to the DB, register for the variables   ");
  blk("  involved in the logic condition(s) and determine if the       "); 
  blk("  condition holds. It will then exit with 0 if it holds or 1    ");
  blk("  otherwise.                                                    ");
  blk("                                                                ");
  blk("  If a variable in the logic condition is unknown to the MOOSDB ");
  blk("  then the whole condition will fail.                           ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uQueryDB condition [file.moos] [OPTIONS]                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uQueryDB.                  ");
  mag("  --wait=N                                                      ");
  blk("      Wait for N secs before exiting with failure (ret val=1)   ");
  mag("  --host=val                                                    ");
  blk("      Provide MOOSDB IP address on the command line rather than ");
  blk("      from a .moos file.                                        ");
  mag("  --port=val                                                    ");
  blk("      Provide MOOSDB port number on the command line rather than");
  blk("      from a .moos file.                                        ");
  mag("  --condition=val                                               ");
  blk("      Provide a logic pass condition.                           ");
  mag("  --pass_condition=val                                          ");
  blk("      Same as --condition param. Provide a logic pass condition.");
  mag("  --fail_condition=val                                          ");
  blk("      Provide a logic fail condition.                           ");
  mag("  --check_var=MY_RESULT                                         ");
  blk("      Value of named variable to be written to .checkvars       ");
  mag("  --csv                                                         ");
  blk("      Format of .checkvars is comma-separated-value             ");
  mag("  --esv                                                         ");
  blk("      Format of .checkvars is equal-separated-value             ");
  mag("  --wsv                                                         ");
  blk("      Format of .checkvars is whitespace-separated-value        ");
  mag("  --vo,-vo                                                      ");
  blk("      Format of .checkvars is value-only                        ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uQueryDB ");
  blk("                                                                ");
  blk("Returns:                                                        ");
  blk("   1: if not all mail has been received yet for all vars        ");
  blk("      involved in either pass or fail conditions                ");
  blk("   1: One or more pass conditions fail                          ");
  blk("   1: One or more fail conditions are true                      ");
  blk("   0: otherwise (THIS MEANS PASS)                               ");
  blk("                                                                ");
  blk("Returns:                                                        ");
  blk("   0 if all pass conditions met and no fail condition met       ");
  blk("   1 otherwise                                                  ");
  blk("                                                                ");
  blk("Examples:                                                       ");
  blk("   $ uQueryDB alpha.moos                                        ");
  blk("   $ uQueryDB alpha.moos --condition=\"DB_UPTIME > 20\"         ");
  blk("   $ uQueryDB alpha.moos --condition=\"DEPLOY=true\" --wait=10  ");
  blk("   $ uQueryDB --condition=\"DEPLOY=false\" --host=localhost --port=9000");
  blk("   $ uQueryDB alpha.moos --condition=\"((MISSION=complete) or (MISSION=halt))\"");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blu("=============================================================== ");
  blu("uQueryDB Example MOOS Configuration                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uQueryDB                                        ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  Note: uQueryDB exit value:                                    ");
  blk("        1: if not all mail has been received yet for all vars   ");
  blk("           involved in either pass or fail conditions           ");
  blk("        1: One or more pass conditions fail                     ");
  blk("        1: One or more fail conditions are true                 ");
  blk("        0: otherwise (THIS MEANS PASS)                          ");
  blk("                                                                ");
  blk("  // If ALL pass conditions are met, uQueryDB succeeds (ret 0)  ");
  blk("  // If ALL pass conditions are met, uQueryDB succeeds (ret 0)  ");
  blk("                                                                ");
  blk("  pass_condition = MISSION_RESULT = pass                        ");
  blk("  fail_condition = COLLISION = true                             ");
  blk("                                                                ");
  blk("  wait = 5                      // Default is zero seconds      ");
  blk("  halt_max_time = 5             // Same as wait=5               ");
  blk("                                                                ");
  blk("  check_var = MISSION_RESULT                                    ");
  blk("  check_var = DB_UPTIME                                         ");
  blk("                                                                ");
  blk("  check_var_format = csv                                        ");
  blk("}                                                               ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("uQueryDB INTERFACE                                              ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  APPCAST_REQ                                                   ");
  blk("  Any variables from pass or fail logic conditions              ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPCAST                                                       ");
  blk("  No other variables                                             ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uQueryDB", "gpl");
  exit(0);
}











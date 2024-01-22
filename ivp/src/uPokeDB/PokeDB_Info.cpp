/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PokeDB_Info.cpp                                      */
/*    DATE: Dec 25th 2011                                        */
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
#include "PokeDB_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uPokeDB application is a command-line tool for poking a   ");
  blk("  MOOSDB with variable-value pairs provided on the command line.");
  blk("  It finds the MOOSDB via mission file provided on the command  ");
  blk("  line, or the IP address and port number given on the command  ");
  blk("  line. It will connect to the DB, show the value prior to      ");
  blk("  poking, poke the DB, and wait an iteration for mail from the  "); 
  blk("  DB to confirm the result of the poke.                         ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uPokeDB [file.moos] [OPTIONS] [VAR-DATA Pair(s)]         ");
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
  blk("      Display the release version of uPokeDB.                   ");
  mag("  --quiet,-q                                                    ");
  blk("      Silence cout, cerr output.                                ");
  mag("  --host=val                                                    ");
  blk("      Provide MOOSDB IP address on the command line rather than ");
  blk("      from a .moos file.                                        ");
  mag("  --port=val                                                    ");
  blk("      Provide MOOSDB port number on the command line rather than");
  blk("      from a .moos file.                                        ");
  mag("  <var=value>                                                   ");
  blk("      Provide one or more MOOS variable-value pairs to poke to  ");
  blk("      the MOOSDB.                                               ");
  mag("  <var:=value>                                                  ");
  blk("      Poke the variable-value pair as a string even if the value");
  blk("      looks like a double.                                      ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uPokeDB ");
  blk("                                                                ");
  blk("Examples:                                                       ");
  blk("     $ uPokeDB alpha.moos DEPLOY=true RETURN_SPEED=2.0          ");
  blk("     $ uPokeDB DEPLOY=true  --host=localhost --port=9000        ");
  blk("     $ uPokeDB STRING_SPEED:=2.3 alpha.moos                     ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uPokeDB Example MOOS Configuration                              ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uPokeDB                                         ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  poke = DEPLOY_ALL=true                                        ");
  blk("  poke = MOOS_MANUAL_OVERRIDE_ALL=false                         ");
  blk("                                                                ");
  blk("  poke = NUMERICAL_STRING:=88                                   ");
  blk("}                                                               ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uPokeDB INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Variables published by the user are also subscribed for as a  ");
  blk("  way of verifying the results of the poke.                     ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Variables published are those specified by the user.          ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uPokeDB", "gpl");
  exit(0);
}











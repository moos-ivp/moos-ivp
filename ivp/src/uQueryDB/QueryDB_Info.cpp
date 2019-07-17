/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  blk("  logic condition provided on the command line.                 ");
  blk("  It finds the MOOSDB via mission file provided on the command  ");
  blk("  line, or the IP address and port number given on the command  ");
  blk("  line. It will connect to the DB, register for the variables   ");
  blk("  involved in the logic condition and determine if the condition"); 
  blk("  holds. It will then exit with 0 if it holds or 1 otherwise.   ");
  blk("                                                                ");
  blk("  It will return its value as soon as the app has received mail ");
  blk("  for all variables involved in the logic condition. Otherwise  ");
  blk("  it will wait for 10 seconds. This can be changed with the     ");
  blk("  --wait=N parameter. If a variable in the logic condition is   ");
  blk("  unknown to the MOOSDB, then the whole condition will fail     ");
  blk("  after the wait period.                                        ");
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
  mag("  --host=val                                                    ");
  blk("      Provide MOOSDB IP address on the command line rather than ");
  blk("      from a .moos file.                                        ");
  mag("  --port=val                                                    ");
  blk("      Provide MOOSDB port number on the command line rather than");
  blk("      from a .moos file.                                        ");
  mag("  --condition=val                                               ");
  blk("      Provide a logic condition.                                ");
  mag("  --wait=val                                                    ");
  blk("      Specifiy the wait period (default is 10 seconds).         ");
  blk("                                                                ");
  blk("Examples:                                                       ");
  blk("   $ uQueryDB alpha.moos --condition=\"DB_UPTIME > 20\" --wait=5");
  blk("   $ uQueryDB --condition=\"DEPLOY=false\" --host=localhost --port=9000");
  blk("   $ uQueryDB alpha.moos --condition=\"((MISSION=complete) or (MISSION=halt))\"");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uQueryDB Example MOOS Configuration                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  mag("Not Applicable"," - uQueryDB is run from the command line. If a ");
  blk("MOOS file is provided on the command line, it is used solely to ");
  blk("read the ServerHost and ServerPort information. uQueryDB does   ");
  blk("not read a configuration block in the provided MOOS file.       ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uQueryDB INTERFACE                                              ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Any variables involved in the specified logic condition.      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  uQueryDB does not publish anything to the MOOSDB.             ");
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










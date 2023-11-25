/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UFS_Info.cpp                                         */
/*    DATE: Nov 23rd, 2011                                       */
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
#include "UFS_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Typically run in a shoreside community. Takes information from");
  blk("  user-configured set of incoming reports and parses out key    ");
  blk("  information into a concise table format. Reports may be any   ");
  blk("  report in the form of comma-separated parameter-value pairs.  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                           ");
  blu("========================================================== ");
  blu("Usage: uFldScope file.moos [OPTIONS]                       ");
  blu("========================================================== ");
  blk("                                                           ");
  showSynopsis();
  blk("                                                           ");
  blk("Options:                                                   ");
  mag("  --alias","=<ProcessName>                                 ");
  blk("      Launch uFldCommsRegister with the given              ");
  blk("      process name rather than uFldCommsRegister.          ");
  mag("  --example, -e                                            ");
  blk("      Display example MOOS configuration block.            ");
  mag("  --help, -h                                               ");
  blk("      Display this help message.                           ");
  mag("  --interface, -i                                          ");
  blk("      Display MOOS publications and subscriptions.         ");
  mag("  --version,-v                                             ");
  blk("      Display release version of uFldCommsRegister.        ");
  blk("                                                           ");
  blk("Note: If argv[2] does not otherwise match a known option,  ");
  blk("      then it will be interpreted as a run alias. This is  ");
  blk("      to support pAntler launching conventions.            ");
  blk("                                                           ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit                                  

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldScope Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldScope                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  SCOPE = var=NODE_REPORT,fld=TIME,alias=Time                   ");
  blk("  SCOPE = var=NODE_REPORT,key=vname,fld=MODE                    ");
  blk("  SCOPE = var=SPEED_REPORT,key=vname,fld=avg_speed,alias=speed  ");
  blk("  SCOPE = var=ODOMETRY_REPORT,key=vname,fld=trip_dist           ");
  blk("  SCOPE = var=ODOMETRY_REPORT,key=vname,fld=total_dist          ");
  blk("                                                                ");
  blk("  LAYOUT = trip_dist, total_dist                                ");
  blk("  LAYOUT = MODE, speed, Time                                    ");
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
  blu("uFldScope INTERFACE                                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  (Configured by user in .moos file)                            ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  None                                                          ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldScope", "gpl");
  exit(0);
}










